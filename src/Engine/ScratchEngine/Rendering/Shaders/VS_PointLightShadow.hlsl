#include "Lighting.hlsli"


struct VertexShaderInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    uint4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
};


cbuffer LightData : register(b1)
{
    LightSource light;
};

cbuffer CameraData : register(b2)
{
    matrix view;
    matrix projection;
    matrix viewProjection;
};

cbuffer ObjectData : register(b3)
{
    matrix world;
    matrix gBoneTransforms[128];
};


VertexToPixel main(VertexShaderInput input)
{
    float4 p;
    
    if (input.position.w == 0)
        p = float4(input.position.xyz, 1.0f);
    else
    {
        p = float4(0, 0, 0, 0);

        for (int i = 0; i < 4; ++i)
            p += input.boneWeights[i] * mul(float4(input.position.xyz, 1.0f), gBoneTransforms[input.boneIndices[i]]);

        p.w = 1;
    }


    VertexToPixel output;
    
    output.position = mul(p, mul(world, viewProjection));
    output.worldPosition = mul(p, world);


    return output;
}