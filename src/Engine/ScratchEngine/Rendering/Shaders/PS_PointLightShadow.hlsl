#include "Lighting.hlsli"


struct VertexToPixel
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};


// Data that can change per light
cbuffer LightData : register(b3)
{
    LightSource light;
};


float main(VertexToPixel input) : SV_DEPTH
{
    //PixelShaderOutput output;

    //output.color = float4(1, 1, 1, 1);
    //output.depth = length(input.worldPosition.xyz - light.position) / light.range;


    return length(input.worldPosition.xyz - light.position) / light.range;
}