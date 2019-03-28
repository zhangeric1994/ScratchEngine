cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{ 
	float3 position : POSITION;
};

struct VertexToPixel
{
	float4 position : SV_POSITION;
};

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
	output.position = mul(float4(input.position, 1.0f), mul(mul(world, view), projection));

	return output;
}