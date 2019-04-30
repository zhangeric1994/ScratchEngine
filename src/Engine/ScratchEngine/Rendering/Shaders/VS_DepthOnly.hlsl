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
    float4 svPosition : SV_POSITION;
};


cbuffer CameraData : register(b2)
{
    matrix view;
    matrix projection;
    matrix viewProjection;
    matrix shadowViewProjection;
};

cbuffer ObjectData : register(b3)
{
    matrix world;
};

cbuffer cbSkinned : register(b4)
{
    matrix gBoneTransforms[128];
};


float4 main(VertexShaderInput input) : SV_POSITION
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

    return mul(p, mul(world, viewProjection));
}