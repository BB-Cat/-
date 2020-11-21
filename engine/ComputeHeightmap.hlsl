#include "noise.fx"

//input
struct InputData
{
    float4 position1;
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
    float yscale = 20;
    float xscale = 10;
    float cell_size = 2000;

    //if xscale and yscale are going to be determined seperately, the x scale needs to be applied before noise sampling for both the
    //current vertex and neighbors otherwise normal calculations will bug out.

    int chunk_and_seam_size = 34;

    int2 uv;
    uv.x = id.x % chunk_and_seam_size;
    uv.y = (id.x / chunk_and_seam_size) % chunk_and_seam_size;
    int z = id.x / (chunk_and_seam_size * chunk_and_seam_size) % 100;
    int z2 = id.x / (chunk_and_seam_size * chunk_and_seam_size) / 100;
    uv.y += z *  (chunk_and_seam_size - 1);
    uv.x += z2 * (chunk_and_seam_size - 1);

    //get the noise for this vertice
    //calculate the xz coordinates for the current vertex. 
    /* it is important that both the position and neighbor xz positions are 
    calculated seperately and before noise, otherwise the noise samples will be different*/
    float3 pos = float3(uv.x, 0, uv.y) * xscale;



    pos.y = brownianPerlin(pos.xz / cell_size, 8, 0.8, 1.4, 20.0) * yscale;


    //-----------------------------------------------------------------//
    //  NORMAL CALCULATION
    //-----------------------------------------------------------------//

    ////adjacent height samples
    float4 h;
    h[0] = brownianPerlin((pos.xz + xscale * float2(0, -1)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale; //bottom
    h[1] = brownianPerlin((pos.xz + xscale * float2(-1, 0)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale; //left
    h[2] = brownianPerlin((pos.xz + xscale * float2(0, 1)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;  //right
    h[3] = brownianPerlin((pos.xz + xscale * float2(1, 0)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;  //top
    //semi adjacent height samples
    float4 h2;
    h2[0] = brownianPerlin((pos.xz + xscale * float2(0, -2)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale; //bottom
    h2[1] = brownianPerlin((pos.xz + xscale * float2(-2, 0)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale; //left
    h2[2] = brownianPerlin((pos.xz + xscale * float2(0, 2)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;  //right
    h2[3] = brownianPerlin((pos.xz + xscale * float2(2, 0)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;  //top
    //corner samples
    float4 h3;
    h3[0] = brownianPerlin((pos.xz + xscale * float2(1, 1)) / cell_size, 8, 0.8, 1.4, 20.0)  * yscale; //top right
    h3[1] = brownianPerlin((pos.xz + xscale * float2(1, -1)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;//bottom right
    h3[2] = brownianPerlin((pos.xz + xscale * float2(-1,-1)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;//bottom left
    h3[3] = brownianPerlin((pos.xz + xscale * float2(-1, 1)) / cell_size, 8, 0.8, 1.4, 20.0) * yscale;//top left

    float3 norm;
    norm.z = h[0] + (h3[1] + h3[2] + h2[0]) * 0.5 - (h[3] + (h3[0] + h3[3] + h2[3]) * 0.5); //bottom minus top
    norm.x = h[1] + (h3[2] + h3[3] + h2[1]) * 0.5 - (h[2] + (h3[0] + h3[1] + h2[2]) * 0.5); //left minus right
    //norm.z = h[0] +  - (h[3]); //bottom minus top
    //norm.x = h[1] +  - (h[2]); //left minus right
    norm.y = yscale * 2.5;

    //-----------------------------------------------------------------//
    //-----------------------------------------------------------------//

    //texture splatting
    float3 terrain = 0;
    terrain.x = min(brownianVoronoi(pos / (cell_size / 3), 4, 0.6, 1.2, 0.7), 1);
    terrain.y = 1.0 - terrain.x;



    OutputData op;
    op.position = pos;
    op.normal = normalize(norm);
    op.texcoord = uv % 2;
    op.terrain_type = float4(normalize(terrain), 1);
    op.unused = 0;

    output[id.x] = op;

    //output[id.x].position1 = float4(final, 1);

}