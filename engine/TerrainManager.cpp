#include "TerrainManager.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "CameraManager.h"
#include "ConstantBufferSystem.h"
#include "Blend.h"
#include "Texture.h"



TerrainManager::TerrainManager(Vector2D visible_chunk_count, Vector2D map_size, Vector2D center_chunk)
{
    //set mode to use texts - this is important for choosing whether or not to load terrain dynamically, and whether to change LOD based 
    //on the camera's position
    m_mode = TerrainManagerMode::USE_TXT;

    for (int i = 0; i < THREADCOUNT; i++) m_thread_is_busy[i] = false;

    initChunkIndexes();
    initSeamIndexes();
    initTexturePointers();
    initRasterizersAndStencil();

    //load all map chunks
    m_mapsize = map_size;
    m_visible_chunks = visible_chunk_count;
    m_center = Vector2D((int)(m_visible_chunks.m_x + 2) / 2, (int)(m_visible_chunks.m_y + 2) / 2);

    Vector2D offset = center_chunk - m_center;

    Vector3D cam_pos = CameraManager::get()->getCamera().getTranslation();
    m_player_chunk = Vector2D((int)(cam_pos.m_x) / (33 * PRELOADED_SCALE), (int)(cam_pos.m_z) / (33 * PRELOADED_SCALE));

    //we load an extra 2 rows and columns as a buffer to load new chunks as the player moves. 
    for (int i = 0; i < m_visible_chunks.m_y + 2; i++)
    {
        std::vector<TerrainChunk> row;
        for (int j = 0; j < m_visible_chunks.m_x + 2; j++)
        {
            if ((Vector2D(i, j) + offset).m_x < 0 || (Vector2D(i, j) + offset).m_x > m_mapsize.m_x ||
                (Vector2D(i, j) + offset).m_y < 0 || (Vector2D(i, j) + offset).m_y > m_mapsize.m_y)
            {
                //if the chunk is out of bounds, initialize it as an empty chunk that does NOT need to be loaded.
                row.push_back(TerrainChunk(true));
                continue;
            }

            row.push_back(TerrainChunk(false));
            m_num_unloaded_chunks++;
        }
        m_map.push_back(row);
    }


}

TerrainManager::TerrainManager(const char* filename_heightmap, const char* filename_terraintype, Vector2D chunk_count, Vector2D offset, Vector2D chunkstart)
{
    //set mode to use texts
    m_mode = TerrainManagerMode::LOAD_BITMAP;
    //save the output offset value for any generated text files
    m_file_output_offset = chunkstart;

    //load all map chunks
	m_mapsize = chunk_count;
    m_visible_chunks = chunk_count;
    m_center = Vector2D((int)(m_visible_chunks.m_x + 2) / 2, (int)(m_visible_chunks.m_y + 2) / 2);

    Vector3D cam_pos = CameraManager::get()->getCamera().getTranslation();
    m_player_chunk = Vector2D((int)(cam_pos.m_x) / 33, (int)(cam_pos.m_z) / 33);

	for (int i = 0; i < m_mapsize.m_x; i++)
	{
		std::vector<TerrainChunk> row;
		for (int j = 0; j < m_mapsize.m_y; j++)
		{
			TerrainPtr t(new Terrain(filename_heightmap, filename_terraintype, Vector2D(i, j) + offset));
            TerrainChunk c(true); //signify that c is already loaded and does NOT need to be threaded.
            c.m_chunk = t;

			row.push_back(c);
		}
		m_map.push_back(row);
	}

    initChunkIndexes();
    initSeamIndexes();
    initTexturePointers();
    initRasterizersAndStencil();

    for(int i = 0; i < THREADCOUNT; i++) m_thread_is_busy[i] = false;
}

TerrainManager::~TerrainManager()
{
	m_map.clear();

    for (int i = 0; i < THREADCOUNT; i++)
    {
        if (m_chunk_thread[i] != nullptr)
        {
            m_chunk_thread[i]->detach();
            m_chunk_thread[i].reset();
        }
    }
}

void TerrainManager::update()
{
    //update the current player chunk and if it has changed, begin a thread to load new data / unload old data
    Vector3D cam_pos = CameraManager::get()->getCamera().getTranslation();
    Vector2D new_pchunk = Vector2D((int)(cam_pos.m_x) / (33 * PRELOADED_SCALE), (int)(cam_pos.m_z) / (33 * PRELOADED_SCALE));

    if (new_pchunk != m_player_chunk)
    {
        int xdif = (int)(new_pchunk.m_x) - (int)(m_player_chunk.m_x);
        int ydif = (int)(new_pchunk.m_y) - (int)(m_player_chunk.m_y);

        if (xdif)
        {
            if (xdif == -1) //the player moved left one chunk
            {
                //remove the last row in m_map
                m_map.pop_back();

                //insert a row at the beginning of m_map
                std::vector<TerrainChunk> row;
                for (int j = 0; j < m_visible_chunks.m_x + 2; j++)
                {
                    row.push_back(TerrainChunk(false));
                    //increment the number of chunks waiting loading
                    m_num_unloaded_chunks++;
                }
                m_map.emplace(m_map.begin(), row);
            }
            else //the player moved right one chunk
            {
                //remove the bottom row of m_map
                m_map.erase(m_map.begin(), m_map.begin()+1);
                //add a row to the back of m_map
                std::vector<TerrainChunk> row;
                for (int j = 0; j < m_visible_chunks.m_x + 2; j++)
                {
                    row.push_back(TerrainChunk(false));
                    //increment the number of chunks waiting loading
                    m_num_unloaded_chunks++;
                }
                m_map.push_back(row);
            }
        }

        if (ydif)
        {
            if (ydif == -1) //the player moved back one chunk
            {

                for (int i = 0; i < m_visible_chunks.m_y + 2; i++)
                {
                    //remove the first chunk from each row
                    m_map[i].pop_back();
                    //add an empty chunk to the back of each row to be loaded in a seperate thread
                    m_map[i].emplace(m_map[i].begin(), TerrainChunk(false));
                    //increment the number of chunks waiting loading
                    m_num_unloaded_chunks++;
                }
            }
            else //the player moved forward one chunk
            {

                for (int i = 0; i < m_visible_chunks.m_y + 2; i++)
                {
                    //remove the leftmost chunk from each row
                    m_map[i].erase(m_map[i].begin(), m_map[i].begin()+1);
                    //add an empty chunk to the back of each row to be loaded in a seperate thread
                    m_map[i].push_back(TerrainChunk(false));
                    //increment the number of chunks waiting loading
                    m_num_unloaded_chunks++;
                }
            }
        }
        m_player_chunk = new_pchunk;
    }

    checkThreads();
}

void TerrainManager::render(int shader, float bumpiness, bool is_wireframe, int is_HD)
{
    //declare temporary smartptr vectors to store the render order for this frame.
    std::vector<TerrainPtr> high, mid, low;
    SeamRenderVectors forward, right;

    if (m_mode == TerrainManagerMode::USE_TXT) update();
    else
    {
        Vector3D cam_pos = CameraManager::get()->getCamera().getTranslation();
        m_player_chunk = Vector2D((int)(cam_pos.m_x) / 33, (int)(cam_pos.m_z) / 33);
    }

    cullChunksByFrustum();

    prepareLODArrays(&high, &mid, &low, &forward, &right);

//====================================================================
//          Set Necessary Resources
//--------------------------------------------------------------------

    //set blend mode to alpha
    BlendMode::get()->SetBlend(BlendType::ALPHA);

    //set the material characteristics
    Material_Obj mat;
    mat.m_diffuseColor = Vector3D(0.55f, 0.75f, 0.15f);
    mat.m_d = 1.0f;
    mat.m_metallicAmount = 0.445f;
    mat.m_shininess = 30;
    mat.m_specularColor = Vector3D(0.2f, 0.4f, 0.4f);
    mat.m_rimColor = Vector3D(0.7f, 0.4f, 0.4f);
    mat.m_rimColor.m_w = 1.0f;
    mat.m_rimPower = 2.0;
    GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetObjectLightPropertyBuffer(mat);

    //set the height buffer for domain shader enabled terrain
    GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetDSHeightBuffer(bumpiness);

    //Set the rasterizer state
    if (is_wireframe)
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs_wire.Get());
    else
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setRasterState(m_rs_fill.Get());

    //set all textures for the terrain
    GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture3SplatTex(
        m_tex1, m_displace1_1, m_norm1, m_rough1, m_ambient_occ1,
        m_tex2, m_displace2_1, m_norm2, m_rough2, m_ambient_occ2, 
        m_tex3, m_displace3_1, m_norm3, m_rough3, m_ambient_occ3,
        m_tex4, m_displace4_1, m_norm4, m_rough4, m_ambient_occ4);

    ////set the terrain color constant buffer --! need to build a system to generate these terrain color values automatically!
    //GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTerrainColorBuffer(
    //    Vector4D(0.25, 0.45, 0.18, 1.0), Vector4D(0.6, 0.6, 0.6, 1.0), Vector4D(0.5, 0.3, 0.25, 1.0));

    Matrix4x4 temp;
    temp.setIdentity();
    Matrix4x4	bone_transforms[MAXBONES];
    GraphicsEngine::get()->getConstantBufferSystem()->updateAndSetTransformationBuffer(temp, bone_transforms);


//--------------------------------------------------------------------
//          Chunk Rendering  
//--------------------------------------------------------------------
    //===========================================================
    // HD Chunks
    //===========================================================
    GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::TERRAIN_HD_TOON);
    if(shader == Shaders::TESSDEMO)    GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::TESSDEMO);
    GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_high);
    for (int i = 0; i < high.size(); i++) high[i]->render(0, 0);

        //===========================================================
        // Seams
        //===========================================================
            //===========================================================
            // High
            //===========================================================
    if (forward.high.size() + right.high.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_seam_high);
        for (int i = 0; i < forward.high.size(); i++) forward.high[i]->render(0, 1, SeamLOD::HIGH);
        for (int i = 0; i < right.high.size(); i++) right.high[i]->render(0, 2, SeamLOD::HIGH);
    }
    
    //===========================================================
    // MD Chunks
    //===========================================================
    GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::TERRAIN_MD_TOON);
    GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_mid);
    for (int i = 0; i < mid.size(); i++) mid[i]->render(1, 0);

        //===========================================================
        // Seams
        //===========================================================
            //===========================================================
            // High To Mid
            //===========================================================
    if (forward.highmid.size() + right.highmid.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_highToMid);
        for (int i = 0; i < forward.highmid.size(); i++) forward.highmid[i]->render(0, 1, SeamLOD::HIGH_MID);
        for (int i = 0; i < right.highmid.size(); i++) right.highmid[i]->render(0, 2, SeamLOD::HIGH_MID);
    }

            //===========================================================
            // Mid To High
            //===========================================================
    if (forward.midhigh.size() + right.midhigh.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_midToHigh);
        for (int i = 0; i < forward.midhigh.size(); i++) forward.midhigh[i]->render(0, 1, SeamLOD::HIGH_MID);
        for (int i = 0; i < right.midhigh.size(); i++) right.midhigh[i]->render(0, 2, SeamLOD::HIGH_MID);
    }

            //===========================================================
            // Mid
            //===========================================================
    if (forward.mid.size() + right.mid.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_seam_mid);
        for (int i = 0; i < forward.mid.size(); i++) forward.mid[i]->render(0, 1, SeamLOD::MID);
        for (int i = 0; i < right.mid.size(); i++) right.mid[i]->render(0, 2, SeamLOD::MID);
    }

    //===========================================================
    // LD Chunks
    //===========================================================
    GraphicsEngine::get()->getShaderManager()->setPipeline(Shaders::TERRAIN_LD_TOON);
    GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_low);
    for (int i = 0; i < low.size(); i++) low[i]->render(2, 0);


        //===========================================================
        // Seams
        //===========================================================
            //===========================================================
            // Mid To Low
            //===========================================================
    if (forward.midlow.size() + right.midlow.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_midToLow);
        for (int i = 0; i < forward.midlow.size(); i++) forward.midlow[i]->render(0, 1, SeamLOD::MID_LOW);
        for (int i = 0; i < right.midlow.size(); i++) right.midlow[i]->render(0, 2, SeamLOD::MID_LOW);
    }

            //===========================================================
            // Low To Mid
            //===========================================================
    if (forward.lowmid.size() + right.lowmid.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_lowToMid);
        for (int i = 0; i < forward.lowmid.size(); i++) forward.lowmid[i]->render(0, 1, SeamLOD::MID_LOW);
        for (int i = 0; i < right.lowmid.size(); i++) right.lowmid[i]->render(0, 2, SeamLOD::MID_LOW);
    }

            //===========================================================
            // Low
            //===========================================================
    if (forward.low.size() + right.low.size() > 0)
    {
        GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_LOD_seam_low);
        for (int i = 0; i < forward.low.size(); i++) forward.low[i]->render(0, 1, SeamLOD::LOW);
        for (int i = 0; i < right.low.size(); i++) right.low[i]->render(0, 2, SeamLOD::LOW);
    }
}

void TerrainManager::initChunkIndexes()
{

    //initialize the index buffers

    int cols = CHUNK_AND_SEAM_SIZE;
    int rows = CHUNK_AND_SEAM_SIZE;

    //Create the grid
    //m_num_vertexes = rows * cols;
    int num_faces = (rows - 1) * (cols - 1) * 2;

    std::vector<DWORD> indices(num_faces * 3);

    int k = 0;
    for (DWORD i = 0; i < rows - 1; i++)
    {
        for (DWORD j = 0; j < cols - 1; j++)
        {
            indices[k] = i * cols + j;        // Bottom left of quad

            indices[k + 1] = i * cols + j + 1;        // Bottom right of quad

            indices[k + 2] = (i + 1) * cols + j;    // Top left of quad


            indices[k + 3] = (i + 1) * cols + j;    // Top left of quad

            indices[k + 4] = i * cols + j + 1;        // Bottom right of quad

            indices[k + 5] = (i + 1) * cols + j + 1;    // Top right of quad

            k += 6; // next quad

        }
    }

    //std::vector<DWORD> indices2((m_num_faces * 3)/3);
    //std::vector<DWORD> indices2(m_num_faces / 4 * 3);
    std::vector<DWORD> indices2((num_faces * 3 / 4));

    k = 0;
    //texUIndex = 0;
    //texVIndex = 0;

    for (DWORD i = 0; i < rows - 1; i += 2)
    {
        for (DWORD j = 0; j < cols - 1; j += 2)
        {
            indices2[k] = i * cols + j;        // Bottom left of quad

            indices2[k + 1] = i * cols + j + 2;        // Bottom right of quad

            indices2[k + 2] = (i + 2) * cols + j;    // Top left of quad

            indices2[k + 3] = (i + 2) * cols + j;    // Top left of quad

            indices2[k + 4] = i * cols + j + 2;        // Bottom right of quad

            indices2[k + 5] = (i + 2) * cols + j + 2;    // Top right of quad

            k += 6; // next quad
        }

    }

    std::vector<DWORD> indices3((num_faces * 3 / 16));

    k = 0;

    for (DWORD i = 0; i < rows - 1; i += 4)
    {
        for (DWORD j = 0; j < cols - 1; j += 4)
        {
            indices3[k] = i * cols + j;        // Bottom left of quad

            indices3[k + 1] = i * cols + j + 4;        // Bottom right of quad

            indices3[k + 2] = (i + 4) * cols + j;    // Top left of quad

            indices3[k + 3] = (i + 4) * cols + j;    // Top left of quad

            indices3[k + 4] = i * cols + j + 4;        // Bottom right of quad

            indices3[k + 5] = (i + 4) * cols + j + 4;    // Top right of quad

            k += 6; // next quad

        }
    }


    m_LOD_high = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());
    m_LOD_mid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices2[0], (UINT)indices2.size());
    m_LOD_low = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices3[0], (UINT)indices3.size());
}

void TerrainManager::initSeamIndexes()
{

    int total_verts = 33 * 2 + 2; //this is the total amount of vertice data available in each strip

    int verts_high = SEAMLESS_CHUNK;
    int verts_mid = (SEAMLESS_CHUNK) / 2;
    int verts_low = (SEAMLESS_CHUNK) / 4;

    //Create the grid
    int num_faces_MH = verts_high + verts_mid + 2; //2 additional vertexes for the cap at both ends of the seam
    int num_faces_LM = verts_mid + verts_low + 2;

    std::vector<DWORD> indices;

//===============================================================================
//  HIGH INDEX
//===============================================================================

    indices.resize(66 * 3); //64 faces in a high res seam plus two faces for caps

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;

    int k = 3;

    for (int i = 1; i <= verts_high; i++)
    {
        indices[k]     = i + 1;
        indices[k + 1] = i + 34;
        indices[k + 2] = i;

        k += 3;
    }

    for (int i = 1; i <= verts_high; i++)
    {
        indices[k] = i + 33 + 1;
        indices[k + 1] = i + 33;
        indices[k + 2] = i;

        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_seam_high = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

//===============================================================================
//===============================================================================

//===============================================================================
//  MID INDEX
//===============================================================================

    indices.clear();
    indices.resize(34 * 3); //32 faces in a high res seam plus two faces for caps

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;

    k = 3;

    for (int i = 1; i <= verts_mid; i++)
    {
        indices[k] = (i + 1) * 2 - 1;
        indices[k + 1] = i * 2 - 1 + 33;
        indices[k + 2] = i * 2 - 1;

        k += 3;
    }

    for (int i = 1; i <= verts_mid; i++)
    {
        indices[k] = i * 2 - 1 + 35;
        indices[k + 1] = i * 2 - 1 + 33;
        indices[k + 2] = (i + 1) * 2 - 1;

        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_seam_mid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

//===============================================================================
//===============================================================================

//===============================================================================
//  LOW INDEX
//===============================================================================
    
    indices.clear();
    indices.resize(18 * 3); //16 faces in a high res seam plus two faces for caps

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;

    k = 3;

    for (int i = 1; i <= verts_low; i++)
    {
        indices[k] = i * 4 + 1;
        indices[k + 1] = (i - 1) * 4 + 34;
        indices[k + 2] = (i - 1) * 4 + 1;


        k += 3;
    }

    for (int i = 1; i <= verts_low; i++)
    {
        indices[k] = i * 4 + 34;
        indices[k + 1] = i * 4 + 1;
        indices[k + 2] = (i - 1) * 4 + 34;


        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_seam_low = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

//===============================================================================
//===============================================================================



//===============================================================================
//  HIGH TO MID INDEX
//===============================================================================

    indices.clear();
    indices.resize(num_faces_MH * 3);

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;


    k = 3;
    //create the high resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
    for (int i = 1; i <= verts_high; i++)
    {
        indices[k] = i + 1;
        indices[k + 1] = (i / 2) * 2 + 34;
        indices[k + 2] = i;


        k += 3;
    }
    //create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
    for (int i = 1; i <= verts_mid; i++)
    {
        indices[k] = i * 2 + 34;
        indices[k + 1] = (i - 1) * 2 + 34;
        indices[k + 2] = i * 2;


        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_highToMid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

//===============================================================================
//===============================================================================

//===============================================================================
//  MID TO HIGH INDEX
//===============================================================================
    /* This is essentially the same as HIGH to MID index, we just reverse the order */
    indices.clear();
    indices.resize(num_faces_MH * 3);

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;


    k = 3;
    //create the high resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
    for (int i = 1; i <= verts_high; i++)
    {
        indices[k] = i + 1 + 33;
        indices[k + 1] = (i / 2) * 2 + 1;
        indices[k + 2] = i + 33;


        k += 3;
    }
    //create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
    for (int i = 1; i <= verts_mid; i++)
    {
        indices[k] = i * 2 + 1;
        indices[k + 1] = (i - 1) * 2 + 1;
        indices[k + 2] = i * 2 + 33;


        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_midToHigh = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());


//===============================================================================
//===============================================================================

//===============================================================================
//  MID TO LOW INDEX
//===============================================================================


    indices.clear();
    indices.resize(num_faces_LM * 3);
    //std::vector<DWORD> indices2((num_faces_LM * 3));

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;
    

    k = 3;
    //create the mid resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
    for (int i = 1; i <= verts_mid; i++)
    {
        indices[k] = (i + 1) * 2 - 1;
        indices[k + 1] = (i / 2) * 4 + 34;
        indices[k + 2] = i * 2 - 1;


        k += 3;
    }
    //create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
    for (int i = 1; i <= verts_low; i++)
    {
        indices[k] = i * 4 + 34;
        indices[k + 1] = i * 4 + 34 - 4;
        indices[k + 2] = i * 4 - 1;


        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_midToLow = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());

//===============================================================================
//===============================================================================



//===============================================================================
//  LOW TO MID INDEX
//===============================================================================

    indices.clear();
    indices.resize(num_faces_LM * 3);
    //std::vector<DWORD> indices2((num_faces_LM * 3));

    //create the index for the top cap
    indices[0] = 1;
    indices[1] = 34;
    indices[2] = 0;


    k = 3;
    //create the mid resolution half of the index, with a number of triangles equal to the edge vertexes of a mid resolution chunk
    for (int i = 1; i <= verts_mid; i++)
    {
        indices[k] = (i + 1) * 2 + 32;
        indices[k + 1] = (i / 2) * 4 + 1;
        indices[k + 2] = i * 2 + 32;


        k += 3;
    }
    //create the second half of the index, with a number of triangles equal to the edge vertexes of a low resolution chunk
    for (int i = 1; i <= verts_low; i++)
    {
        indices[k] = i * 4 + 1;
        indices[k + 1] = i * 4 + 1 - 4;
        indices[k + 2] = i * 4 + 32;


        k += 3;
    }

    //create the index for the bottom cap
    indices[k] = 67;
    indices[k + 2] = 33;
    indices[k + 1] = 66;

    m_LOD_lowToMid = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&indices[0], (UINT)indices.size());
//===============================================================================
//===============================================================================

}

void TerrainManager::initRasterizersAndStencil()
{
    HRESULT hr;
    
    D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_NONE;
    rsDesc.FrontCounterClockwise = false;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.AntialiasedLineEnable = TRUE;
    hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rsDesc, m_rs_fill.GetAddressOf());
    if (FAILED(hr))	assert(0 && "Error loading the fill terrain rasterizer");
    
    
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.FillMode = D3D11_FILL_WIREFRAME;
    rsDesc.CullMode = D3D11_CULL_NONE;
    rsDesc.FrontCounterClockwise = false;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.AntialiasedLineEnable = TRUE;
    hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateRasterizerState(&rsDesc, m_rs_wire.GetAddressOf());
    if (FAILED(hr))	assert(0 && "Error loading the wire terrain rasterizer"); 
    
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = FALSE;
    hr = GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateDepthStencilState(&dsDesc, m_stencil.GetAddressOf());
    
}

void TerrainManager::initTexturePointers()
{
    m_tex1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Basecolor_A.png");
    m_displace1_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Height.png");
    m_norm1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Normal.png");
    m_rough1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_Metallic.png");
    m_ambient_occ1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Grass\\Dense2\\Grass_Dense_Tint_02_Base_AO.png");


    m_tex2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Basecolor.png");
    m_displace2_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Height.png");
    m_norm2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Normal.png");
    m_rough2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_Metallic.png");
    m_ambient_occ2 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Dirt\\Dirt_Path_AO.png");

    m_tex3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Basecolor.png");
    m_displace3_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Height.png");
    m_norm3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Normal.png");
    m_rough3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_Metallic.png");
    m_ambient_occ3 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Rocks\\Rock_Volcanic_B_AO.png");

    m_tex4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Basecolor_A.png");
    m_displace4_1 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Height.png");
    m_norm4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Normal.png");
    m_rough4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_Metallic.png");
    m_ambient_occ4 = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\AssetPack\\Cliff\\Rocks_Cliff_B_AO.png");
}

void TerrainManager::cullChunksByFrustum()
{

    int offset = 1 * (m_mode == TerrainManagerMode::USE_TXT);

    int max_height = 255.0f * PRELOADED_SCALE;

    int drawcount = 0;

    //determine whether each renderable chunk is inside the frustum
    for (int i = 0; i < m_visible_chunks.m_x; i++)
    {
        for (int j = 0; j < m_visible_chunks.m_y; j++)
        {
            if (m_map[i + offset][j + offset].m_chunk == nullptr)  continue;

            //Vector3D corners[8];
            //m_map[i + offset][j + offset].m_chunk->getCorners(&corners[0]);

            bool cull = true;

            //for(int i = 0; i < 4; i++)
            //{
                //if (CameraManager::get()->isInFrustum(Vector3D(corners[i].m_x, 0, corners[i].m_y))) cull = false;
                //if (CameraManager::get()->isInFrustum(Vector3D(corners[i].m_x, max_height, corners[i].m_y))) cull = false;
            //}
            Vector2D temp = m_map[i + offset][j + offset].m_chunk->getCenter();
            if (CameraManager::get()->isInFrustum_Cube(Vector3D(temp.m_x, max_height / 2, temp.m_y), max_height / 2))
            {
                cull = false;
                drawcount++;
            }

            //m_map[i + offset][j + offset].m_is_culled = !CameraManager::get()->isInFrustum(m_map[i + offset][j + offset].m_chunk->getCenter());
            m_map[i + offset][j + offset].m_is_culled = cull;
        }
    }
}

void TerrainManager::prepareLODArrays(std::vector<TerrainPtr>* high, std::vector<TerrainPtr>* mid, std::vector<TerrainPtr>* low,
    SeamRenderVectors* forward, SeamRenderVectors* right)
{
    //Vector2D reference_point = m_player_chunk;
    Vector2D reference_point = Vector2D((int)(m_visible_chunks.m_x) / 2, (int)(m_visible_chunks.m_y) / 2);

    int lodmap[TERRAIN_MAP_SIZE_MAX][TERRAIN_MAP_SIZE_MAX] = {};

    int hd, md, ld;

    hd = 0;
    md = 1;
    ld = 2;

    int offset = 1 * (m_mode == TerrainManagerMode::USE_TXT);

    //determine the LOD for each visible chunk before we can decide how to render the seams.
    for (int i = 0; i < m_visible_chunks.m_x; i++)
    {
        for (int j = 0; j < m_visible_chunks.m_y; j++)
        {
            int xdif, zdif;

            xdif = reference_point.m_x - i;
            zdif = reference_point.m_y - j;

            if (xdif < 0) xdif *= -1;
            if (zdif < 0) zdif *= -1;

            if (xdif <= LOD_THRESHHOLD_HIGH && zdif <= LOD_THRESHHOLD_HIGH) lodmap[i][j] = hd;
            else if (xdif <= LOD_THRESHHOLD_MID && zdif <= LOD_THRESHHOLD_MID) lodmap[i][j] = md;
            else lodmap[i][j] = ld;
        }
    }

    for (int i = 0; i < m_visible_chunks.m_x; i++)
    {
        for (int j = 0; j < m_visible_chunks.m_y; j++)
        {
            //if there is no chunk currently in this member of the map, don't try to render it
            if (m_map[i + offset][j + offset].m_chunk == nullptr) continue;

            if (m_map[i + offset][j + offset].m_is_culled == true) continue;

            int lod = lodmap[i][j];

            if (lod == hd) high->push_back(m_map[i+ offset][j+ offset].m_chunk);
            else if (lod == md) mid->push_back(m_map[i+ offset][j+ offset].m_chunk);
            else low->push_back(m_map[i+ offset][j+ offset].m_chunk);

            if (i < m_mapsize.m_x - 1)
            {
                //determine the seam type and put it into the seam render vector
                chooseSeamArray(Vector2D(i+ offset, j+ offset), lod, lodmap[i + 1][j], right);
            }

            if (j < m_mapsize.m_y - 1)
            {
                //determine the seam type and put it into the seam render vector
                chooseSeamArray(Vector2D(i+ offset, j+ offset), lod, lodmap[i][j + 1], forward);
            }
        }
    }

}

void TerrainManager::chooseSeamArray(Vector2D pos, int source, int comp, SeamRenderVectors* srv)
{

    if (source < comp)
    {
        if (source == 0) srv->highmid.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
        else if (source == 1) srv->midlow.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
    }

    if (source > comp)
    {
        if (source == 1) srv->midhigh.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
        else if (source == 2) srv->lowmid.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
    }

    if (source == comp)
    {
        if (source == 0) srv->high.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
        else if (source == 1) srv->mid.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
        else srv->low.push_back(m_map[pos.m_x][pos.m_y].m_chunk);
    }
}

void TerrainManager::checkThreads()
{

    //if there are any chunks that need to be loaded, create / load threads
    if (m_num_unloaded_chunks)
    {

        for (int i = 0; i < THREADCOUNT; i++)
        {
            //if there is a thread, check if it is busy
            if (m_chunk_thread[i] != nullptr)
            {
                //if the thread is finished, join
                if (m_thread_is_busy[i] == false)
                {
                    m_chunk_thread[i]->join();

                    Vector2D target = m_threaded_chunk_pos[i] + m_center - m_player_chunk;

                    //confirm that the chunk is still within a useful range
                    if (target.m_x >= 0 && target.m_x < m_map.size() &&
                        target.m_y >= 0 && target.m_y < m_map[target.m_x].size())
                    {
                        m_map[target.m_x][target.m_y].m_chunk = m_temp_terrain[i];
                        m_map[target.m_x][target.m_y].m_is_loaded = true;
                        m_map[target.m_x][target.m_y].m_is_queued = false;
                        m_num_unloaded_chunks--;
                    }



                    m_temp_terrain[i].reset();
                    m_chunk_thread[i].reset();
                }
            }

            //if there is no thread, see if one should be made
            if (m_chunk_thread[i] == nullptr)
            {
                bool found = false;

                for (int k = 0; k < m_visible_chunks.m_y + 2; k++)
                {
                    for (int j = 0; j < m_visible_chunks.m_x + 2; j++)
                    {
                        if (m_map[k][j].m_is_queued == false && m_map[k][j].m_is_loaded == false)
                        {
                            Vector2D pos = Vector2D(k, j) - m_center + m_player_chunk;
                            m_thread_is_busy[i] = true;
                            std::shared_ptr<std::thread> t(new std::thread(&TerrainManager::threadLoadChunk, this, pos, i));
                            m_chunk_thread[i] = t;

                            m_map[k][j].m_is_queued = true;
                            found = true;
                        }
                        if (found) break;
                    }
                    if (found) break;
                }
            }
        }
    }
}

void TerrainManager::threadLoadChunk(Vector2D location, int thread_num)
{
    //save the chunk location that we are loading so we can decide which element of m_map to put it into once thread is finished
    m_threaded_chunk_pos[thread_num] = location;

    //if the chunk is within map bounds, load it
    if (location.m_x < m_mapsize.m_x &&
        location.m_x > 0 &&
        location.m_y < m_mapsize.m_y &&
        location.m_y > 0)  
        m_temp_terrain[thread_num] = TerrainPtr(new Terrain(location, true));
    //otherwise return a nullptr to signal the edge of the map
    else m_temp_terrain[thread_num].reset();

    //tell the terrain manager that the chunk is finished loading
    m_thread_is_busy[thread_num] = false;
}

void TerrainManager::outputFiles()
{
    for (int i = 0; i < m_map.size(); i++)
    {
        for (int j = 0; j < m_map[i].size(); j++)
        {
            if (m_map[i][j].m_chunk != nullptr)
                m_map[i][j].m_chunk->outputFile(m_file_output_offset);
        }
    }
}

void TerrainManager::updateTerrainTypes()
{
    for (int i = 0; i < m_map.size(); i++)
    {
        for (int j = 0; j < m_map[i].size(); j++)
        {
            if (m_map[i][j].m_chunk != nullptr) 
                m_map[i][j].m_chunk->updateTextureSplat("..\\Assets\\texturesplat.bmp", m_mapsize);
        }
    }
}
