struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 Position : SV_POSITION;
};

cbuffer ShadowData : register(b0) {
	matrix world;
	matrix shadowView;
	matrix shadowProjection;
}

VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;

	matrix WVP = mul(mul(world, shadowView), shadowProjection);
	output.Position = mul(float4(input.position, 1.0f), WVP);

	return output;
}