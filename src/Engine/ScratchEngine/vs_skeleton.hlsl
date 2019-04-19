

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 boneIndex : BLENDINDICES;
	float4 weight : BLENDWEIGHT;
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
};

cbuffer cbSkinned : register(b4)
{
	matrix gBoneTransforms[69];
};

struct VertexToPixel
{
	float4 svPosition : SV_POSITION;
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

VertexToPixel main(VertexShaderInput vin)
{
	VertexToPixel vout;

	// balance the bone weight
	float weight0 = vin.weight.x;
	float weight1 = vin.weight.y;
	float weight2 = vin.weight.z;
	float weight3 = vin.weight.w;
	float sum = weight0 + weight1;
		//+ weight2 + weight3;
	weight0 /= sum;
	weight1 /= sum;
	//weight2 /= sum;
	//weight3 /= sum;
	// offset position by bone matrices, using weights to scale
	float4 p = weight0 * mul(float4(vin.position, 1.0f), gBoneTransforms[(int)vin.boneIndex[0]]);
	p += weight1 * mul(float4(vin.position, 1.0f), gBoneTransforms[(int)vin.boneIndex[1]]);
	//p += weight2 * mul(float4(vin.position, 1.0f), gBoneTransforms[(int)vin.boneIndex[2]]);
	//p += weight3 * mul(float4(vin.position, 1.0f), gBoneTransforms[(int)vin.boneIndex[3]]);
	p.w = 1.0f;

	// offset normal by bone matrices, using weights to scale
	float4 n = weight0 * mul(float4(vin.normal, 0.0f), gBoneTransforms[(int)vin.boneIndex[0]]);
	n += weight1 * mul(float4(vin.normal, 0.0f), gBoneTransforms[vin.boneIndex[1]]);
	//n += weight2 * mul(float4(vin.normal, 0.0f), gBoneTransforms[vin.boneIndex[2]]);
	//n += weight3 * mul(float4(vin.normal, 0.0f), gBoneTransforms[vin.boneIndex[3]]);
	n.w = 0.0f;

	// offset tangent by bone matrices, using weights to scale
	//float4 t = weight0 * mul(float4(vin.Tan.xyz, 0.0f), gBoneTransforms[vin.BoneIndex[0]]);
	//t += weight1 * mul(float4(vin.Tan.xyz, 0.0f), gBoneTransforms[vin.BoneIndex[1]]);
	//t += weight2 * mul(float4(vin.Tan.xyz, 0.0f), gBoneTransforms[vin.BoneIndex[2]]);
	//t += weight3 * mul(float4(vin.Tan.xyz, 0.0f), gBoneTransforms[vin.BoneIndex[3]]);
	//t.w = 0.0f;


	// Transform to world space.
	vout.position = mul(p, world);
	vout.normal = normalize(mul(n.xyz, (float3x3)world));
	//vout.TangentW = float4(mul(t, (float3x3)gWorld), vin.Tan.w);

	matrix WVP = mul(world, viewProjection);


	vout.svPosition = mul(p, WVP);

	vout.uv = vin.uv;

	return vout;
}
