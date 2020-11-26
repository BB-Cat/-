#include "noise.fx"

//input
struct InputData
{
    float2 origin;
    float2 range;
};

//output
struct OutputData
{
    float3 position;
    float2 texcoord;
    float3 normal;
    float4 terrain_type;
    float4 unused;
};

StructuredBuffer<InputData>             input   : register(t0);
RWStructuredBuffer<OutputData>          output  : register(u0);


int loopPositive(int input, int limit)
{
    if (input < 0) return input + limit;
    else return input;
}

//float3 filterNormalFromNoise(float2 uv, float y, float xscale, float yscale, float cell_size)
//{
//    float4 h;
//    h[0] = brownianPerlin((uv + xscale * float2(0, -1)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
//    h[1] = brownianPerlin((uv + xscale * float2(-1, 0)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
//    h[2] = brownianPerlin((uv + xscale * float2(1,  0)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
//    h[3] = brownianPerlin((uv + xscale * float2(0,  1)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
//
//    float3 n;
//    n.z = h[0] - h[3];
//    n.x = h[1] - h[2];
//    n.y = yscale;
//
//    return n;
//}

//The number of threads should be a MULTIPLE OF 32 FOR NVIDIA GPU'S
[numthreads(34, 1, 1)]
void CS_main(int3 id : SV_DispatchThreadID)
{
    float yscale = m_yscale;
    float xscale = m_xscale;
    //float cell_size = 2000;

    //define the chunk size here.  this value will not change.
    int chunk_and_seam_size = 34;

    ////calculate the in-chunk uv position
    //int2 uv;
    //uv.x = id.x % chunk_and_seam_size;
    //uv.y = (id.x / chunk_and_seam_size) % chunk_and_seam_size;

    ////calculate where the chunk is relative to the other chunks
    //int chunk_x = id.x / (chunk_and_seam_size * chunk_and_seam_size);
    //int chunk_y = chunk_x / input[0].range.x;
    //chunk_x = chunk_x % input[0].range.x;

    int chunk_num = id.x / (chunk_and_seam_size * chunk_and_seam_size);
    //int chunk_x = chunk_num % input[0].range.x;
    //int chunk_y = chunk_num / input[0].range.x;

    //int chunk_x = (chunk_num) % (int)(input[0].range.x);
    int chunk_x = chunk_num % (int)(input[0].range.x);
    int chunk_y = (chunk_num) / (int)(input[0].range.x);
    //int chunk_y = chunk_num;

    float2 uv;
    uv.x = id.x % chunk_and_seam_size;
    uv.y = (id.x % (chunk_and_seam_size * chunk_and_seam_size)) / chunk_and_seam_size;

    //slide the uv's
    uv.x += chunk_x * (chunk_and_seam_size - 1);
    uv.y += chunk_y * (chunk_and_seam_size - 1);

    //add the origin value
    uv += input[0].origin * (chunk_and_seam_size - 1);

    //scale the UV by xscale to get the real world position of the vertice
    float3 pos = float3(uv.x, 0, uv.y) * xscale;
    pos /= m_compute_cell_size;

    //calculate the y value for this vertice
    pos.y = calculateHeightMap(pos.xz) * yscale;


    //-----------------------------------------------------------------//
    //  NORMAL CALCULATION
    //-----------------------------------------------------------------//

    //adjacent height samples
    float4 h = 0;
    h[0] = clamp(calculateHeightMap(pos.xz  + (3 * xscale * float2(0, -1)) / m_compute_cell_size), 0, 1) * yscale; //bottom
    h[1] = clamp(calculateHeightMap(pos.xz  + (3 * xscale * float2(-1, 0)) / m_compute_cell_size), 0, 1) * yscale; //left
    h[2] = clamp(calculateHeightMap(pos.xz  + (3 * xscale * float2(0,  1)) / m_compute_cell_size), 0, 1) * yscale;  //right
    h[3] = clamp(calculateHeightMap(pos.xz  + (3 * xscale * float2(1,  0)) / m_compute_cell_size), 0, 1) * yscale;  //top
    //semi adjacent height samples      
    float4 h2 = 0;                      
    h2[0] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(0, -2)) / m_compute_cell_size), 0, 1) * yscale; //bottom
    h2[1] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(-2, 0)) / m_compute_cell_size), 0, 1) * yscale; //left
    h2[2] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(0,  2)) / m_compute_cell_size), 0, 1) * yscale;  //right
    h2[3] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(2,  0)) / m_compute_cell_size), 0, 1) * yscale;  //top
    //corner samples                
    float4 h3 = 0;                  
    h3[0] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(1,  1)) / m_compute_cell_size), 0, 1) * yscale; //top right
    h3[1] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(1, -1)) / m_compute_cell_size), 0, 1) * yscale;//bottom right
    h3[2] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(-1,-1)) / m_compute_cell_size), 0, 1) * yscale;//bottom left
    h3[3] = clamp(calculateHeightMap(pos.xz + (3 * xscale * float2(-1, 1)) / m_compute_cell_size), 0, 1) * yscale;//top left
    //calculate the normal vector.  (still not normalized)
    float3 norm;
    norm.z = h[0] + (h3[1] + h3[2] + h2[0]) * 0.5 - (h[3] + (h3[0] + h3[3] + h2[3]) * 0.5); //bottom minus top
    norm.x = h[1] + (h3[2] + h3[3] + h2[1]) * 0.5 - (h[2] + (h3[0] + h3[1] + h2[2]) * 0.5); //left minus right

    norm.y = 25;//yscale * (1.0 - ((norm.x + norm.z) / yscale));

    //float3 norm = float3(0, 1, 0);

    //-----------------------------------------------------------------//
    //-----------------------------------------------------------------//

    //texture splatting
    float3 terrain = 0;
    //terrain.x = min(brownianVoronoi(pos / (cell_size / 3), 4, 0.6, 1.2, 0.7), 1);
    //terrain.x = 0.5;
    terrain.x = perlinNoise((pos.xz) * 5) + 0.5;
    terrain.y = perlinNoise((pos.xz + float2(984, 30)) * 5) + 0.5;
    terrain.z = perlinNoise((pos.xz + float2(4, -2530)) * 5) + 0.5;
    terrain.x = easeInOut(terrain.x);
    terrain.y = easeInOut(terrain.y);
    terrain.z = easeInOut(clamp(1.0 - abs(terrain.z), 1, 0));

    //terrain.y = 1 - terrain.x;
    terrain = normalize(terrain);


    OutputData op;
    pos.xz /= xscale;
    pos.xz *= m_compute_cell_size;
    pos.xz *= 3; //generic mesh scaling


    op.position = pos;
    op.normal = normalize(norm);
    op.texcoord = uv % 2;
    op.terrain_type = float4(normalize(terrain), 1);
    op.unused = 0;

    output[id.x] = op;
}