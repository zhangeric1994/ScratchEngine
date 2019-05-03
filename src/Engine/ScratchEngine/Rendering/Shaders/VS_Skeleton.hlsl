struct VertexShaderInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
	uint4 boneIndices : BLENDINDICES;
	float4 boneWeights : BLENDWEIGHT;
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

struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 shadowPos : SHADOW;
};

VertexToPixel main(VertexShaderInput vin)
{
	VertexToPixel vout;

    float4 p = float4(0, 0, 0, 0);
    float4 n = float4(0, 0, 0, 0);

    for (int i = 0; i < 4; ++i)
    {
        float weight = vin.boneWeights[i];
        float4x4 m = gBoneTransforms[vin.boneIndices[i]];
        p += weight * mul(float4(vin.position.xyz, 1.0f), m);
        n += weight * mul(float4(vin.normal, 0.0f), m);
    }

    p.w = 1;
    n.w = 0;
        

    matrix WVP = mul(world, viewProjection);
    matrix shadowWVP = mul(world, shadowViewProjection);

    vout.svPosition = mul(p, WVP);
	vout.position = mul(p, world);
    vout.normal = normalize(mul(n.xyz, (float3x3)world));
	vout.uv = vin.uv;
    vout.shadowPos = mul(p, shadowWVP);

	return vout;
}
