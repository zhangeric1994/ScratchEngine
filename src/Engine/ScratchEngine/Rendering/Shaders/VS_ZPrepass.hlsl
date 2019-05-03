struct VertexShaderInput
{
    float3 position : POSITION;
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
};

cbuffer ObjectData : register(b3)
{
    matrix world;
};


VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

    output.svPosition = mul(float4(input.position, 1.0f), mul(world, viewProjection));

	return output;
}