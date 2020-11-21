#include "noise.fx"

//input
struct ConstantParticleData
{
    float4 position1;
};

//output
struct ParticleData
{
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
    float2 uv = float2((dispatchThreadID.x % 512.0) / 512.0, dispatchThreadID.x / 512.0 / 512.0);

    //new
    float2 pos = uv * m_xscale;
    float value = 1 - abs(brownianPerlin(pos / m_compute_cell_size, 5, 0.6, 1.4, 1.0) * m_yscale);
    //


    //float3 value = float3(uv.xy, 1);
    //float noise = (1.0 - brownianTiledVoronoi(value, (int)2, (int)4, 0.3, 2, 0.8).x);
    //float3 final = noise;
    //if (!m_compute_cell_size) final = 0;
    float3 final = value;
    outputParticleData[dispatchThreadID.x].position1 = float4(final, 1);
}