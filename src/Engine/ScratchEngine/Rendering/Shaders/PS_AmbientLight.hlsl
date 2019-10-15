#include "Lighting.hlsli"


struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 viewRay : TEXCOORD0;
};


// Data that only changes once per frame
cbuffer FrameData : register(b0)
{
	// Runtime options
    int GammaCorrection;
};

// Data that can change per light
cbuffer LightData : register(b3)
{
    LightSource light;
};


// Textures and such
Texture2D gBufferAlbedo : register(t0);
Texture2D ssaoBuffer : register(t1);


float4 main(VertexToPixel input) : SV_TARGET
{
    float3 pixelIndex = float3(input.position.xy, 0);


    float3 albedo = gBufferAlbedo.Load(pixelIndex).rgb;


    float3 ssao = ssaoBuffer.Load(pixelIndex).rgb;

    ssao.r = lerp(1.0, ssao.r, saturate(light.hasShadow));
    ssao.g = lerp(1.0, ssao.g, saturate(light.hasShadow));
    ssao.b = lerp(1.0, ssao.b, saturate(light.hasShadow));


    return float4(albedo * light.color.rgb * ssao, 1);
}