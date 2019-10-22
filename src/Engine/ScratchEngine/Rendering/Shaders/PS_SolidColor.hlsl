#include "Lighting.hlsli"


cbuffer FrameData : register(b0)
{
    int GammaCorrection;
}


cbuffer MaterialData : register(b3)
{
    float4 tint;
}


float4 main(float4 position : SV_POSITION) : SV_TARGET
{
	// Tint the surface color by the light
    float3 color = tint.rgb;


	// Adjust the light color by the light amount
    return float4(lerp(color, pow(color, 1.0f / 2.2f), GammaCorrection), 1);
}