#include "sprite.hlsli"  

VS_OUT main(float4 pos : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) 
{
	VS_OUT vout;
	vout.pos = pos; 
	vout.color = color;  // UNIT.04  
	vout.texcoord = texcoord;  
	return vout; 
}