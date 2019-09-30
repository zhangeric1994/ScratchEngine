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
    float4 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
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
    matrix gBoneTransforms[128];
};


VertexToPixel main(VertexShaderInput input)
{
    float4 p = float4(0, 0, 0, 0);
    float4 n = float4(0, 0, 0, 0);

    for (int i = 0; i < 4; ++i)
    {
        float weight = input.boneWeights[i];
        float4x4 m = gBoneTransforms[input.boneIndices[i]];
        p += weight * mul(float4(input.position.xyz, 1.0f), m);
        n += weight * mul(float4(input.normal, 0.0f), m);
    }

    p.w = 1;
    n.w = 0;
        

    VertexToPixel output;

    output.svPosition = mul(p, mul(world, viewProjection));
    output.worldPosition = mul(p, world);
    output.normal = normalize(mul(n.xyz, (float3x3) world));
    output.tangent = input.tangent;
    output.uv = input.uv;


    return output;
}
