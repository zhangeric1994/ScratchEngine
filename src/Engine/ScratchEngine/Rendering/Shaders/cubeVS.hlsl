struct VertexShaderInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
	float3 direction  : DIRECTION;
};

cbuffer CameraData : register(b2)
{
	matrix view;
	matrix projection;
};


VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;
	
	matrix viewWithoutT = view;
	viewWithoutT._41 = 0;
	viewWithoutT._42 = 0;
	viewWithoutT._43 = 0;

	matrix VP = mul(viewWithoutT, projection);
	output.svPosition = mul(float4(input.position.xyz, 1.0f), VP);

	output.svPosition.z = output.svPosition.w;

    output.direction = input.position.xyz;

	return output;
}