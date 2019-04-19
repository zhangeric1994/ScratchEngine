struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
	float3 direction  : DIRECTION;
};

TextureCube cubeTexture : register(t0);

SamplerState basicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return cubeTexture.Sample(basicSampler, input.direction);
}