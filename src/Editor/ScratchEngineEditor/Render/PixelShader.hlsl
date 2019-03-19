
struct DirectionalLight {
	float4   AmbientColor;
	float4   DiffuseColor;
	float3   Direction;
};

struct PointLight {
	float4	 AmbientColor;
	float4	 DiffuseColor;
	float3	 Position;
};

struct SpotLight {
	float4	 AmbientColor;
	float4	 DiffuseColor;
	float3	 Position;
	float	 Cone;
	float3	 Direction;
	float	 Range;
};

//cbuffer colorData : register(b0) {
//	DirectionalLight light;
//};
//
//cbuffer colorData2 : register(b1) {
//	DirectionalLight light2;
//}
//
//cbuffer pointLightData : register(b2) {
//	PointLight pointLight;
//}

cbuffer spotLightData : register(b0) {
	SpotLight spotLight;
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
	float3 nDirection = normalize(position - pointLight.Position);
	float NdotL = saturate(dot(normal, nDirection));
	float4 finalColor = mul(NdotL, pointLight.DiffuseColor) + pointLight.AmbientColor;
	return finalColor;
}

float4 calculateSpotLight(float3 normal, float3 position, SpotLight spotLight) {
	float3 lightDir = position - spotLight.Position;
	float  d = length(lightDir);

	/*if (d > spotLight.Range)
		return spotLight.DiffuseColor * spotLight.AmbientColor;*/

	lightDir = normalize(lightDir);

	float NdotL = dot(normal, lightDir);

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (NdotL > 0.0f) {
		float4 finalColor = mul(NdotL, spotLight.DiffuseColor) + spotLight.AmbientColor;
		finalColor *= pow(max(dot(-lightDir, spotLight.Direction), 0.0f), spotLight.Cone);
	}

	float test = pow(max(dot(-lightDir, spotLight.Direction), 0.0f), spotLight.Cone);

	finalColor = saturate(finalColor);

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
float4 main(VertexToPixel input) : SV_TARGET{
	//float4 lightColor1 = calculateDirectionalLight(input.normal, light);
	//float4 lightColor2 = calculateDirectionalLight(input.normal, light2);

	//float4 pointLightColor = calculatePointLight(input.normal, input.worldPos, pointLight);

	float4 spotLightColor = calculateSpotLight(input.normal, input.worldPos, spotLight);

	//float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	//return surfaceColor * lightColor1 + surfaceColor * lightColor2;
	//return lightColor1;
	//return pointLightColor;
	//return spotLight.DiffuseColor;
	//return pointLight.AmbientColor;
	return spotLightColor;
}