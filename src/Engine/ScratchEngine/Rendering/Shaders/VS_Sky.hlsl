struct VertexShaderInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 viewRay : TEXCOORD0;
};


cbuffer CameraData : register(b2)
{
	matrix view;
	matrix projection;
};


VertexToPixel main(VertexShaderInput input)
{
	matrix viewWithoutT = view;
	viewWithoutT._41 = 0;
	viewWithoutT._42 = 0;
	viewWithoutT._43 = 0;

	matrix VP = mul(viewWithoutT, projection);


    VertexToPixel output;
	

	output.position = mul(float4(input.position.xyz, 1.0f), VP);
	output.position.z = output.position.z;
	
    output.viewRay = input.position.xyz;


	return output;
}