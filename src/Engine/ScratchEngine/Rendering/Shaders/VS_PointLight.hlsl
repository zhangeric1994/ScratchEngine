// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

// Defines the output data of our vertex shader
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 viewRay : TEXCOORD0;
};


cbuffer CameraData : register(b2)
{
    matrix view;
    matrix projection;
    matrix viewProjection;
    matrix inverseViewProjection;
    float3 cameraPosition;
};

cbuffer ObjectData : register(b3)
{
    matrix world;
};


// The entry point for our vertex shader
VertexToPixel main(VertexShaderInput input)
{
	// Set up output
	VertexToPixel output;

    output.position = mul(float4(input.position.xyz, 1), mul(mul(world, view), projection));

	// Calculate the view ray from the camera through this vertex
	// which we need to reconstruct world position from depth in pixel shader
    output.viewRay = mul(float4(input.position.xyz, 1), world).xyz - cameraPosition;


	return output;
}