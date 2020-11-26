#include "noise.fx"

//input
struct InputData
{
    float4 position1;
};

//output
struct OutputData
{
    float4 position1;
};

StructuredBuffer<InputData>  input : register(t0);
RWStructuredBuffer<OutputData> output : register(u0);

[numthreads(1024, 1, 1)]
void CS_main(int3 id : SV_DispatchThreadID)
{

    //calculate the uv for this pixel
    float2 uv = float2((id.x % 512.0) / 512.0, id.x / 512.0 / 512.0);
    //get the basic sample position
    float2 sample_pos = (uv * m_xscale);
    //sample_pos /= m_compute_cell_size;
    //sample the noise 
    float3 final = calculateHeightMap(sample_pos);
    //return the result
    output[id.x].position1 = float4(final, 1);
}