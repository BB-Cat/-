#include "MyFbxManager.h"
#include "GraphicsEngine.h"

#include <vector> 
#include <functional>  



MyFbxManager::MyFbxManager()
{
}

MyFbxManager::~MyFbxManager()
{
}

std::vector<MeshData> MyFbxManager::loadFbxMesh(const wchar_t* filename, float* topology)
{
    //create a vector of meshdata which will be stored in the skinnedmesh and a vector for vertices and indexes that will be reused
    std::vector<MeshData> buffers;
    std::vector<VertexMesh> vertices;
    std::vector<unsigned int> indices;

    std::map<int, int> controlpoints;

    //prepare the shader byte code and size of shader to load vertex and index buffers later
    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

    //create the FBX manager
    FbxManager* m_manager = FbxManager::Create();

    // Create the IO settings object.
    FbxIOSettings* m_io_settings = FbxIOSettings::Create(m_manager, IOSROOT);
    m_manager->SetIOSettings(m_io_settings);

    // Create an importer using the SDK manager.
    FbxImporter* m_importer = FbxImporter::Create(m_manager, "");

    //convert the filename to a character string for FBX SDK
    const wchar_t* input = filename;

    // Count required buffer size (plus one for null-terminator).
    size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
    char* filename_buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
    // wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
    size_t convertedSize;
    std::wcstombs_s(&convertedSize, filename_buffer, size, input, size);
#else
    std::wcstombs(filename_buffer, input, size);
#endif

    // Use the first argument as the filename for the importer.
    if (!m_importer->Initialize(filename_buffer, -1, m_manager->GetIOSettings()))
    {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", m_importer->GetStatus().GetErrorString());
        exit(-1);
    }
    delete filename_buffer;

    // Create a new scene so that it can be populated by the imported file.
    FbxScene* m_scene = FbxScene::Create(m_manager, "myScene");



    // Import the contents of the file into the scene.
    m_importer->Import(m_scene);

    FbxAxisSystem axis_converter;
    axis_converter.ConvertScene(m_scene);

    // The file is imported, so get rid of the importer.
    m_importer->Destroy();

    // Convert mesh, NURBS and patch into triangle mesh
    fbxsdk::FbxGeometryConverter geometry_converter(m_manager);
    geometry_converter.Triangulate(m_scene, true);

    // Fetch node attributes and materials under this node recursively. Currently only mesh.
    std::vector<FbxNode*> fetched_meshes;

    std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
        if (node) {

   

            FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
            if (fbx_node_attribute) {
                switch (fbx_node_attribute->GetAttributeType()) {
                case FbxNodeAttribute::eMesh:
                    fetched_meshes.push_back(node);
                    break;
                }
            }
            for (int i = 0; i < node->GetChildCount(); i++)
                traverse(node->GetChild(i));
        }
    };
    traverse(m_scene->GetRootNode());

    buffers.resize(fetched_meshes.size());

    for (size_t i = 0; i < fetched_meshes.size(); i++)
    {
        FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
        MeshData& mesh = buffers.at(i);

        // Fetch mesh data
        unsigned int vertex_count = 0;
        fbxsdk::FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);


        const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
        buffers[i].m_subs.resize(number_of_materials); // UNIT.18 
        for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
        {
            Subset_FBX& subset = buffers[i].m_subs.at(index_of_material); // UNIT.18 

            const FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);

            //const char* nodeName = fbx_mesh->GetNode()->GetName();

            //// CHECK IF IT IS A HITBOX NODE OR A MESH NODE BASED ON NAME.  IF IT IS A HITBOX GENERATE HITBOX DATA AND CONTINUE ////


            ////-----------------------------------------------------------------------------------------------------------------////

            //////////////////////////////////////////////////////////
            // diffuse properties//
            //////////////////////////////////////////////////////////
            FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);

            if (property.IsValid() && factor.IsValid()) {
                FbxDouble3 color = property.Get<FbxDouble3>();
                double f = factor.Get<FbxDouble>();
                subset.diffuse.m_color.m_x = static_cast<float>(color[0] * f);
                subset.diffuse.m_color.m_y = static_cast<float>(color[1] * f);
                subset.diffuse.m_color.m_z = static_cast<float>(color[2] * f);
            }

            if (property.IsValid())
            {
                const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
                if (number_of_textures) { //!! --- might be able to make this section a loop in order to retrieve bump maps etc
                    const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
                    if (file_texture)
                    {
                        const char* filename = file_texture->GetFileName();
                        const size_t cSize = strlen(filename) + 1;
                        wchar_t* target = new wchar_t[cSize];
                        mbstowcs(target, filename, cSize);
                        subset.diffuse.m_map = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(target);
                    }
                }
            }

            //////////////////////////////////////////////////////////
            // specular properties//
            //////////////////////////////////////////////////////////
            property = surface_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            factor = surface_material->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
            FbxProperty extra = surface_material->FindProperty(FbxSurfaceMaterial::sShininess);

            if (property.IsValid() && factor.IsValid() && extra.IsValid()) {
                FbxDouble3 color = property.Get<FbxDouble3>();
                double f = factor.Get<FbxDouble>();
                subset.specular.m_color.m_x = static_cast<float>(color[0] * f);
                subset.specular.m_color.m_y = static_cast<float>(color[1] * f);
                subset.specular.m_color.m_z = static_cast<float>(color[2] * f);
                subset.specular.m_extra_params = extra.Get<FbxDouble>();
            }

            if (property.IsValid())
            {
                const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
                if (number_of_textures) { //!! --- might be able to make this section a loop in order to retrieve bump maps etc
                    const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
                    if (file_texture)
                    {
                        const char* filename = file_texture->GetFileName();
                        const size_t cSize = strlen(filename) + 1;
                        wchar_t* target = new wchar_t[cSize];
                        mbstowcs(target, filename, cSize);
                        subset.specular.m_map = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(target);
                    }
                }
            }

            //////////////////////////////////////////////////////////
            // emissive properties//
            //////////////////////////////////////////////////////////
            property = surface_material->FindProperty(FbxSurfaceMaterial::sEmissive);
            factor = surface_material->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);

            if (property.IsValid() && factor.IsValid()) {
                FbxDouble3 color = property.Get<FbxDouble3>();
                double f = factor.Get<FbxDouble>();
                subset.emit.m_color.m_x = static_cast<float>(color[0] * f);
                subset.emit.m_color.m_y = static_cast<float>(color[1] * f);
                subset.emit.m_color.m_z = static_cast<float>(color[2] * f);
            }

            if (property.IsValid())
            {
                const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
                if (number_of_textures) { //!! --- might be able to make this section a loop in order to retrieve bump maps etc
                    const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
                    if (file_texture)
                    {
                        const char* filename = file_texture->GetFileName();
                        const size_t cSize = strlen(filename) + 1;
                        wchar_t* target = new wchar_t[cSize];
                        mbstowcs(target, filename, cSize);
                        subset.emit.m_map = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(target);
                    }
                }
            }

            //////////////////////////////////////////////////////////
            // normal map properties//
            //////////////////////////////////////////////////////////
            property = surface_material->FindProperty(FbxSurfaceMaterial::sBump);
            factor = surface_material->FindProperty(FbxSurfaceMaterial::sBumpFactor);

            if (property.IsValid() && factor.IsValid()) {
                double f = factor.Get<FbxDouble>();
                subset.m_bumpiness = static_cast<float>(f);
            }

            if (property.IsValid())
            {
                const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
                if (number_of_textures) { //!! --- might be able to make this section a loop in order to retrieve bump maps etc
                    const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
                    if (file_texture)
                    {
                        const char* filename = file_texture->GetFileName();
                        const size_t cSize = strlen(filename) + 1;
                        wchar_t* target = new wchar_t[cSize];
                        mbstowcs(target, filename, cSize);
                        subset.m_map_normal = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(target);
                    }
                }
            }

        }

        // Count the polygon count of each material  
        if (number_of_materials > 0)
        {
            // Count the faces of each material   
            const int number_of_polygons = fbx_mesh->GetPolygonCount();
            for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
            {
                const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().
                    GetAt(index_of_polygon);
                buffers[i].m_subs.at(material_index).index_count += 3;
            }

            // Record the offset (how many vertex)   
            int offset = 0;
            for (Subset_FBX& subset : buffers[i].m_subs)
            {
                subset.index_start = offset;
                offset += subset.index_count;
                // This will be used as counter in the following procedures, reset to zero    
                subset.index_count = 0;
            }
        }

        const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
        const int number_of_polygons = fbx_mesh->GetPolygonCount();
        indices.resize(number_of_polygons * 3); 

        //retrieve the bone influences for this subset of vertexes
        std::vector<bone_influences_per_control_point> bone_influences;
        fetchBoneInfluences(fbx_mesh, bone_influences, controlpoints, vertices);

        //fbxsdk::FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
        //fbxsdk::FbxTime frame_time;
        //frame_time.SetTime(0, 0, 0, 1, 0, time_mode);

        //fetchAnimations(fbx_mesh, buffers[i].m_skeletons);

        for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
        {
            // The material for current face.   
            int index_of_material = 0;
            if (number_of_materials > 0)
            {
                index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
            }

            // Where should I save the vertex attribute index, according to the material   
            Subset_FBX& subset = buffers[i].m_subs.at(index_of_material);
            const int index_offset = subset.index_start + subset.index_count;

            for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) 
            {

                int control_point_index = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);


                VertexMesh vertex;
                const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
                vertex.m_position.m_x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
                vertex.m_position.m_y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
                vertex.m_position.m_z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
                
                
                
                
                FbxVector4 normal;
                fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
                vertex.m_normal.m_x = static_cast<float>(normal[0]);
                vertex.m_normal.m_y = static_cast<float>(normal[1]); 
                vertex.m_normal.m_z = static_cast<float>(normal[2]); 

                FbxVector2 uv;
                bool unmapped_uv;
                fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
                vertex.m_texcoord.m_x = static_cast<float>(uv[0]);
                vertex.m_texcoord.m_y = (1.0f - static_cast<float>(uv[1]));

                controlpoints[control_point_index] = vertices.size();

                int vector_size = bone_influences.at(index_of_control_point).size();
                for (int i = 0; i < vector_size; ++i)
                {

                    if (vector_size > 4)
                    {
                        throw std::exception("More bone influences than the bone influence maximum were created!");
                    }

                    vertex.m_bone_indices[i] = bone_influences.at(index_of_control_point).at(i).index;
                    vertex.m_bone_influence[i] = bone_influences.at(index_of_control_point).at(i).weight;
                }

                vertices.push_back(vertex);
                //indices.push_back(vertex_count);    
                indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
                vertex_count += 1;
            }
            subset.index_count += 3; // UNIT.18 
        }

        FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);  
        mesh.m_mesh_world = DirectX::XMFLOAT4X4({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
        for (int row = 0; row < 4; row++) 
        { 
            for (int column = 0; column < 4; column++) 
                { 
                 mesh.m_mesh_world.m_mat[row][column] = static_cast<float>(global_transform[row][column]); 
                } 
        }

        //use the vertices and indices to fill the next buffer with mesh information
        buffers[i].m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&vertices[0], sizeof(VertexMesh),
            (UINT)vertices.size(), shader_byte_code, (UINT)size_shader);
        buffers[i].m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

        //reset the vertex and indice buffer
        vertices.clear();
        indices.clear();
    }


    //destroy the manager instance
    m_manager->Destroy();


    return buffers;
}

void MyFbxManager::fetchBoneInfluences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& influences, std::map<int, int>& controlpoints, std::vector<VertexMesh>& vertices)
{
    const int number_of_control_points = fbx_mesh->GetControlPointsCount();   
    influences.resize(number_of_control_points);


    const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);   
    for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer) 
    {
        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

        const int number_of_clusters = skin->GetClusterCount();    
        for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster) 
        {
            FbxCluster* cluster = skin->GetCluster(index_of_cluster);

            const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();     
            const int* array_of_control_point_indices = cluster->GetControlPointIndices();     
            const double* array_of_control_point_weights = cluster->GetControlPointWeights();

            for (int i = 0; i < number_of_control_point_indices; ++i) 
            { 
                bone_influences_per_control_point& influences_per_control_point = influences.at(array_of_control_point_indices[i]);     
                bone_influence influence;      
                influence.index = index_of_cluster;      
                influence.weight = static_cast<float>(array_of_control_point_weights[i]);      
                influences_per_control_point.push_back(influence); 
            }
        }
    }
}

void MyFbxManager::fetchBoneMatrices(FbxMesh* fbx_mesh, Skeleton& skeleton, FbxTime time)
{
    const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);  
    for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer) 
    {

        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

        const int number_of_clusters = skin->GetClusterCount();   
        skeleton.m_bones.resize(number_of_clusters);
        for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster) 
        {
            bone& bone = skeleton.m_bones.at(index_of_cluster);

            FbxCluster* cluster = skin->GetCluster(index_of_cluster);

            // this matrix trnasforms coordinates of the initial pose from mesh space to global space 
            FbxAMatrix reference_global_init_position;    
            cluster->GetTransformMatrix(reference_global_init_position);

            // this matrix trnasforms coordinates of the initial pose from bone space to global space    
            FbxAMatrix cluster_global_init_position;   
            cluster->GetTransformLinkMatrix(cluster_global_init_position); 

            // this matrix trnasforms coordinates of the current pose from bone space to global space    
            FbxAMatrix cluster_global_current_position;    
            cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time); 

            // this matrix trnasforms coordinates of the current pose from mesh space to global space    
            FbxAMatrix reference_global_current_position;    
            reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time); 

            // Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation    // (translation, rotation and scale), the last row of the matrix represents the translation part of the    // transformation.    
            FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position * 
                cluster_global_init_position.Inverse() * reference_global_init_position; 

            // convert FbxAMatrix(transform) to XMDLOAT4X4(bone.transform) 
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    bone.transform.m_mat[i][j] = transform.Get(i, j);
                }   
            }
        }
    }  
   
}

void MyFbxManager::fetchAnimations(FbxMesh* fbx_mesh, SkeletalAnimation& skeletal_animation, u_int sampling_rate)
{

    // Get the list of all the animation stack.   
    FbxArray<FbxString *> array_of_animation_stack_names;   
    fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names); 
    // Get the number of animations.   
    int number_of_animations = array_of_animation_stack_names.Size(); 

    if (number_of_animations > 0) 
    {    
        // Get the FbxTime per animation's frame.    
        FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();    
        FbxTime frame_time;    frame_time.SetTime(0, 0, 0, 1, 0, time_mode); 

        sampling_rate = sampling_rate > 0 ? sampling_rate : frame_time.GetFrameRate(time_mode);    
        float sampling_time = 1.0f / sampling_rate;    
        skeletal_animation.sampling_time = sampling_time;    
        skeletal_animation.animation_tick = 0.0f;

        FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(0);    
        FbxAnimStack* current_animation_stack = fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());    
        fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);

        FbxTakeInfo* take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());    
        FbxTime start_time = take_info->mLocalTimeSpan.GetStart();    
        FbxTime end_time = take_info->mLocalTimeSpan.GetStop();

        FbxTime sampling_step;    
        sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);    
        sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_time);  

        for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step) 
        { 
           Skeleton skeleton;     
            fetchBoneMatrices(fbx_mesh, skeleton, current_time);
            skeletal_animation.push_back(skeleton);
        }
    }   
    for (int i = 0; i < number_of_animations; i++) 
    { 
        delete array_of_animation_stack_names[i]; 
    }
}

void MyFbxManager::loadAnimationData(const wchar_t* filename, SkeletalAnimation& skeletal_animation, u_int sampling_rate)
{
    //create the FBX manager
    FbxManager* m_manager = FbxManager::Create();

    // Create the IO settings object.
    FbxIOSettings* m_io_settings = FbxIOSettings::Create(m_manager, IOSROOT);
    m_manager->SetIOSettings(m_io_settings);

    // Create an importer using the SDK manager.
    FbxImporter* m_importer = FbxImporter::Create(m_manager, "");

    //convert the filename to a character string for FBX SDK
    const wchar_t* input = filename;

    // Count required buffer size (plus one for null-terminator).
    size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
    char* filename_buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
    // wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
    size_t convertedSize;
    std::wcstombs_s(&convertedSize, filename_buffer, size, input, size);
#else
    std::wcstombs(filename_buffer, input, size);
#endif

    // Use the first argument as the filename for the importer.
    if (!m_importer->Initialize(filename_buffer, -1, m_manager->GetIOSettings()))
    {
        std::string err = "Unable to load animation file";
       throw std::exception(err.c_str());
    }
    delete filename_buffer;

    // Create a new scene so that it can be populated by the imported file.
    FbxScene* m_scene = FbxScene::Create(m_manager, "myScene");

    // Import the contents of the file into the scene.
    m_importer->Import(m_scene);

    FbxAxisSystem axis_converter;
    axis_converter.ConvertScene(m_scene);

    // The file is imported, so get rid of the importer.
    m_importer->Destroy();

    // Convert mesh, NURBS and patch into triangle mesh
    fbxsdk::FbxGeometryConverter geometry_converter(m_manager);
    geometry_converter.Triangulate(m_scene, true);

    // Fetch node attributes and materials under this node recursively. Currently only mesh.
    std::vector<FbxNode*> fetched_meshes;

    std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
        if (node) {



            FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
            if (fbx_node_attribute) {
                switch (fbx_node_attribute->GetAttributeType()) {
                case FbxNodeAttribute::eMesh:
                    fetched_meshes.push_back(node);
                    break;
                }
            }
            for (int i = 0; i < node->GetChildCount(); i++)
                traverse(node->GetChild(i));
        }
    };
    traverse(m_scene->GetRootNode());


    for (size_t i = 0; i < fetched_meshes.size(); i++)
    {
        FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();

        fbxsdk::FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
        fbxsdk::FbxTime frame_time;
        frame_time.SetTime(0, 0, 0, 1, 0, time_mode);

        fetchAnimations(fbx_mesh, skeletal_animation, sampling_rate);
    }
}
