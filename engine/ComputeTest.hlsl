#include "noise.fx"

//input
struct ConstantParticleData
{
    //float3 position;
    //float3 velocity;
    //float3 initialVelocity;
    float4 position1;
};

//output
struct ParticleData
{
    //float3 position;
    //float3 velocity;
    float4 position1;
};

StructuredBuffer<ConstantParticleData>  inputConstantParticleData   : register(t0);
RWStructuredBuffer<ParticleData>        outputParticleData          : register(u0);



//!!!!!!!!!!!!!!!! The number of threads should be a MULTIPLE OF 32 FOR NVIDIA GPU'S
// all threads in a group should be guaranteed to follow the same branch for maximum efficiency!  otherwise both branches will be
//calculated at the same time effectively doubling the overall cost.
[numthreads(1024, 1, 1)]
void CS_main(int3 dispatchThreadID : SV_DispatchThreadID)
{
    //outputParticleData[dispatchThreadID.x].position1 = 3;
    //outputParticleData[dispatchThreadID.x].position2 = 4;
    //outputParticleData[dispatchThreadID.x].position3 = 5;
    //outputParticleData[dispatchThreadID.x].position4 = 6;
    
    //outputParticleData[dispatchThreadID.x].position1 = float3(dispatchThreadID.x + 1, 0, 0);
    //outputParticleData[dispatchThreadID.x].position2 = float3(dispatchThreadID.x + 2, 0, 0);
    //outputParticleData[dispatchThreadID.x].position3 = float3(dispatchThreadID.x + 3, 0, 0);
    //outputParticleData[dispatchThreadID.x].position4 = float3(dispatchThreadID.x + 4, 0, 0);
    //outputParticleData[dispatchThreadID.x].position1 = inputConstantParticleData[dispatchThreadID.x].position1;


    //float2 uv = float2((dispatchThreadID.x % 512.0f) / 512.0f, (dispatchThreadID.x / 512.0f) / 512.0f);
    float2 uv = float2((dispatchThreadID.x % 512.0) / 512.0, dispatchThreadID.x / 512.0 / 512.0);
    float3 value = float3(uv.xy, 1);

    float noise = (1.0 - brownianTiledVoronoi(value, (int)2, (int)4, 0.3, 2, 0.8).x);

    //float3 final = dispatchThreadID.x / (65536.0 * 4);

    //float3 final = 0;
    //final.x = uv.x;
    //final.y = uv.y;
    //float3 final = 1;
    float3 final = noise;
    //float3 final = ((dispatchThreadID.x % 512.0f) / 512.0f);


    outputParticleData[dispatchThreadID.x].position1 = float4(final, 1);
    //outputParticleData[dispatchThreadID.y].position1 = float4(final, 1);
    //outputParticleData[dispatchThreadID.x].position1 = float4(1, 1, 1, 1);

}