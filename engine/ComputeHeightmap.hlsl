#include "noise.fx"

//input
struct InputData
{
    float4 position1;
    //float2 texcoord;
    //float3 normal;
    //float4 terrain_type;
    //float4 unused;
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

float3 filterNormalFromNoise(float2 uv, float y, float xscale, float yscale, float cell_size)
{
    float4 h;
    h[0] = brownianPerlin((uv + xscale * float2(0, -1)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    h[1] = brownianPerlin((uv + xscale * float2(-1, 0)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    h[2] = brownianPerlin((uv + xscale * float2(1,  0)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    h[3] = brownianPerlin((uv + xscale * float2(0,  1)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;

    float3 n;
    n.z = h[0] - h[3];
    n.x = h[1] - h[2];
    n.y = yscale;

    return n;
}



//The number of threads should be a MULTIPLE OF 32 FOR NVIDIA GPU'S
[numthreads(33, 1, 1)]
void CS_main(int3 id : SV_DispatchThreadID)
{
    float yscale = 20;
    float xscale = 10;
    float cell_size = 2000;

    //if xscale and yscale are going to be determined seperately, the x scale needs to be applied before noise sampling for both the
    //current vertex and neighbors otherwise normal calculations will bug out.

    int2 uv;
    uv.x = id.x % 33;
    uv.y = (id.x / 33) % 33;
    int z = id.x / (33 * 33) % 100;
    int z2 = id.x / (33 * 33) / 100;
    uv.y += z * 32;
    uv.x += z2 * 32;


    //get the noise for this vertice
    //float noise = max_height * rand2dTo1d(uv / 10); // min(brownianPerlin(uv, 3, 0.4, 1.3, 1.0), 1);
    
    //float noise = rand2dTo1d(uv.x, uv.y) * yscale;

    //calculate the xz coordinates for the current vertex. 
    /* it is important that both the position and neighbor xz positions are 
    calculated seperately and before noise, otherwise the noise samples will be different*/
    float3 pos = float3(uv.x, 0, uv.y) * xscale;




    ////TEEEEEST!!//

    ////find the other two vertices of the triangle.
    //float3 v2, v3;
    ///*
    //possible pos o---------|
    //             |\     2  |
    //             |  \      |
    //             |    \    |
    //             |  1   \  |
    //possible pos o---------|
    //*/
    //if (uv.y % 2)    //if the y UV is odd numbered, we are in the second triangle in the quad
    //{
    //    v2 = pos + float3(1, 0,  0) * xscale;
    //    v3 = pos + float3(1, 0, -1) * xscale;
    //}
    //else // if not, we are in the first triangle in the quad
    //{
    //    v2 = pos + float3(0, 0, 1) * xscale;
    //    v3 = pos + float3(1, 0, 0) * xscale;
    //}
    ////get the y components of each vertex.
    //pos.y = brownianPerlin(pos.xz / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    //v2.y =  brownianPerlin(v2.xz / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    //v3.y =  brownianPerlin(v3.xz / cell_size, 4, 0.8, 1.2, 5.0) * yscale;

    ////calculate triangle normal
    //float3 norm = cross(v2 - pos, v3 - pos);
    //

    ////TEEEEEEST!! END//

    //calculate the neighbor vertice xz coordinates.
    //float3 neighbor_pos[6];
    //neighbor_pos[0] = (pos + float3(-1, 0, 1) * xscale);
    //neighbor_pos[1] = (pos + float3(0, 0, 1)  * xscale);
    //neighbor_pos[2] = (pos + float3(1, 0, 0)  * xscale);
    //neighbor_pos[3] = (pos + float3(1, 0, -1) * xscale);
    //neighbor_pos[4] = (pos + float3(0, 0, -1) * xscale);
    //neighbor_pos[5] = (pos + float3(-1, 0, 0) * xscale);

    ////get the noise for 6 neighboring vertices for normal calculation
    //for (int i = 0; i < 6; i++)
    //{
    //   // neighbor_pos[i].y = rand2dTo1d(neighbor_pos[i].xz) * yscale;

    //    neighbor_pos[i].y = brownianPerlin(neighbor_pos[i].xz / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    //}



    //////TEST//
    //////calculate the neighbor vertice xz coordinates.
    ////float3 neighbor_pos[8];
    ////neighbor_pos[0] = (pos + float3(-1, 0, 1) * xscale);
    ////neighbor_pos[1] = (pos + float3(0, 0, 1)  * xscale);
    ////neighbor_pos[2] = (pos + float3(1, 0, 1)  * xscale);
    ////neighbor_pos[3] = (pos + float3(1, 0, 0) * xscale);
    ////neighbor_pos[4] = (pos + float3(1, 0, -1) * xscale);
    ////neighbor_pos[5] = (pos + float3(0, 0, -1) * xscale);
    ////neighbor_pos[6] = (pos + float3(-1, 0, -1) * xscale);
    ////neighbor_pos[7] = (pos + float3(-1, 0, 0) * xscale);

    //////get the noise for 8 neighboring vertices for normal calculation
    ////for (int i = 0; i < 8; i++)
    ////{
    ////   // neighbor_pos[i].y = rand2dTo1d(neighbor_pos[i].xz) * yscale;

    ////    neighbor_pos[i].y = brownianPerlin(neighbor_pos[i].xz / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    ////}
    //////TEST END//

    ////pos.y = rand2dTo1d(pos.xz) * yscale;
    pos.y = brownianPerlin(pos.xz / cell_size, 8, 0.8, 1.4, 20.0) * yscale;
    ////normal calculations
    //float3 vec1;
    //float3 vec2;
    //float3 norm = 0;


    //for (int i = 0; i < 6; i++)
    //{
    //    vec1 = neighbor_pos[i] - pos;
    //    vec2 = neighbor_pos[loopPositive(i - 1, 6)] - pos;
    //    norm += cross(vec2, vec1);
    //    
    //}
    //norm /= 6;




    //neighbor_pos[0] = (pos + float3(-1, 0, 1) * xscale);
    //neighbor_pos[1] = (pos + float3(0, 0, 1)  * xscale);
    //neighbor_pos[2] = (pos + float3(1, 0, 0)  * xscale);
    //neighbor_pos[3] = (pos + float3(1, 0, -1) * xscale);
    //neighbor_pos[4] = (pos + float3(0, 0, -1) * xscale);
    //neighbor_pos[5] = (pos + float3(-1, 0, 0) * xscale);

    ////float3 area_normals[4];
    //float3 norm = filterNormalFromNoise(pos.xz, 1, xscale, yscale, cell_size);
    ////area_normals[0] = filterNormalFromNoise(pos.xz + float2(-2,0) * xscale, 1, xscale, yscale, cell_size);
    ////area_normals[1] = filterNormalFromNoise(pos.xz + float2(0, 2) * xscale, 1, xscale, yscale, cell_size);
    ////area_normals[2] = filterNormalFromNoise(pos.xz + float2(2, 0) * xscale, 1, xscale, yscale, cell_size);
    ////area_normals[3] = filterNormalFromNoise(pos.xz + float2(0,-2) * xscale, 1, xscale, yscale, cell_size);

    ////float3 norm = area_normals[0] + area_normals[1] + area_normals[2] + area_normals[3];
    ////norm /= 4;

    ///////////////////////////////////////////////////
    //float4 h;
    //h[0] = brownianPerlin((uv + xscale * float2(0, -1)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    //h[1] = brownianPerlin((uv + xscale * float2(-1, 0)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    //h[2] = brownianPerlin((uv + xscale * float2(1, 0)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;
    //h[3] = brownianPerlin((uv + xscale * float2(0, 1)) / cell_size, 4, 0.8, 1.2, 5.0) * yscale;

    //float3 n;
    //n.z = h[0] - h[3];
    //n.x = h[1] - h[2];
    //n.y = yscale;

    //return n;


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


    ////float3 adj_n;
    ////adj_n.z = h[0] - h[3];
    ////adj_n.x = h[1] - h[2];
    ////adj_n.y = yscale;
    float4 h_combined = 0;

    float3 norm;
    norm.z = h[0] + (h3[1] + h3[2] + h2[0]) * 0.5 - (h[3] + (h3[0] + h3[3] + h2[3]) * 0.5); //bottom minus top
    norm.x = h[1] + (h3[2] + h3[3] + h2[1]) * 0.5 - (h[2] + (h3[0] + h3[1] + h2[2]) * 0.5); //left minus right
    //norm.z = h[0] +  - (h[3]); //bottom minus top
    //norm.x = h[1] +  - (h[2]); //left minus right
    norm.y = yscale * 2.5;

    ///////////////////////////////////////////////////



    ////TEST//
    //for (int i = 0; i < 8; i++)
    //{
    //    vec1 = neighbor_pos[i] - pos;
    //    vec2 = neighbor_pos[loopPositive(i - 1, 8)] - pos;
    //    norm += cross(vec2, vec1);

    //}
    //norm /= 8;
    ////TEST END//

    OutputData op;


    op.position = pos;
    op.normal = normalize(norm);
    //op.normal = filterNormalFromNoise(pos.xz, 1, xscale, yscale, cell_size);

    //temp
    //vec1 = normalize(neighbor_pos[0] - pos);
    //vec2 = normalize(neighbor_pos[5] - pos);
    //op.normal = normalize(cross(vec1, vec2));

    //texture splatting
    float3 terrain = 0;
    terrain.x = min(brownianVoronoi(pos / (cell_size / 3), 4, 0.6, 1.2, 0.7), 1); 
    //terrain.y = brownianVoronoi(pos / cell_size, 1, 0.3, 1.2, 5.0); 
    //terrain.z = brownianVoronoi(pos * 40 / cell_size, 2, 0.3, 1.1, 6.0); 
    terrain.y = 1.0 - terrain.x;

    op.texcoord = uv % 2;
    op.terrain_type = float4(normalize(terrain), 1);
    op.unused = 0;

    output[id.x] = op;

    //output[id.x].position1 = float4(final, 1);

}