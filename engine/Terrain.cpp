#include "Terrain.h"
#include "GraphicsEngine.h"
#include "MyFbxManager.h"
#include "ConstantBufferSystem.h"
#include "DeviceContext.h"
#include "Blend.h"

#include <exception>

#include <iostream>
#include <fstream>  

Terrain::Terrain(Vector2D map_pos, bool from_generated) : m_pos(map_pos)
{
    int cols = CHUNK_AND_SEAM_SIZE;
    int rows = CHUNK_AND_SEAM_SIZE;

    //Create the grid
    m_num_vertexes = rows * cols;
    m_num_faces = (rows - 1) * (cols - 1) * 2;

    loadChunkFromText(from_generated);
    loadSeamRFromText(from_generated);
    loadSeamFFromText(from_generated);

    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

    //use the vertices to prepare buffers
    m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_vec_vm_chunk[0], sizeof(VertexMesh),
        (UINT)m_vec_vm_chunk.size(), shader_byte_code, (UINT)size_shader);

    m_seam_vb_forward = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_vec_vm_seamF[0], sizeof(VertexMesh),
        (UINT)m_vec_vm_seamF.size(), shader_byte_code, (UINT)size_shader);

    m_seam_vb_right = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_vec_vm_seamR[0], sizeof(VertexMesh),
        (UINT)m_vec_vm_seamR.size(), shader_byte_code, (UINT)size_shader);

}

Terrain::Terrain(const char* filename_heightmap, const char* filename_terraintype, Vector2D map_pos) : m_pos(map_pos)
{
    //get the terrain data 
    fetchMaps(filename_heightmap, filename_terraintype, m_heightmap_info, map_pos);

    int cols = loaded_cols;
    int rows = loaded_rows;

    //Create the grid
    m_num_vertexes = rows * cols;
    m_num_faces = (rows - 1) * (cols - 1) * 2;

    std::vector<VertexMesh> verts(m_num_vertexes);
    std::vector<VertexMesh> verts_forward_seam;
    std::vector<VertexMesh> verts_right_seam;

    for (DWORD i = 0; i < loaded_rows; ++i)
    {
        for (DWORD j = 0; j < loaded_cols; ++j)
        {
            verts[i * loaded_cols + j].m_position = m_heightmap_info.height_map[i * loaded_cols + j]
                + Vector3D(map_pos.m_x * CHUNK_AND_SEAM_SIZE, 0, map_pos.m_y * CHUNK_AND_SEAM_SIZE)
                + Vector3D( -3 * (m_left_buffer), 0, -3 * (m_bottom_buffer));

            verts[i * loaded_cols + j].m_normal = Vector3D(0.0f, 1.0f, 0.0f);
            verts[i * loaded_cols + j].m_bone_influence[0] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_x;
            verts[i * loaded_cols + j].m_bone_influence[1] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_y;
            verts[i * loaded_cols + j].m_bone_influence[2] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_z;
            verts[i * loaded_cols + j].m_bone_influence[3] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_w;
        }
    }

    std::vector<DWORD> indices(m_num_faces * 3);

    int k = 0;
    int texUIndex = 0;
    int texVIndex = 0;
    for (DWORD i = 0; i < loaded_rows - 1; i++)
    {
        for (DWORD j = 0; j < loaded_cols - 1; j++)
        {
            indices[k] = i * loaded_cols + j;        // Bottom left of quad
            verts[i * loaded_cols + j].m_texcoord = Vector2D(texUIndex + 0.0f, texVIndex + 1.0f);

            indices[k + 1] = i * loaded_cols + j + 1;        // Bottom right of quad
            verts[i * loaded_cols + j + 1].m_texcoord = Vector2D(texUIndex + 1.0f, texVIndex + 1.0f);

            indices[k + 2] = (i + 1) * loaded_cols + j;    // Top left of quad
            verts[(i + 1) * loaded_cols + j].m_texcoord = Vector2D(texUIndex + 0.0f, texVIndex + 0.0f);


            indices[k + 3] = (i + 1) * loaded_cols + j;    // Top left of quad
            verts[(i + 1) * loaded_cols + j].m_texcoord = Vector2D(texUIndex + 0.0f, texVIndex + 0.0f);

            indices[k + 4] = i * loaded_cols + j + 1;        // Bottom right of quad
            verts[i * loaded_cols + j + 1].m_texcoord = Vector2D(texUIndex + 1.0f, texVIndex + 1.0f);

            indices[k + 5] = (i + 1) * loaded_cols + j + 1;    // Top right of quad
            verts[(i + 1) * loaded_cols + j + 1].m_texcoord = Vector2D(texUIndex + 1.0f, texVIndex + 0.0f);

            k += 6; // next quad

            texUIndex++;
        }
        texUIndex = 0;
        texVIndex++;
    }


    //////////////////////Compute Normals///////////////////////////
    //Now we will compute the normals for each vertex using normal averaging
    std::vector<Vector3D> tempNormal;

    //normalized and unnormalized normals
    Vector3D unnormalized = Vector3D(0.0f, 0.0f, 0.0f);

    //Used to get vectors (sides) from the position of the verts
    float vecX, vecY, vecZ;

    //Two edges of our triangle
    Vector4D edge1 = Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4D edge2 = Vector4D(0.0f, 0.0f, 0.0f, 0.0f);

    //Compute face normals
    for (int i = 0; i < m_num_faces; ++i)
    {
        //Get the vector describing one edge of our triangle (edge 0,2)
        vecX = verts[indices[(i * 3)]].m_position.m_x - verts[indices[(i * 3) + 1]].m_position.m_x;
        vecY = verts[indices[(i * 3)]].m_position.m_y - verts[indices[(i * 3) + 1]].m_position.m_y;
        vecZ = verts[indices[(i * 3)]].m_position.m_z - verts[indices[(i * 3) + 1]].m_position.m_z;
        edge1 = Vector4D(vecX, vecY, vecZ, 0.0f);    //Create our first edge

        //Get the vector describing another edge of our triangle (edge 2,1)
        vecX = verts[indices[(i * 3) + 2]].m_position.m_x - verts[indices[(i * 3) + 1]].m_position.m_x;
        vecY = verts[indices[(i * 3) + 2]].m_position.m_y - verts[indices[(i * 3) + 1]].m_position.m_y;
        vecZ = verts[indices[(i * 3) + 2]].m_position.m_z - verts[indices[(i * 3) + 1]].m_position.m_z;
        edge2 = Vector4D(vecX, vecY, vecZ, 0.0f);    //Create our second edge

        //Cross multiply the two edge vectors to get the un-normalized face normal
        unnormalized = Vector3D::cross(Vector3D(edge1.m_x, edge1.m_y, edge1.m_z), Vector3D(edge2.m_x, edge2.m_y, edge2.m_z));
        tempNormal.push_back(unnormalized);            //Save unormalized normal (for normal averaging)
    }

    //Compute vertex normals (normal Averaging)
    Vector4D normalSum = Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
    int facesUsing = 0;
    float tX;
    float tY;
    float tZ;

    //Go through each vertex
    for (int i = 0; i < m_num_vertexes; ++i)
    {
        //Check which triangles use this vertex
        for (int j = 0; j < m_num_faces; ++j)
        {
            if (indices[j * 3] == i ||
                indices[(j * 3) + 1] == i ||
                indices[(j * 3) + 2] == i)
            {
                tX = normalSum.m_x + tempNormal[j].m_x;
                tY = normalSum.m_y + tempNormal[j].m_y;
                tZ = normalSum.m_z + tempNormal[j].m_z;

                normalSum = Vector4D(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum
                facesUsing++;
            }
        }

        //Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
        normalSum = normalSum / facesUsing;

        //Normalize the normalSum vector
        Vector3D normalizedSum;
        normalizedSum = Vector3D(normalSum.m_x, normalSum.m_y, normalSum.m_z);
        normalizedSum.normalize();
        normalSum = normalizedSum;

        //Store the normal in our current vertex
        verts[i].m_normal.m_x = -normalSum.m_x;
        verts[i].m_normal.m_y = -normalSum.m_y;
        verts[i].m_normal.m_z = -normalSum.m_z;

        //Clear normalSum and facesUsing for next vertex
        normalSum = Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
        facesUsing = 0;
    }

    //save the dot product of each face with straight down which we will use later for wall collision detection
    for (int i = 0; i < verts.size(); i++)
    {
        float dot = Vector3D::dot(verts[i].m_normal, Vector3D(0, -1, 0));
        verts[i].m_bone_influence[3] = -dot;
    }


    //seperate the loaded vertex data into seperate pieces for the chunk and seams
    int chunk_cols = CHUNK_AND_SEAM_SIZE;
    int chunk_rows = CHUNK_AND_SEAM_SIZE;
    
    //Create the grid
    m_num_vertexes = rows * cols;
    m_num_faces = (rows - 1) * (cols - 1) * 2;
    
    std::vector<VertexMesh> chunkverts(m_num_vertexes);

    for (int i = 0; i < chunk_rows; i++)
    {
        for (int j = 0; j < chunk_cols; j++)
        {
            chunkverts[i * chunk_cols + j] = verts[(i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))];
        }
    }

    VertexMesh c1, c2, c3;
    findCaps(verts, cols, rows, &c1, &c2, &c3);

    verts_forward_seam.resize(chunk_cols * 2 + 2);

    for (int i = 0; i < chunk_cols; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            verts_forward_seam[i + j * chunk_cols + 1] = verts[chunk_rows - 1 + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))];
        }
    }

    verts_forward_seam[0] = c1;
    verts_forward_seam[chunk_cols * 2 + 1] = c2;

    verts_right_seam.resize(chunk_cols * 2 + 2);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < chunk_cols; j++)
        {
            verts_right_seam[i * chunk_rows + j + 1] = verts[(chunk_rows - 1) * cols + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))];
        }
    }

    verts_right_seam[0] = c3;
    verts_right_seam[chunk_cols * 2 + 1] = c2;


    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

    //use the vertices to prepare buffers
    m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&chunkverts[0], sizeof(VertexMesh),
        (UINT)chunkverts.size(), shader_byte_code, (UINT)size_shader);

    if (verts_forward_seam.size()) m_seam_vb_forward = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&verts_forward_seam[0], sizeof(VertexMesh),
        (UINT)verts_forward_seam.size(), shader_byte_code, (UINT)size_shader);

   if(verts_right_seam.size()) m_seam_vb_right = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&verts_right_seam[0], sizeof(VertexMesh),
        (UINT)verts_right_seam.size(), shader_byte_code, (UINT)size_shader);


    m_vec_vm_chunk = chunkverts;
    m_vec_vm_seamR = verts_right_seam;
    m_vec_vm_seamF = verts_forward_seam;
}


Terrain::Terrain(const std::vector<VertexMesh>& verts)
{

    //seperate the loaded vertex data into seperate pieces for the chunk and seams
    int chunk_columns = CHUNK_AND_SEAM_SIZE;
    int chunk_rows = CHUNK_AND_SEAM_SIZE;
    int total_columns = CHUNK_AND_SEAM_SIZE + 1;
    int total_rows = CHUNK_AND_SEAM_SIZE + 1;

    //Create the grid
    m_num_vertexes = CHUNK_AND_SEAM_SIZE * CHUNK_AND_SEAM_SIZE;
    m_num_faces = (CHUNK_AND_SEAM_SIZE - 1) * (CHUNK_AND_SEAM_SIZE - 1) * 2;

    std::vector<VertexMesh> chunkverts(m_num_vertexes);

    for (int i = 0; i < chunk_rows; i++)
    {
        for (int j = 0; j < chunk_columns; j++)
        {
            chunkverts[i * chunk_columns + j] = verts[(i * total_columns) + (j)];
        }
    }

    std::vector<VertexMesh> verts_right_seam;
    verts_right_seam.resize(chunk_columns * 2 + 2);

    /* Due to compatibility restrictions with the old terrain generation system which is unfortunately inefficient, the first
   and last vertex in each seam do not behave like the others and have set positions.  I will change this in the future
   but for now it is a lot of work to fix and I don't have that much time. */

    for (int i = 0; i < chunk_columns; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            verts_right_seam[i + j * chunk_columns + 1] = verts[chunk_columns - 1 + total_columns * i + j];
        }
    }
    /* for compute shader generated terrain, the right seam wants to hide the first and last polygons it has
    which are now unecessary due to computation changes. we will set them to the same value as one of the vertices they are indexed with*/
    verts_right_seam[0] = verts[chunk_columns + total_columns * (chunk_columns - 1)];
    verts_right_seam[chunk_columns * 2 + 1] = verts_right_seam[0];

    std::vector<VertexMesh> verts_forward_seam;
    verts_forward_seam.resize(chunk_columns * 2 + 2);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < chunk_columns; j++)
        {
            //verts_right_seam[i * chunk_rows + j + 1] = verts[(chunk_rows - 1) * CHUNK_AND_SEAM_SIZE + (i + 3 * (m_left_buffer)) * CHUNK_AND_SEAM_SIZE + (j + 3 * (m_bottom_buffer))];
            verts_forward_seam[i * chunk_rows + j + 1] = verts[(total_columns * (total_rows - 2)) + total_columns * i + j];
        }
    }

    verts_forward_seam[0] = verts[total_columns * total_rows - 1];
    verts_forward_seam[chunk_columns * 2 + 1] = verts[chunk_columns + total_rows * (chunk_columns - 1)];
    //verts_right_seam[0] = c3;
    //verts_right_seam[chunk_cols * 2 + 1] = c2;


    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

    //use the vertices to prepare buffers
    m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&chunkverts[0], sizeof(VertexMesh),
        (UINT)chunkverts.size(), shader_byte_code, (UINT)size_shader);

    m_seam_vb_forward = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&verts_forward_seam[0], sizeof(VertexMesh),
        (UINT)verts_forward_seam.size(), shader_byte_code, (UINT)size_shader);

    m_seam_vb_right = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&verts_right_seam[0], sizeof(VertexMesh),
        (UINT)verts_right_seam.size(), shader_byte_code, (UINT)size_shader);


    m_vec_vm_chunk = chunkverts;
    //m_vec_vm_seamR = verts_right_seam;
    //m_vec_vm_seamF = verts_forward_seam;
}


Terrain::~Terrain()
{
    delete[] m_heightmap_info.height_map;
    if (m_heightmap_info.forward_seam_hm != nullptr) delete[] m_heightmap_info.forward_seam_hm;
    if (m_heightmap_info.right_seam_hm != nullptr) delete[] m_heightmap_info.right_seam_hm;
    delete[] m_heightmap_info.terrain_color;
    if (m_heightmap_info.forward_seam_tc != nullptr) delete[] m_heightmap_info.forward_seam_tc;
    if (m_heightmap_info.right_seam_tc != nullptr) delete[] m_heightmap_info.right_seam_tc;

}

//we are going to send the splatmap AS BONEWEIGHT to the vertex shader, which will calculate the balance of textures as a float3 value
//for the domain shader to determine which height to use, and for the pixel shader to pick the appropriate texture.

void Terrain::render(int lod, int vb_id, int seam_lod)
{

    //set the vertices which will be drawn
    if (vb_id == 0) GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
    else if (vb_id == 1 && m_seam_vb_forward != nullptr) GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_seam_vb_forward);
    else if (vb_id == 2 && m_seam_vb_right != nullptr) GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_seam_vb_right);
    else if (vb_id == 1 && m_seam_vb_forward == nullptr ||
        vb_id == 2 && m_seam_vb_right == nullptr)
        {
            return;
        }

    int vcount; //count of vertexes that need to be rendered

    if (vb_id == 0) //for the main chunk
    {
        vcount = (m_num_faces * 3) * (lod == 0) +
                 (m_num_faces * 3 / 4) * (lod == 1) +
                 (m_num_faces * 3 / 16) * (lod == 2);
    }
    else //for the seams
    { 
        //number of faces is equal to have the vertex count per edge of the surrounding chunks + 2 for caps. HD chunk is 32 so 32+32+2 for HIGH
        vcount = (66 * 3) * (seam_lod == SeamLOD::HIGH) +
                 (50 * 3) * (seam_lod == SeamLOD::HIGH_MID) +
                 (34 * 3) * (seam_lod == SeamLOD::MID) +
                 (26 * 3) * (seam_lod == SeamLOD::MID_LOW) +
                 (18 * 3) * (seam_lod == SeamLOD::LOW);
    }

    //render the terrain
    if (GraphicsEngine::get()->getShaderManager()->isTessActive()) GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawControlPointPatchList(vcount, 0, 0);
    else GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(vcount, 0, 0);
}

//Vector3D Terrain::clampToTerrain(const Vector3D& pos)
//{
//    Vector3D out = pos;
//    if (out.m_x < 0) out.m_x = 0;
//    else if (out.m_x > (m_heightmap_info.terrainWidth - 1) * TERRAIN_XZ_SCALE) out.m_x = (m_heightmap_info.terrainWidth - 1) * TERRAIN_XZ_SCALE;
//
//    if (out.m_z < 0) out.m_z = 0;
//    else if (out.m_z > (m_heightmap_info.terrainHeight - 1) * TERRAIN_XZ_SCALE) out.m_z = (m_heightmap_info.terrainHeight - 1) * TERRAIN_XZ_SCALE;
//
//
//    return out;
//}

//float Terrain::getTerrainY(const Vector3D& pos)
//{
//    Vector3D out = pos;
//
//    Vector4D terrain_heights;
//    //get the height of the four vertexes around the player's position
//    terrain_heights.m_x = m_heightmap_info.height_map[((int)(pos.m_x) / TERRAIN_XZ_SCALE) * m_heightmap_info.terrainWidth        + (int)(pos.m_z) / TERRAIN_XZ_SCALE].m_y;
//    terrain_heights.m_y = m_heightmap_info.height_map[((int)(pos.m_x) / TERRAIN_XZ_SCALE + 1) * m_heightmap_info.terrainWidth    + (int)(pos.m_z) / TERRAIN_XZ_SCALE].m_y;
//    terrain_heights.m_z = m_heightmap_info.height_map[((int)(pos.m_x) / TERRAIN_XZ_SCALE) * m_heightmap_info.terrainWidth        + ((int)pos.m_z + 1) / TERRAIN_XZ_SCALE].m_y;
//    terrain_heights.m_w = m_heightmap_info.height_map[((int)(pos.m_x) / TERRAIN_XZ_SCALE + 1) * m_heightmap_info.terrainWidth    + ((int)pos.m_z + 1) / TERRAIN_XZ_SCALE].m_y;
//
//
//    //find the player's position within the face
//    float x_remainder = pos.m_x - (int)pos.m_x;
//    float z_remainder = pos.m_z - (int)pos.m_z;
//
//    //if the position is directly on a vertex, return the vertex's height
//    if (x_remainder == 0 && z_remainder == 0) return terrain_heights.m_x;
//    if (x_remainder == 1 && z_remainder == 0) return terrain_heights.m_y;
//    if (x_remainder == 0 && z_remainder == 1) return terrain_heights.m_z;
//    if (x_remainder == 1 && z_remainder == 1) return terrain_heights.m_w;
//
//    //calculate the height of the position on each axis seperately
//    float x_height = (terrain_heights.m_y * x_remainder) + (terrain_heights.m_x * (1.0f - x_remainder));
//    float z_height = (terrain_heights.m_w * x_remainder) + (terrain_heights.m_z * (1.0f - x_remainder));
//    //add them together proportional to their weight
//    return (x_height * (1.0f - z_remainder))  + (z_height * z_remainder);
//}

Vector2D Terrain::getCenter()
{
    //float center_height = m_vec_vm_chunk[(int)(CHUNK_AND_SEAM_SIZE * CHUNK_AND_SEAM_SIZE) / 2].m_position.m_y;
    return (Vector2D(m_pos.m_x, m_pos.m_y) * (CHUNK_AND_SEAM_SIZE - 1) +
        Vector2D((CHUNK_AND_SEAM_SIZE-1) / 2.0f, (CHUNK_AND_SEAM_SIZE - 1) / 2.0f)) * PRELOADED_SCALE;
}

void Terrain::getCorners(Vector3D* corner_array)
{
    Vector2D corners[4];
    

    //calculate the x/z locations of each corner at 0 level
    corners[0] = m_pos * (CHUNK_AND_SEAM_SIZE - 1);
    corners[1] = m_pos * (CHUNK_AND_SEAM_SIZE - 1) + Vector2D((CHUNK_AND_SEAM_SIZE - 1), 0);
    corners[2] = m_pos * (CHUNK_AND_SEAM_SIZE - 1) + Vector2D(0, (CHUNK_AND_SEAM_SIZE - 1));
    corners[3] = m_pos * (CHUNK_AND_SEAM_SIZE - 1) + Vector2D((CHUNK_AND_SEAM_SIZE - 1), (CHUNK_AND_SEAM_SIZE - 1));

    //find the Y value of each corner's vertex
    float h1 = m_vec_vm_chunk[0].m_position.m_y;
    float h2 = m_vec_vm_chunk[(int)(CHUNK_AND_SEAM_SIZE - 1)].m_position.m_y;
    float h3 = m_vec_vm_chunk[(int)(CHUNK_AND_SEAM_SIZE * CHUNK_AND_SEAM_SIZE) - (1 + (int)(CHUNK_AND_SEAM_SIZE - 1))].m_position.m_y;
    float h4 = m_vec_vm_chunk[(int)(CHUNK_AND_SEAM_SIZE * CHUNK_AND_SEAM_SIZE) - 1].m_position.m_y;

    //output the 8 chunk corners

    corner_array[0] = Vector3D(corners[0].m_x, 0, corners[0].m_y);
    corner_array[1] = Vector3D(corners[1].m_x, 0, corners[1].m_y);
    corner_array[2] = Vector3D(corners[2].m_x, 0, corners[2].m_y);
    corner_array[3] = Vector3D(corners[3].m_x, 0, corners[3].m_y);
    corner_array[4] = Vector3D(corners[0].m_x, h1, corners[0].m_y);
    corner_array[5] = Vector3D(corners[1].m_x, h2, corners[1].m_y);
    corner_array[6] = Vector3D(corners[2].m_x, h3, corners[2].m_y);
    corner_array[7] = Vector3D(corners[3].m_x, h4, corners[3].m_y);
}

void Terrain::outputFile(Vector2D output_offset)
{

    std::string folder = "..\\ChunkData\\Generated\\";

    std::string chunk_x, chunk_z;
    chunk_x = std::to_string((int)(m_pos.m_x + output_offset.m_x));
    chunk_z = std::to_string((int)(m_pos.m_y + output_offset.m_y));

//====================================================================================
    //output the chunk data
//------------------------------------------------------------------------------------
    std::string file = folder + chunk_x + "_" + chunk_z + "_chunk.txt";
    std::ofstream outfile(file.c_str());

    Vector3D pos, normal;
    float tex_splat[4];

    for (int i = 0; i < m_vec_vm_chunk.size(); i++)
    {
        pos = m_vec_vm_chunk[i].m_position / PRELOADED_SCALE;
        normal = m_vec_vm_chunk[i].m_normal;
        tex_splat[0] = m_vec_vm_chunk[i].m_bone_influence[0];
        tex_splat[1] = m_vec_vm_chunk[i].m_bone_influence[1];
        tex_splat[2] = m_vec_vm_chunk[i].m_bone_influence[2];
        tex_splat[3] = m_vec_vm_chunk[i].m_bone_influence[3];

        //output position
        outfile << std::to_string(pos.m_x) << " ";
        outfile << std::to_string(pos.m_y) << " ";
        outfile << std::to_string(pos.m_z) << " ";

        //output normal
        outfile << std::to_string(normal.m_x) << " ";
        outfile << std::to_string(normal.m_y) << " ";
        outfile << std::to_string(normal.m_z) << " ";

        //output texture splat data
        outfile << std::to_string(tex_splat[0]) << " ";
        outfile << std::to_string(tex_splat[1]) << " ";
        outfile << std::to_string(tex_splat[2]) << " ";
        outfile << std::to_string(tex_splat[3]) << " ";
    }

    outfile.close();
//====================================================================================

//====================================================================================
    //output the right seam
//------------------------------------------------------------------------------------
    file = folder + std::to_string((int)(m_pos.m_x)) + "_" + std::to_string((int)(m_pos.m_y)) + "_seamR.txt";
    std::ofstream outfile2(file.c_str());

    if (m_seam_vb_right != nullptr)
    {
        for (int i = 0; i < m_vec_vm_seamR.size(); i++)
        {
            pos = m_vec_vm_seamR[i].m_position / PRELOADED_SCALE;
            normal = m_vec_vm_seamR[i].m_normal;
            tex_splat[0] = m_vec_vm_seamR[i].m_bone_influence[0];
            tex_splat[1] = m_vec_vm_seamR[i].m_bone_influence[1];
            tex_splat[2] = m_vec_vm_seamR[i].m_bone_influence[2];
            tex_splat[3] = m_vec_vm_seamR[i].m_bone_influence[3];

            //output position
            outfile << std::to_string(pos.m_x) << " ";
            outfile << std::to_string(pos.m_y) << " ";
            outfile << std::to_string(pos.m_z) << " ";

            //output normal
            outfile << std::to_string(normal.m_x) << " ";
            outfile << std::to_string(normal.m_y) << " ";
            outfile << std::to_string(normal.m_z) << " ";

            //output texture splat data
            outfile << std::to_string(tex_splat[0]) << " ";
            outfile << std::to_string(tex_splat[1]) << " ";
            outfile << std::to_string(tex_splat[2]) << " ";
            outfile << std::to_string(tex_splat[3]) << " ";
        }
    }
    else
    {
        outfile2 << std::to_string(-1) << std::endl;
    }

    outfile2.close();
//====================================================================================

//====================================================================================
    //output the forward seam
//------------------------------------------------------------------------------------
    file = folder + std::to_string((int)(m_pos.m_x)) + "_" + std::to_string((int)(m_pos.m_y)) + "_seamF.txt";
    std::ofstream outfile3(file.c_str());

    if (m_seam_vb_forward != nullptr)
    {
        for (int i = 0; i < m_vec_vm_seamF.size(); i++)
        {
            pos = m_vec_vm_seamF[i].m_position / PRELOADED_SCALE;
            normal = m_vec_vm_seamF[i].m_normal;
            tex_splat[0] = m_vec_vm_seamF[i].m_bone_influence[0];
            tex_splat[1] = m_vec_vm_seamF[i].m_bone_influence[1];
            tex_splat[2] = m_vec_vm_seamF[i].m_bone_influence[2];
            tex_splat[3] = m_vec_vm_seamF[i].m_bone_influence[3];

            //output position
            outfile << std::to_string(pos.m_x) << " ";
            outfile << std::to_string(pos.m_y) << " ";
            outfile << std::to_string(pos.m_z) << " ";

            //output normal
            outfile << std::to_string(normal.m_x) << " ";
            outfile << std::to_string(normal.m_y) << " ";
            outfile << std::to_string(normal.m_z) << " ";

            //output texture splat data
            outfile << std::to_string(tex_splat[0]) << " ";
            outfile << std::to_string(tex_splat[1]) << " ";
            outfile << std::to_string(tex_splat[2]) << " ";
            outfile << std::to_string(tex_splat[3]) << " ";
        }
    }
    else
    {
        outfile3 << std::to_string(-1) << std::endl;
    }

    outfile3.close();
//====================================================================================


}

void Terrain::updateTextureSplat(const char* file, Vector2D max)
{
    //if (m_pos.m_x < 0 || m_pos.m_y < 0 || m_pos.m_x >= max.m_x || m_pos.m_y >= max.m_y) return;

    fetchTerrainTypes(file, m_heightmap_info, m_pos);

    //for (DWORD i = 0; i < CHUNK_AND_SEAM_SIZE; ++i)
    //{
    //    for (DWORD j = 0; j < CHUNK_AND_SEAM_SIZE; ++j)
    //    {
    //        m_vec_vm_chunk[i * CHUNK_AND_SEAM_SIZE + j].m_bone_influence[0] = m_heightmap_info.terrain_color[i * CHUNK_AND_SEAM_SIZE + j].m_x;
    //        m_vec_vm_chunk[i * CHUNK_AND_SEAM_SIZE + j].m_bone_influence[1] = m_heightmap_info.terrain_color[i * CHUNK_AND_SEAM_SIZE + j].m_y;
    //        m_vec_vm_chunk[i * CHUNK_AND_SEAM_SIZE + j].m_bone_influence[2] = m_heightmap_info.terrain_color[i * CHUNK_AND_SEAM_SIZE + j].m_z;
    //        m_vec_vm_chunk[i * CHUNK_AND_SEAM_SIZE + j].m_bone_influence[3] = m_heightmap_info.terrain_color[i * CHUNK_AND_SEAM_SIZE + j].m_w;
    //    }
    //}

    //if (m_heightmap_info.forward_seam_tc != nullptr)
    //{
    //    m_vec_vm_seamF[0].m_bone_influence[0] = m_heightmap_info.forward_seam_tc[0].m_x;
    //    m_vec_vm_seamF[0].m_bone_influence[1] = m_heightmap_info.forward_seam_tc[0].m_y;
    //    m_vec_vm_seamF[0].m_bone_influence[2] = m_heightmap_info.forward_seam_tc[0].m_z;
    //    m_vec_vm_seamF[0].m_bone_influence[3] = m_heightmap_info.forward_seam_tc[0].m_w;

    //    for (DWORD i = 0; i < 2; ++i)
    //    {
    //        for (DWORD j = 0; j < CHUNK_AND_SEAM_SIZE; ++j)
    //        {
    //            m_vec_vm_seamF[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[0] = m_heightmap_info.forward_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_x;
    //            m_vec_vm_seamF[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[1] = m_heightmap_info.forward_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_y;
    //            m_vec_vm_seamF[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[2] = m_heightmap_info.forward_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_z;
    //            m_vec_vm_seamF[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[3] = m_heightmap_info.forward_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_w;
    //        }
    //    }
    //    m_vec_vm_seamF[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[0] = m_heightmap_info.forward_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_x;
    //    m_vec_vm_seamF[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[1] = m_heightmap_info.forward_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_y;
    //    m_vec_vm_seamF[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[2] = m_heightmap_info.forward_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_z;
    //    m_vec_vm_seamF[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[3] = m_heightmap_info.forward_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_w;
    //}

    //if (m_heightmap_info.right_seam_tc != nullptr)
    //{
    //    m_vec_vm_seamR[0].m_bone_influence[0] = m_heightmap_info.right_seam_tc[0].m_x;
    //    m_vec_vm_seamR[0].m_bone_influence[1] = m_heightmap_info.right_seam_tc[0].m_y;
    //    m_vec_vm_seamR[0].m_bone_influence[2] = m_heightmap_info.right_seam_tc[0].m_z;
    //    m_vec_vm_seamR[0].m_bone_influence[3] = m_heightmap_info.right_seam_tc[0].m_w;

    //    for (DWORD i = 0; i < 2; ++i)
    //    {
    //        for (DWORD j = 0; j < CHUNK_AND_SEAM_SIZE; ++j)
    //        {
    //            m_vec_vm_seamR[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[0] = m_heightmap_info.right_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_x;
    //            m_vec_vm_seamR[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[1] = m_heightmap_info.right_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_y;
    //            m_vec_vm_seamR[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[2] = m_heightmap_info.right_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_z;
    //            m_vec_vm_seamR[i * CHUNK_AND_SEAM_SIZE + j + 1].m_bone_influence[3] = m_heightmap_info.right_seam_tc[i * CHUNK_AND_SEAM_SIZE + j + 1].m_w;
    //        }
    //    }
    //    m_vec_vm_seamR[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[0] = m_heightmap_info.right_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_x;
    //    m_vec_vm_seamR[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[1] = m_heightmap_info.right_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_y;
    //    m_vec_vm_seamR[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[2] = m_heightmap_info.right_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_z;
    //    m_vec_vm_seamR[CHUNK_AND_SEAM_SIZE * 2 + 1].m_bone_influence[3] = m_heightmap_info.right_seam_tc[CHUNK_AND_SEAM_SIZE * 2 + 1].m_w;

    //}

    int cols = loaded_cols;
    int rows = loaded_rows;

    //Create the grid
    int num_vertexes = rows * cols;
    int num_faces = (rows - 1) * (cols - 1) * 2;

    std::vector<VertexMesh> verts(num_vertexes);

    for (DWORD i = 0; i < loaded_rows; ++i)
    {
        for (DWORD j = 0; j < loaded_cols; ++j)
        {
            verts[i * loaded_cols + j].m_bone_influence[0] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_x;
            verts[i * loaded_cols + j].m_bone_influence[1] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_y;
            verts[i * loaded_cols + j].m_bone_influence[2] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_z;
            //verts[i * loaded_cols + j].m_bone_influence[3] = m_heightmap_info.terrain_color[i * loaded_cols + j].m_w;
        }
    }

    //seperate the loaded vertex data into seperate pieces for the chunk and seams
    int chunk_cols = CHUNK_AND_SEAM_SIZE;
    int chunk_rows = CHUNK_AND_SEAM_SIZE;

    //Create the grid
    num_vertexes = rows * cols;
    num_faces = (rows - 1) * (cols - 1) * 2;

    //std::vector<VertexMesh> chunkverts(num_vertexes);

    for (int i = 0; i < chunk_rows; i++)
    {
        for (int j = 0; j < chunk_cols; j++)
        {
            m_vec_vm_chunk[i * chunk_cols + j].m_bone_influence[0] = 
                verts[(i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[0];
            m_vec_vm_chunk[i * chunk_cols + j].m_bone_influence[1] =
                verts[(i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[1];
            m_vec_vm_chunk[i * chunk_cols + j].m_bone_influence[2] =
                verts[(i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[2];
            //m_vec_vm_chunk[i * chunk_cols + j].m_bone_influence[3] =
            //    verts[(i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[3];
        }
    }

    VertexMesh c1, c2, c3;
    findCaps(verts, cols, rows, &c1, &c2, &c3); //we are ONLY going to read the influences from these.


    for (int i = 0; i < chunk_cols; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            m_vec_vm_seamF[i + j * chunk_cols + 1].m_bone_influence[0] = 
                verts[chunk_rows - 1 + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[0];
            m_vec_vm_seamF[i + j * chunk_cols + 1].m_bone_influence[1] =
                verts[chunk_rows - 1 + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[1];
            m_vec_vm_seamF[i + j * chunk_cols + 1].m_bone_influence[2] =
                verts[chunk_rows - 1 + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[2];
            //m_vec_vm_seamF[i + j * chunk_cols + 1].m_bone_influence[3] =
            //    verts[chunk_rows - 1 + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[3];
        }
    }

    m_vec_vm_seamF[0].m_bone_influence[0] = c1.m_bone_influence[0];
    m_vec_vm_seamF[0].m_bone_influence[1] = c1.m_bone_influence[1];
    m_vec_vm_seamF[0].m_bone_influence[2] = c1.m_bone_influence[2];
    m_vec_vm_seamF[0].m_bone_influence[3] = c1.m_bone_influence[3];
    m_vec_vm_seamF[chunk_cols * 2 + 1].m_bone_influence[0] = c2.m_bone_influence[0];
    m_vec_vm_seamF[chunk_cols * 2 + 1].m_bone_influence[1] = c2.m_bone_influence[1];
    m_vec_vm_seamF[chunk_cols * 2 + 1].m_bone_influence[2] = c2.m_bone_influence[2];
    m_vec_vm_seamF[chunk_cols * 2 + 1].m_bone_influence[3] = c2.m_bone_influence[3];


    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < chunk_cols; j++)
        {
            m_vec_vm_seamR[i * chunk_rows + j + 1].m_bone_influence[0]
                = verts[(chunk_rows - 1) * cols + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[0];
            m_vec_vm_seamR[i * chunk_rows + j + 1].m_bone_influence[1]
                = verts[(chunk_rows - 1) * cols + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[1];
            m_vec_vm_seamR[i * chunk_rows + j + 1].m_bone_influence[2]
                = verts[(chunk_rows - 1) * cols + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[2];
            //m_vec_vm_seamR[i * chunk_rows + j + 1].m_bone_influence[3]
            //    = verts[(chunk_rows - 1) * cols + (i + 3 * (m_left_buffer)) * cols + (j + 3 * (m_bottom_buffer))].m_bone_influence[3];
        }
    }

    m_vec_vm_seamR[0].m_bone_influence[0] = c3.m_bone_influence[0];
    m_vec_vm_seamR[0].m_bone_influence[1] = c3.m_bone_influence[1];
    m_vec_vm_seamR[0].m_bone_influence[2] = c3.m_bone_influence[2];
   // m_vec_vm_seamR[0].m_bone_influence[3] = c3.m_bone_influence[3];
    m_vec_vm_seamR[chunk_cols * 2 + 1].m_bone_influence[0] = c2.m_bone_influence[0];
    m_vec_vm_seamR[chunk_cols * 2 + 1].m_bone_influence[1] = c2.m_bone_influence[1];
    m_vec_vm_seamR[chunk_cols * 2 + 1].m_bone_influence[2] = c2.m_bone_influence[2];
   // m_vec_vm_seamR[chunk_cols * 2 + 1].m_bone_influence[3] = c2.m_bone_influence[3];

    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);

    //use the vertices to prepare buffers
    m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_vec_vm_chunk[0], sizeof(VertexMesh),
        (UINT)m_vec_vm_chunk.size(), shader_byte_code, (UINT)size_shader);

    m_seam_vb_forward = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_vec_vm_seamF[0], sizeof(VertexMesh),
        (UINT)m_vec_vm_seamF.size(), shader_byte_code, (UINT)size_shader);

    m_seam_vb_right = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&m_vec_vm_seamR[0], sizeof(VertexMesh),
        (UINT)m_vec_vm_seamR.size(), shader_byte_code, (UINT)size_shader);
}

bool Terrain::fetchMaps(const char* height_filename, const char* tex_filename, HeightMapInfo& hminfo, Vector2D chunk_id)
{
    FILE *filePtr, *filePtrTex;                            // Point to the current position in the file
    BITMAPFILEHEADER bitmapFileHeader, bitmapFileHeaderTex;        // Structure which stores information about file
    BITMAPINFOHEADER bitmapInfoHeader, bitmapInfoHeaderTex;        // Structure which stores information about image
    int image_size, index;
    unsigned char height;

    // Open the files
    filePtr = fopen(height_filename, "rb");
    if (filePtr == NULL)
        return 0;

    filePtrTex = fopen(tex_filename, "rb");
    if (filePtrTex == NULL)
        return 0;

    // Read bitmaps headers
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
    fread(&bitmapFileHeaderTex, sizeof(BITMAPFILEHEADER), 1, filePtrTex);

    // Read the info header
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    fread(&bitmapInfoHeaderTex, sizeof(BITMAPINFOHEADER), 1, filePtrTex);

    // Get the width and height (width and length) of the image
    hminfo.terrainWidth = bitmapInfoHeader.biWidth;
    hminfo.terrainHeight = bitmapInfoHeader.biHeight;

    // if the width is odd, then we need to add padding
    const uint32_t bitCount = 24;
    LONG strideInBytes;
    if (hminfo.terrainWidth & 0x1)
    {
        strideInBytes = ((((hminfo.terrainWidth * bitCount) + 31) & ~31) >> 3);
    }
    else
    {
        strideInBytes = hminfo.terrainWidth * 3;
    }

    // Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
    image_size = strideInBytes * hminfo.terrainHeight;

    // Initialize the arrays which store the image data
    unsigned char* bitmapImage = new unsigned char[image_size];
    unsigned char* bitmapImageTex = new unsigned char[image_size];

    // Set the file pointers to the beginning of the image data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
    fseek(filePtrTex, bitmapFileHeaderTex.bfOffBits, SEEK_SET);

    // Store image data in bitmapImages
    fread(bitmapImage, 1, image_size, filePtr);
    fread(bitmapImageTex, 1, image_size, filePtrTex);

    // Close files
    fclose(filePtr);
    fclose(filePtrTex);

    // Determine how many loadable chunks the image contains on the Z axis (y axis in the actual image)
    // This is important to determine the number of unused bytes in each row
    int num_chunks_z = (hminfo.terrainHeight) / CHUNK_AND_SEAM_SIZE;
    int num_chunks_x = (hminfo.terrainWidth) / CHUNK_AND_SEAM_SIZE;

    //if there is terrain data beyond this chunk, we load the next several rows to calculate normal data for our seams correctly.
    //it will take a little more time, but the end result will be prettier.
    loaded_cols = CHUNK_AND_SEAM_SIZE + 3 * (chunk_id.m_y < num_chunks_z - 1) + 3 * (chunk_id.m_y > 0);
    loaded_rows = CHUNK_AND_SEAM_SIZE + 3 * (chunk_id.m_x < num_chunks_x - 1) + 3 * (chunk_id.m_x > 0);
    if (chunk_id.m_y < num_chunks_z - 1) m_has_forward_seam = true;
    if (chunk_id.m_x < num_chunks_x - 1) m_has_right_seam = true;
    if (chunk_id.m_y > 0) m_bottom_buffer = true;
    if (chunk_id.m_x > 0) m_left_buffer = true;

    // Initialize the heightMap array (stores the vertices of our terrain)
    hminfo.height_map = new Vector3D[loaded_cols * loaded_rows];
    hminfo.terrain_color = new Vector4D[loaded_cols * loaded_rows];



    //the number of unused_bytes per row in the bmp will vary depending on if there is an even or odd number of chunks in the image
    //int unused_bytes = 1 * (num_chunks_z % 2 == 1) + 2 * (num_chunks_z % 2 == 0);
    int unused_bytes = num_chunks_z % 4;



    // We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
    // So we use this counter to skip the next two components in the image data (we read R, then skip BG)
    int k = 0;

    //calculate the data offsets for the chunk we will load
    int x_offset = (((chunk_id.m_x * CHUNK_AND_SEAM_SIZE) * hminfo.terrainHeight) + (chunk_id.m_y * CHUNK_AND_SEAM_SIZE)) * 3;

    //adjust the x offset to account for the unused bytes at the end of each row of pixel data
    x_offset += unused_bytes * CHUNK_AND_SEAM_SIZE * chunk_id.m_x;
    //if we are reading the left and bottom portions of the map as well we need to push the x offset backwards
    if (m_bottom_buffer) x_offset -= 3 * 3;
    if (m_left_buffer) x_offset -= strideInBytes * 3;


    //z offset is the height of the total image, minus the size of just the chunk we are loading.
    //int z_offset = CHUNK_AND_SEAM_SIZE * (num_chunks_z - 1) * 3 + unused_bytes;
    int z_offset = strideInBytes - loaded_cols * 3;
    //int z_offset = strideInBytes - loaded_cols * 3 + 6;
    int full_row_offset = strideInBytes;




    // Read the image data into our heightMap array
    for (int j = 0; j < loaded_rows; j++)
    {
        for (int i = 0; i < loaded_cols; i++)
        {
            //find the surrounding heights and calculate the average
            height = bitmapImage[k + x_offset] + bitmapImage[k + x_offset + 3];
            //float fheight = height / 2.0f;


            index = (loaded_cols * j) + i;

            hminfo.height_map[index].m_x = (float)j;
            hminfo.height_map[index].m_y = sampleHeight(bitmapImage, k, x_offset, full_row_offset, image_size) / EROSION;
            hminfo.height_map[index].m_z = (float)i;


            float r = bitmapImageTex[k + x_offset];
            float g = bitmapImageTex[k + x_offset + 1];
            float b = bitmapImageTex[k + x_offset + 2];

            hminfo.terrain_color[index].m_x = r;
            hminfo.terrain_color[index].m_y = g;
            hminfo.terrain_color[index].m_z = b;

            k += 3;
        }

        k += z_offset;
    }

    delete[] bitmapImage;
    delete[] bitmapImageTex;
    bitmapImage = 0;
    bitmapImageTex = 0;

    return true;
}

bool Terrain::fetchHeightMap(const char* filename, HeightMapInfo& hminfo)
{
    FILE* filePtr;                            // Point to the current position in the file
    BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
    BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
    int imageSize, index;
    unsigned char height;

    // Open the file
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return 0;

    // Read bitmaps header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

    // Read the info header
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

    // Get the width and height (width and length) of the image
    hminfo.terrainWidth = bitmapInfoHeader.biWidth;
    hminfo.terrainHeight = bitmapInfoHeader.biHeight;

    // if the width is odd, then we need to add padding
    const uint32_t bitCount = 24;
    LONG strideInBytes;
    if (hminfo.terrainWidth & 0x1)
    {
        strideInBytes = ((((hminfo.terrainWidth * bitCount) + 31) & ~31) >> 3);
    }
    else
    {
        strideInBytes = hminfo.terrainWidth * 3;
    }

    // Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
    imageSize = strideInBytes * hminfo.terrainHeight;

    // Initialize the array which stores the image data
    unsigned char* bitmapImage = new unsigned char[imageSize];

    // Set the file pointer to the beginning of the image data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // Store image data in bitmapImage
    fread(bitmapImage, 1, imageSize, filePtr);

    // Close file
    fclose(filePtr);


    // Initialize the heightMap array (stores the vertices of our terrain)
    hminfo.height_map = new Vector3D[hminfo.terrainWidth * hminfo.terrainHeight];

    // We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
    // So we use this counter to skip the next two components in the image data (we read R, then skip BG)
    int k = 0;

    // Read the image data into our heightMap array
    for (int j = 0; j < hminfo.terrainHeight; j++)
    {
        for (int i = 0; i < hminfo.terrainWidth; i++)
        {
            height = bitmapImage[k];

            index = (hminfo.terrainHeight * j) + i;

            hminfo.height_map[index].m_x = (float)j;// / widthFactor;
            hminfo.height_map[index].m_y = (float)height / EROSION;// / heightFactor;
            hminfo.height_map[index].m_z = (float)i;// / widthFactor;

            k += 3;
        }
        if (hminfo.terrainWidth & 0x1) k += 1; //adjust for odd-width images
    }

    delete[] bitmapImage;
    bitmapImage = 0;

    return true;
}
bool Terrain::fetchHeightMap(const char* filename, HeightMapInfo& hminfo, Vector2D chunk_id)
{
    FILE* filePtr;                            // Point to the current position in the file
    BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
    BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
    int image_size, index;
    unsigned char height;

    // Open the file
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return 0;

    // Read bitmaps header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

    // Read the info header
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

    // Get the width and height (width and length) of the image
    hminfo.terrainWidth = bitmapInfoHeader.biWidth;
    hminfo.terrainHeight = bitmapInfoHeader.biHeight;

    // if the width is odd, then we need to add padding
    const uint32_t bitCount = 24;
    LONG strideInBytes;
    if (hminfo.terrainWidth & 0x1)
    {
        strideInBytes = ((((hminfo.terrainWidth * bitCount) + 31) & ~31) >> 3);
    }
    else
    {
        strideInBytes = hminfo.terrainWidth * 3;
    }

    // Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
    image_size = strideInBytes * hminfo.terrainHeight;

    // Initialize the array which stores the image data
    unsigned char* bitmapImage = new unsigned char[image_size];

    // Set the file pointer to the beginning of the image data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // Store image data in bitmapImage
    fread(bitmapImage, 1, image_size, filePtr);

    // Close file
    fclose(filePtr);

    // Determine how many loadable chunks the image contains on the Z axis (y axis in the actual image)
    // This is important to determine the number of unused bytes in each row
    int num_chunks_z = (hminfo.terrainHeight) / CHUNK_AND_SEAM_SIZE;
    int num_chunks_x = (hminfo.terrainWidth) / CHUNK_AND_SEAM_SIZE;

    //the number of unused_bytes per row in the bmp will vary depending on if there is an even or odd number of chunks in the image
    //int unused_bytes = 1 * (num_chunks_z % 2 == 1) + 2 * (num_chunks_z % 2 == 0);
    int unused_bytes = num_chunks_z % 4;

    // Initialize the heightMap array (stores the vertices of our terrain)
    hminfo.height_map = new Vector3D[CHUNK_AND_SEAM_SIZE * CHUNK_AND_SEAM_SIZE];

    // We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
    // So we use this counter to skip the next two components in the image data (we read R, then skip BG)
    int k = 0;

    //calculate the data offsets for the chunk we will load
    int x_offset = (( (chunk_id.m_x * CHUNK_AND_SEAM_SIZE) * hminfo.terrainHeight) + (chunk_id.m_y * CHUNK_AND_SEAM_SIZE)) * 3;

    //adjust the x offset to account for the unused bytes at the end of each row of pixel data
    x_offset += unused_bytes * CHUNK_AND_SEAM_SIZE * chunk_id.m_x;
    

    //z offset is the height of the total image, minus the size of just the chunk we are loading.
    int z_offset = CHUNK_AND_SEAM_SIZE * (num_chunks_z - 1) * 3 + unused_bytes;
    int full_row_offset = CHUNK_AND_SEAM_SIZE * num_chunks_z * 3 + unused_bytes;


    //if there is terrain data beyond this chunk, we load the next several rows to calculate normal data for our seams correctly.
    //it will take a little more time, but the end result will be prettier.
    loaded_cols = CHUNK_AND_SEAM_SIZE + 3 * (chunk_id.m_y < num_chunks_z - 1);
    loaded_rows = CHUNK_AND_SEAM_SIZE + 3 * (chunk_id.m_x < num_chunks_x - 1);

    if (loaded_cols > CHUNK_AND_SEAM_SIZE) m_has_forward_seam = true;
    if (loaded_rows > CHUNK_AND_SEAM_SIZE) m_has_right_seam = true;

    // Read the image data into our heightMap array
    for (int j = 0; j < loaded_cols; j++)
    {
        for (int i = 0; i < loaded_rows; i++)
        {
            //find the surrounding heights and calculate the average
            height = bitmapImage[k + x_offset] + bitmapImage[k + x_offset + 3];
            //float fheight = height / 2.0f;
            

            index = (CHUNK_AND_SEAM_SIZE * j) + i;

            hminfo.height_map[index].m_x = (float)j;
            hminfo.height_map[index].m_y = sampleHeight(bitmapImage, k, x_offset, full_row_offset, image_size) / EROSION;
            hminfo.height_map[index].m_z = (float)i;

            k += 3;
        }

        k += z_offset;
    }


    ////=================================================//
    //    //Get seams if necessary
    ////=================================================//
    //int num_chunks_x = (hminfo.terrainWidth) / CHUNK_AND_SEAM_SIZE;
    //
    //if (chunk_id.m_y != num_chunks_z - 1)
    //{
    //    //get the forward seam data

    //    hminfo.forward_seam_hm = new Vector3D[CHUNK_AND_SEAM_SIZE * 2 + 2]; //2 rows of data plus 2 caps
    //    int z_offset_seam = CHUNK_AND_SEAM_SIZE * (num_chunks_z) * 3 + unused_bytes - 2 * 3;

    //    /* This is all pretty iffy.  I may be using variables or naming conventions that just "happen" to work and might not be accurate */
    //    /* Also this all needs to be tested if it works with different sizes of image and so forth */
    //    int k = 0;

    //    for (int j = 0; j < CHUNK_AND_SEAM_SIZE; j++)
    //    {
    //        for (int i = 0; i < 2; i++)
    //        {
    //            height = bitmapImage[k + x_offset + SEAMLESS_CHUNK * 3];// -unused_bytes];

    //            index = (CHUNK_AND_SEAM_SIZE * i) + j + 1;

    //            hminfo.forward_seam_hm[index].m_x = (float)j;
    //            hminfo.forward_seam_hm[index].m_y = sampleHeight(bitmapImage, k, x_offset + SEAMLESS_CHUNK * 3, full_row_offset, image_size) / EROSION;
    //            hminfo.forward_seam_hm[index].m_z = (float)i;

    //            k += 3;
    //        }

    //        k += z_offset_seam;
    //    }

    //    //for the corner of each seam, we are using the first pixel of the NEXT chunk to the current chunk
    //    hminfo.forward_seam_hm[0].m_y = hminfo.forward_seam_hm[CHUNK_AND_SEAM_SIZE + 1].m_y;

    //    float temp = sampleHeight(bitmapImage, k + 3, x_offset + SEAMLESS_CHUNK * 3, full_row_offset, image_size) / EROSION;
    //    hminfo.forward_seam_hm[CHUNK_AND_SEAM_SIZE * 2 + 1] = Vector3D(0, temp, 0);
    //    //hminfo.forward_seam_hm[CHUNK_AND_SEAM_SIZE * 2 + 1] = Vector3D(0, bitmapImage[k + x_offset + SEAMLESS_CHUNK * 3] / EROSION, 0);
    //}

    //if (chunk_id.m_x != num_chunks_x - 1)
    //{
    //    //get the right seam data

    //    hminfo.right_seam_hm = new Vector3D[CHUNK_AND_SEAM_SIZE * 2 + 2]; //3 rows of data plus 2 caps
    //    int x_offset_seam = ((((chunk_id.m_x + 1) * CHUNK_AND_SEAM_SIZE) * hminfo.terrainHeight) + (chunk_id.m_y * CHUNK_AND_SEAM_SIZE)) * 3;
    //    x_offset_seam += unused_bytes * CHUNK_AND_SEAM_SIZE * (chunk_id.m_x + 1);

    //    //for some reason when the unused_byte count equals 3, the x_offset seam needs to be moved back 3 bytes....
    //    x_offset_seam -= 3 * (unused_bytes == 3);

    //    int z_offset_seam = CHUNK_AND_SEAM_SIZE * (num_chunks_z) * 3;

    //    //subtract a row from the x offset to get the start of the seam
    //    x_offset_seam -= z_offset_seam;

    //    /* This is all pretty iffy.  I may be using variables or naming conventions that just "happen" to work and might not be accurate */
    //    /* Also this all needs to be tested if it works with different sizes of image and so forth */
    //    int k = 0;




    //    for (int j = 0; j < 2; j++)
    //    {
    //        for (int i = 0; i < CHUNK_AND_SEAM_SIZE; i++)
    //        {
    //            //height = bitmapImage[k + x_offset_seam];

    //            index = (CHUNK_AND_SEAM_SIZE * j) + i + 1;

    //            hminfo.right_seam_hm[index].m_x = (float)j;
    //            hminfo.right_seam_hm[index].m_y = sampleHeight(bitmapImage, k, x_offset_seam, 
    //                full_row_offset, image_size) / EROSION;
    //            hminfo.right_seam_hm[index].m_z = (float)i;

    //            k += 3;
    //        }

    //        k += z_offset + unused_bytes * 3 - (9 * (unused_bytes != 0)); //for SOME reason the z offset bugs out without this.
    //    }

    //    //for the corner of each seam, we are using the first pixel of the NEXT chunk diagonal to the current chunk
    //    //as reference, which has actually already been read at specifically CHUNK_AND_SEAM_SIZE + 1 position of the index.
    //    hminfo.right_seam_hm[0].m_y = hminfo.right_seam_hm[CHUNK_AND_SEAM_SIZE + 1].m_y;

    //    //float temp = bitmapImage[k + x_offset_seam - (z_offset + unused_bytes * 3 - (9 * (unused_bytes != 0)))] / EROSION;
    //    float temp = sampleHeight(bitmapImage, k - z_offset + unused_bytes * 3 - (9 * (unused_bytes != 0)), x_offset_seam,
    //        full_row_offset, image_size) / EROSION;
    //    hminfo.right_seam_hm[CHUNK_AND_SEAM_SIZE * 2 + 1] = Vector3D(0, temp, 0);

    //}


    delete[] bitmapImage;
    bitmapImage = 0;

    return true;
}

bool Terrain::fetchTerrainTypes(const char* filename, HeightMapInfo& hminfo, Vector2D chunk_id)
{
    FILE* filePtrTex;                            // Point to the current position in the file
    BITMAPFILEHEADER bitmapFileHeaderTex;        // Structure which stores information about file
    BITMAPINFOHEADER bitmapInfoHeaderTex;        // Structure which stores information about image
    int image_size, index;
    unsigned char height;

    filePtrTex = fopen(filename, "rb");
    if (filePtrTex == NULL)
        return 0;

    // Read bitmaps headers
    fread(&bitmapFileHeaderTex, sizeof(BITMAPFILEHEADER), 1, filePtrTex);

    // Read the info header
    fread(&bitmapInfoHeaderTex, sizeof(BITMAPINFOHEADER), 1, filePtrTex);

    // Get the width and height (width and length) of the image
    hminfo.terrainWidth = bitmapInfoHeaderTex.biWidth;
    hminfo.terrainHeight = bitmapInfoHeaderTex.biHeight;

    // if the width is odd, then we need to add padding
    const uint32_t bitCount = 24;
    LONG strideInBytes;
    if (hminfo.terrainWidth & 0x1)
    {
        strideInBytes = ((((hminfo.terrainWidth * bitCount) + 31) & ~31) >> 3);
    }
    else
    {
        strideInBytes = hminfo.terrainWidth * 3;
    }

    // Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
    image_size = strideInBytes * hminfo.terrainHeight;

    // Initialize the arrays which store the image data
    unsigned char* bitmapImageTex = new unsigned char[image_size];

    // Set the file pointers to the beginning of the image data
    fseek(filePtrTex, bitmapFileHeaderTex.bfOffBits, SEEK_SET);

    // Store image data in bitmapImages
    fread(bitmapImageTex, 1, image_size, filePtrTex);

    // Close files
    fclose(filePtrTex);

    // Determine how many loadable chunks the image contains on the Z axis (y axis in the actual image)
    // This is important to determine the number of unused bytes in each row
    int num_chunks_z = (hminfo.terrainHeight) / CHUNK_AND_SEAM_SIZE;
    int num_chunks_x = (hminfo.terrainWidth) / CHUNK_AND_SEAM_SIZE;

    //if there is terrain data beyond this chunk, we load the next several rows to calculate normal data for our seams correctly.
    //it will take a little more time, but the end result will be prettier.
    loaded_cols = CHUNK_AND_SEAM_SIZE + 3 * (chunk_id.m_y < num_chunks_z - 1) + 3 * (chunk_id.m_y > 0);
    loaded_rows = CHUNK_AND_SEAM_SIZE + 3 * (chunk_id.m_x < num_chunks_x - 1) + 3 * (chunk_id.m_x > 0);
    if (chunk_id.m_y < num_chunks_z - 1) m_has_forward_seam = true;
    if (chunk_id.m_x < num_chunks_x - 1) m_has_right_seam = true;
    if (chunk_id.m_y > 0) m_bottom_buffer = true;
    if (chunk_id.m_x > 0) m_left_buffer = true;

    // Initialize the heightMap array (stores the vertices of our terrain)
    hminfo.terrain_color = new Vector4D[loaded_cols * loaded_rows];

    //the number of unused_bytes per row in the bmp will vary depending on if there is an even or odd number of chunks in the image
    //int unused_bytes = 1 * (num_chunks_z % 2 == 1) + 2 * (num_chunks_z % 2 == 0);
    int unused_bytes = num_chunks_z % 4;

    // We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
    // So we use this counter to skip the next two components in the image data (we read R, then skip BG)
    int k = 0;

    //calculate the data offsets for the chunk we will load
    int x_offset = (((chunk_id.m_x * CHUNK_AND_SEAM_SIZE) * hminfo.terrainHeight) + (chunk_id.m_y * CHUNK_AND_SEAM_SIZE)) * 3;

    //adjust the x offset to account for the unused bytes at the end of each row of pixel data
    x_offset += unused_bytes * CHUNK_AND_SEAM_SIZE * chunk_id.m_x;
    //if we are reading the left and bottom portions of the map as well we need to push the x offset backwards
    if (m_bottom_buffer) x_offset -= 3 * 3;
    if (m_left_buffer) x_offset -= strideInBytes * 3;

    //z offset is the height of the total image, minus the size of just the chunk we are loading.
    //int z_offset = CHUNK_AND_SEAM_SIZE * (num_chunks_z - 1) * 3 + unused_bytes;
    int z_offset = strideInBytes - loaded_cols * 3;
    //int z_offset = strideInBytes - loaded_cols * 3 + 6;
    int full_row_offset = strideInBytes;


    // Read the image data into our heightMap array
    for (int j = 0; j < loaded_rows; j++)
    {
        for (int i = 0; i < loaded_cols; i++)
        {
            index = (loaded_cols * j) + i;

            float r = bitmapImageTex[k + x_offset];
            float g = bitmapImageTex[k + x_offset + 1];
            float b = bitmapImageTex[k + x_offset + 2];

            hminfo.terrain_color[index].m_x = r;
            hminfo.terrain_color[index].m_y = g;
            hminfo.terrain_color[index].m_z = b;

            k += 3;
        }

        k += z_offset;
    }

    delete[] bitmapImageTex;
    bitmapImageTex = 0;

    return true;
}

float Terrain::sampleHeight(const unsigned char* bmp, int step, int start_offset, int row_step, int bmp_max)
{
    float count = 1;
    float h = bmp[step + start_offset];


    //if (step + start_offset - row_step * 4 < bmp_max)
    //{
    //    h += bmp[step + start_offset + row_step * 4];
    //                                 
    //    h += bmp[step + start_offset + row_step * 3 + 3];
    //    h += bmp[step + start_offset + row_step * 3];
    //    h += bmp[step + start_offset + row_step * 3 - 3];
    //                                 
    //    h += bmp[step + start_offset + row_step * 2 + 6];
    //    h += bmp[step + start_offset + row_step * 2 + 3];
    //    h += bmp[step + start_offset + row_step * 2];
    //    h += bmp[step + start_offset + row_step * 2 - 3];
    //    h += bmp[step + start_offset + row_step * 2 - 6];
    //                                 
    //    h += bmp[step + start_offset + row_step + 9];
    //    h += bmp[step + start_offset + row_step + 6];
    //    h += bmp[step + start_offset + row_step + 3];
    //    h += bmp[step + start_offset + row_step];
    //    h += bmp[step + start_offset + row_step - 3];
    //    h += bmp[step + start_offset + row_step - 6];
    //    h += bmp[step + start_offset + row_step - 9];
    //                                 
    //    h += bmp[step + start_offset + 12];
    //    h += bmp[step + start_offset + 9];
    //    h += bmp[step + start_offset + 6];
    //    h += bmp[step + start_offset + 3];
    //    count += 20.0f;
    //}
    if (step + start_offset + row_step * 3 < bmp_max)
    {
        h += bmp[step + start_offset + row_step * 3];

        h += bmp[step + start_offset + row_step * 2 + 3];
        h += bmp[step + start_offset + row_step * 2];
        h += bmp[step + start_offset + row_step * 2 - 3];

        h += bmp[step + start_offset + row_step + 6];
        h += bmp[step + start_offset + row_step + 3];
        h += bmp[step + start_offset + row_step];
        h += bmp[step + start_offset + row_step - 6];
        h += bmp[step + start_offset + row_step - 3];
        h += bmp[step + start_offset + 9];
        h += bmp[step + start_offset + 6];
        h += bmp[step + start_offset + 3];
        count += 12.0f;
    }
    else if (step + start_offset + row_step * 2 < bmp_max)
    {
        h += bmp[step + start_offset + row_step * 2];
        h += bmp[step + start_offset + row_step + 3];
        h += bmp[step + start_offset + row_step];
        h += bmp[step + start_offset + row_step - 3];
        h += bmp[step + start_offset + 6];
        h += bmp[step + start_offset + 3];
        count += 6.0f;
    }
    else if (step + start_offset + row_step < bmp_max)
    {
        h += bmp[step + start_offset + row_step];
        h += bmp[step + start_offset + 3];
        count += 2.0f;
    }
    else if (step + start_offset + 3 < bmp_max)
    {
        h += bmp[step + start_offset + 3];
        count++;
    }

    //if (step + start_offset - row_step * 4 >= 0)
    //{
    //    h += bmp[step + start_offset - row_step * 4];

    //    h += bmp[step + start_offset - row_step * 3 + 3];
    //    h += bmp[step + start_offset - row_step * 3];
    //    h += bmp[step + start_offset - row_step * 3 - 3];

    //    h += bmp[step + start_offset - row_step * 2 + 6];
    //    h += bmp[step + start_offset - row_step * 2 + 3];
    //    h += bmp[step + start_offset - row_step * 2];
    //    h += bmp[step + start_offset - row_step * 2 - 3];
    //    h += bmp[step + start_offset - row_step * 2 - 6];

    //    h += bmp[step + start_offset - row_step + 9];
    //    h += bmp[step + start_offset - row_step + 6];
    //    h += bmp[step + start_offset - row_step + 3];
    //    h += bmp[step + start_offset - row_step];
    //    h += bmp[step + start_offset - row_step - 3];
    //    h += bmp[step + start_offset - row_step - 6];
    //    h += bmp[step + start_offset - row_step - 9];

    //    h += bmp[step + start_offset - 12];
    //    h += bmp[step + start_offset - 9];
    //    h += bmp[step + start_offset - 6];
    //    h += bmp[step + start_offset - 3];
    //    count += 20.0f;
    //}
    if (step + start_offset - row_step * 3 >= 0)
    {
        h += bmp[step + start_offset - row_step * 3];
                                     
        h += bmp[step + start_offset - row_step * 2 + 3];
        h += bmp[step + start_offset - row_step * 2];
        h += bmp[step + start_offset - row_step * 2 - 3];
                                     
        h += bmp[step + start_offset - row_step + 6];
        h += bmp[step + start_offset - row_step + 3];
        h += bmp[step + start_offset - row_step];
        h += bmp[step + start_offset - row_step - 6];
        h += bmp[step + start_offset - row_step - 3];

        h += bmp[step + start_offset - 9];
        h += bmp[step + start_offset - 6];
        h += bmp[step + start_offset - 3];
        count += 12.0f;
    }
    else if (step + start_offset - row_step * 2 >= 0)
    {
        h += bmp[step + start_offset - row_step * 2];
        h += bmp[step + start_offset - row_step + 3];
        h += bmp[step + start_offset - row_step];
        h += bmp[step + start_offset - row_step - 3];
        h += bmp[step + start_offset - 6];
        h += bmp[step + start_offset - 3];
        count += 6.0f;
    }
    else if (step + start_offset - row_step >= 0)
    {
        h += bmp[step + start_offset - row_step];
        h += bmp[step + start_offset - 3];
        count += 2.0f;
    }
    else if (step + start_offset - 3 >= 0)
    {
        h += bmp[step + start_offset - 3];
        count++;
    }

    return (h / count);
}

void Terrain::findCaps(const std::vector<VertexMesh> chunk, int height, int width, VertexMesh *topleft, VertexMesh *topright, VertexMesh *right)
{
    //necessary variables
    int offset_x = 3 * (m_left_buffer); 
    int offset_z = 3 * (m_bottom_buffer);

    Vector3D h1, h2, h3, h4;
    Vector4D t1, t2, t3, t4;
    Vector3D n1, n2, n3, n4;

        
    //Bottom right cap
    /* if there is no bottom buffer, put in a dummy value */
    if (!m_bottom_buffer)
    {
        *right = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)];
    }
    else
    {
        h1 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)].m_position;
        h2 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1) - 1].m_position;
        h3 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x)].m_position;
        h4 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x) - 1].m_position;

        n1 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)].m_normal;
        n2 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1) - 1].m_normal;
        n3 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x)].m_normal;
        n4 = chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x) - 1].m_normal;

        t1.loadFloat(&chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)].m_bone_influence[0]);
        t2.loadFloat(&chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1) - 1].m_bone_influence[0]);
        t3.loadFloat(&chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x)].m_bone_influence[0]);
        t4.loadFloat(&chunk[offset_z + height * (CHUNK_AND_SEAM_SIZE + offset_x) - 1].m_bone_influence[0]);

        *right = VertexMesh(Vector3D((h1 + h2 + h3 + h4) / 4.0f), Vector3D((n1 + n2 + n3 + n4) / 4.0f), Vector4D((t1 + t2 + t3 + t4) / 4.0f));
    }

    //Top Right Cap
    h1 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)].m_position;
    h2 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1) - 1].m_position;
    h3 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x)].m_position;
    h4 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x) - 1].m_position;

    n1 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)].m_normal;
    n2 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1) - 1].m_normal;
    n3 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x)].m_normal;
    n4 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x) - 1].m_normal;

    t1.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1)].m_bone_influence[0]);
    t2.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x - 1) - 1].m_bone_influence[0]);
    t3.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x)].m_bone_influence[0]);
    t4.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (CHUNK_AND_SEAM_SIZE + offset_x) - 1].m_bone_influence[0]);

    *topright = VertexMesh(Vector3D((h1 + h2 + h3 + h4) / 4.0f), Vector3D((n1 + n2 + n3 + n4) / 4.0f), Vector4D((t1 + t2 + t3 + t4) / 4.0f));

    //Top Left Cap

    /* If there is nothing to the left of the chunk (beginning of the array, put in a dummy value and return to avoid a negative array value */
    if (!m_left_buffer)
    {
        *topleft = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x)];
        return;
    }

    h1 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x - 1)].m_position;
    h2 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x - 1) - 1].m_position;
    h3 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x)].m_position;
    h4 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x) - 1].m_position;

    n1 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x - 1)].m_normal;
    n2 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x - 1) - 1].m_normal;
    n3 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x)].m_normal;
    n4 = chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x) - 1].m_normal;

    t1.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x - 1)].m_bone_influence[0]);
    t2.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x - 1) - 1].m_bone_influence[0]);
    t3.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x)].m_bone_influence[0]);
    t4.loadFloat(&chunk[offset_z + CHUNK_AND_SEAM_SIZE + height * (offset_x) - 1].m_bone_influence[0]);

    *topleft = VertexMesh(Vector3D((h1 + h2 + h3 + h4) / 4.0f), Vector3D((n1 + n2 + n3 + n4) / 4.0f), Vector4D((t1 + t2 + t3 + t4) / 4.0f));
}



void Terrain::loadChunkFromText(bool from_generated)
{
    std::string file = std::to_string((int)(m_pos.m_x)) + "_" + std::to_string((int)(m_pos.m_y)) + "_chunk.txt";
    std::ifstream fin;

    //if (from_generated)  fin = std::ifstream("..\\ChunkData\\Generated\\" + file);
    //else fin = std::ifstream("..\\ChunkData\\" + file);
    fin = std::ifstream("..\\ChunkData\\" + file);

    _ASSERT_EXPR(fin.is_open(), L"Terrain Data File not found!");

    Vector3D pos, norm;
    Vector4D texsplat;

    for (int i = 0; i < CHUNK_AND_SEAM_SIZE * CHUNK_AND_SEAM_SIZE; i++)
    {
        fin >> pos.m_x >> pos.m_y >> pos.m_z;
        pos = pos * PRELOADED_SCALE;
        fin >> norm.m_x >> norm.m_y >> norm.m_z;
        fin >> texsplat.m_x >> texsplat.m_y >> texsplat.m_z >> texsplat.m_w;
        //TODO: As a temporary fix, we are reversing the w value (cliff face value) as a mistake was made and they were all saved as negative values before.
        //next time new terrain is generated we need to reverse the calculation.
        texsplat.m_w *= -1;

        m_vec_vm_chunk.push_back(VertexMesh(pos, norm, texsplat));
    }
}

void Terrain::loadSeamRFromText(bool from_generated)
{
    std::string file = std::to_string((int)(m_pos.m_x)) + "_" + std::to_string((int)(m_pos.m_y)) + "_seamR.txt";
    std::ifstream fin;

    //if (from_generated)  fin = std::ifstream("..\\ChunkData\\Generated\\" + file);
    //else fin = std::ifstream("..\\ChunkData\\" + file);
    fin = std::ifstream("..\\ChunkData\\" + file);
    
    //if there is no seam for this chunk, simply return
    if (!fin) return;

    Vector3D pos, norm;
    Vector4D texsplat;

    for (int i = 0; i < CHUNK_AND_SEAM_SIZE * 2 + 2; i++)
    {
        fin >> pos.m_x >> pos.m_y >> pos.m_z;
        pos = pos * PRELOADED_SCALE;
        fin >> norm.m_x >> norm.m_y >> norm.m_z;
        fin >> texsplat.m_x >> texsplat.m_y >> texsplat.m_z >> texsplat.m_w;
        //TODO: As a temporary fix, we are reversing the w value (cliff face value) as a mistake was made and they were all saved as negative values before.
        //next time new terrain is generated we need to reverse the calculation.
        texsplat.m_w *= -1;

        m_vec_vm_seamR.push_back(VertexMesh(pos, norm, texsplat));
    }
}

void Terrain::loadSeamFFromText(bool from_generated)
{
    std::string file = std::to_string((int)(m_pos.m_x)) + "_" + std::to_string((int)(m_pos.m_y)) + "_seamF.txt";
    std::ifstream fin;

    //if (from_generated)  fin = std::ifstream("..\\ChunkData\\Generated\\" + file);
    //else fin = std::ifstream("..\\ChunkData\\" + file);
    fin = std::ifstream("..\\ChunkData\\" + file);

    //if there is no seam for this chunk, simply return
    if (!fin) return;

    Vector3D pos, norm;
    Vector4D texsplat;

    for (int i = 0; i < CHUNK_AND_SEAM_SIZE * 2 + 2; i++)
    {
        fin >> pos.m_x >> pos.m_y >> pos.m_z;
        pos = pos * PRELOADED_SCALE;
        fin >> norm.m_x >> norm.m_y >> norm.m_z;
        fin >> texsplat.m_x >> texsplat.m_y >> texsplat.m_z >> texsplat.m_w;
        //TODO: As a temporary fix, we are reversing the w value (cliff face value) as a mistake was made and they were all saved as negative values before.
        //next time new terrain is generated we need to reverse the calculation.
        texsplat.m_w *= -1;

        m_vec_vm_seamF.push_back(VertexMesh(pos, norm, texsplat));
    }
}

