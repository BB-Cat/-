
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


[numthreads(1, 1, 1)]
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


    float r = (dispatchThreadID.x % 32.0) / 32.0;
    float g = (dispatchThreadID.x / 32.0) / 32.0;
    float b = (dispatchThreadID.x) / (32.0 * 32.0);
    float a = 1;

    //float r = dispatchThreadID.x / 32.0;
    //float g = 0;
    //float b = 1;
    //float a = 2;

    outputParticleData[dispatchThreadID.x].position1 = float4(r, g, b, a);
    //outputParticleData[dispatchThreadID.x].position1 = float4(1, 1, 1, 1);

}