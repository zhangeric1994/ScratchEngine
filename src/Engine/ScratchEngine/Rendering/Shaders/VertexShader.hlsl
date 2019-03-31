struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
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


VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;

    matrix WVP = mul(world, viewProjection);

    output.svPosition = mul(float4(input.position, 1.0f), WVP);
    output.position = mul(float4(input.position, 1.0f), world);
    output.normal = normalize(mul(input.normal, (float3x3)world));
	output.uv = input.uv;

	return output;
}