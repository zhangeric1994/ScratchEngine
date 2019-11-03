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
	float3 direction  : DIRECTION;
};

cbuffer CameraData : register(b2)
{
	matrix view;
	matrix projection;
};


VertexToPixel main(VertexShaderInput input)
{
	matrix V = view;
	V._41 = 0;
	V._42 = 0;
	V._43 = 0;


    VertexToPixel output;

    output.position = mul(float4(input.position.xyz, 1.0f), mul(V, projection));
    output.position.z = output.position.w;
    output.direction = input.position.xyz;


	return output;
}