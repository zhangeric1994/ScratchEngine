
struct DirectionalLight {
	float3   AmbientColor;
	float	 CameraX;
	float3   DiffuseColor;
	float	 CameraY;
	float3   Direction;
	float	 CameraZ;
};

struct PointLight {
	float3   AmbientColor;
	float	 CameraX;
	float3   DiffuseColor;
	float	 CameraY;
	float3   Position;
	float	 CameraZ;
};

struct SpotLight {
	float4	 AmbientColor;
	float4	 DiffuseColor;
	float3	 Position;
	float	 Cone;
	float3	 Direction;
	float	 Range;
};

cbuffer colorData : register(b0) {
	DirectionalLight light;
};

cbuffer colorData2 : register(b1) {
	DirectionalLight light1;
}

cbuffer pointLightData : register(b2) {
	PointLight pointLight;
}

cbuffer spotLightData : register(b3) {
	SpotLight spotLight;
}

Texture2D diffuseTexture : register(t0);
Texture2D normalMap : register(t1);

SamplerState basicSampler : register(s0);

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
	float3 tangent		: TANGENT;
	float2 uv			: TEXCOORD;
};

float3 calculateDirectionalLight(float3 normal, float3 position, DirectionalLight light) {
	//diffuse part
	float3 nDirection = -normalize(light.Direction);
	float  NdotL = saturate(dot(normal, nDirection));
	NdotL = saturate(NdotL);

	//specular part
	float3 CameraPos = float3(light.CameraX, light.CameraY, light.CameraZ);
	float3 dirToCamera = normalize(CameraPos - position);
	float3 reflectance = reflect(-nDirection, normal);
	float  spec = pow(saturate(dot(reflectance, dirToCamera)), 32.0f);

	float3 finalColor =  NdotL * (light.DiffuseColor + spec.rrr);
	return finalColor;
}

float3 calculatePointLight(float3 normal,float3 position, PointLight pointLight) {
	//diffuse part
	float3 nDirection = -normalize(position - pointLight.Position);
	float NdotL = saturate(dot(normal, nDirection));
	NdotL = saturate(NdotL);

	//spec part
	float3 CameraPos = float3(light.CameraX, light.CameraY, light.CameraZ);
	float3 dirToCamera = normalize(CameraPos - position);
	float3 reflectance = reflect(-nDirection, normal);
	float  spec = pow(saturate(dot(reflectance, dirToCamera)), 32.0f);

	float3 finalColor = pointLight.DiffuseColor * NdotL + spec.rrr;
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
	//normalize normal and tangent
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	//normal map
	float3 textureNormal = normalMap.Sample(basicSampler, input.uv).rgb * 2 - 1;

	float3 N = input.normal;
	float3 T = input.tangent;
	float3 B = cross(T, N);

	float3x3 TBN = float3x3(T, B, N);

	input.normal = normalize(mul(textureNormal, TBN));

	//light color calculation
	float3 lightColor = calculateDirectionalLight(input.normal, input.worldPos, light);
	float3 lightColor1 = calculateDirectionalLight(input.normal, input.worldPos, light1);

	float3 pointLightColor = calculatePointLight(input.normal, input.worldPos, pointLight);

	float4 spotLightColor = calculateSpotLight(input.normal, input.worldPos, spotLight);

	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return float4(surfaceColor.rgb * lightColor, 1.0f);
	//return float4(input.tangent, 1.0f);
	//return float4(surfaceColor.rgb, 1.0f);
	return float4(surfaceColor.rgb * (lightColor1 + pointLightColor + lightColor), 1.0f);
}