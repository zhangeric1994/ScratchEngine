
struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

struct LightSource
{
	float4 ambientColor;
	float4 aiffuseColor;
	int type;
	float3 position;
	float3 direction;
};

struct PointLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Position;
};

cbuffer colorData : register(b0)
{
	DirectionalLight light;
};

cbuffer colorData2 : register(b1)
{
	DirectionalLight light2;
}

cbuffer pointLightData : register(b2)
{
	PointLight pointLight;
}

//Texture2D diffuseTexture : register(t0);
//
//SamplerState basicSampler : register(s0);

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};

float4 calculateDirectionalLight(float3 normal, DirectionalLight light) {
	float3 nDirection = -normalize(light.Direction);
	float  NdotL = saturate(dot(normal, nDirection));
	float4 finalColor = mul(NdotL, light.DiffuseColor) + light.AmbientColor;
	return finalColor;
}

float4 calculatePointLight(float3 normal,float3 position, PointLight pointLight) {
	float3 nDirection = -normalize(position - pointLight.Position);
	float NdotL = saturate(dot(normal, nDirection));
	float4 finalColor = mul(NdotL, pointLight.DiffuseColor) + pointLight.AmbientColor;
	return finalColor;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 lightColor1 = calculateDirectionalLight(input.normal, light);
	//float4 lightColor2 = calculateDirectionalLight(input.normal, light2);

	float4 pointLightColor = calculatePointLight(input.normal, input.worldPos, pointLight);

	//float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	//return surfaceColor * lightColor1 + surfaceColor * lightColor2;
	return pointLightColor;
}