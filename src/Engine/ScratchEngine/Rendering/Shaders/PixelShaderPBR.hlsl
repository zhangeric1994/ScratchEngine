static const float PI = 3.14159265f;

struct VertexToPixel {
	float4 svPosition : SV_POSITION;
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;
	float4 shadowPos : SHADOW;
};

struct LightSource {
	float4 ambientColor;
	float4 diffuseColor;
	int type;
	float3 position;
	float range;
	float3 direction;
	//float intensity;
};

struct material {
	float3 albedo;
	float roughness;
	float metalness;
};

cbuffer EnableMaps : register(b0) {
	int hasTexture;
	int hasNormalMap;
	int hasShadowMap;
	int hasMetalnessMap;
	int hasRoughnessMap;
}

cbuffer LightSourceData : register(b1) {
	LightSource light;
};

cbuffer CameraData : register(b2) {
	float3 cameraPosition;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalMap : register(t1);
Texture2D ShadowMap	: register(t2);
Texture2D roughnessMap : register(t3);
Texture2D metalnessMap : register(t4);

SamplerState basicSampler : register(s0);

SamplerComparisonState shadowSampler : register(s1);

//returns result of the distribution function
//distribution function is GGX
float DistributionGGX(float3 n, float3 h, float a) {
	float a2 = a * a;
	a2 = a2 * a2;
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;

	float nom = a2; 
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}

//returns result of the geometry function
float GeometrySchlickGGX(float NdotV, float k) {
	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float k) {
	float NdotV = saturate(dot(N, V));
	float NdotL = saturate(dot(N, L));

	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

//calculate k parameter in geometry function
float KIBL(float roughness) {
	return pow(roughness + 1, 2) / 8.0f;
}

float3 fresnelSchlick(float cosTheta, float3 f0) {
	return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5);
}

//calculate f0
float3 calculateF0(float3 surfaceColor, float metalness) {
	//float3 f0 = float3(0.04f, 0.04f, 0.04f);
	float3 f0 = lerp(0.04f.rrr, surfaceColor, metalness);
	return f0;
}

//Lambert diffuse function
float diffusePBR(float3 normal, float3 wi) {
	return saturate(dot(normal, wi));
}

//Multiple kd in render equation with diffuse
float3 DiffuseEnergyConserve(float diffuse, float3 spec, float metalness) {
	return diffuse * ((1 - saturate(spec)) * (1 - metalness));
}

//Microfacet BRDF calculation
float3 MicrofacetBRDF(float3 n, float3 wi, float3 wo, float roughness, float metalness, float3 surfaceColor) {
	//halfway vector
	float3 h = normalize(wi + wo);

	//normal dot outgoing(view) direction wo
	float NdotV = saturate(dot(n, wo));
	
	//f0
	float3 f0 = calculateF0(surfaceColor, metalness);

	//k in geometry function
	float k = KIBL(roughness);

	float D = DistributionGGX(n, h, roughness);
	float3 F = fresnelSchlick(NdotV, f0);
	float G = GeometrySmith(n, wo, wi, k);

	return (D * F * G) / (4 * dot(n, wi) * dot(n, wo));
}

//function to calculate directional light pbr
float3 directionalLightPBR(float3 normal, float3 wo, float3 wi, float roughness, float metalness, float3 surfaceColor, LightSource light) {
	//spec color
	float3 specColor = MicrofacetBRDF(normal, wo, wi, roughness, metalness, surfaceColor);

	//diffuse color
	float diffuse = diffusePBR(normal, wi);
	float3 diffuseColor = DiffuseEnergyConserve(diffuse, specColor, metalness);

	return (diffuseColor * surfaceColor + specColor);
}

//End of PBR functions


float4 main(VertexToPixel input) : SV_TARGET {	
	//normalize input normal and tangent
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	//normal map
	//change the normal of each point
	float3 textureNormal = normalMap.Sample(basicSampler, input.uv).rgb * 2 - 1;

	float3 N = input.normal;
	float3 T = input.tangent;
	float3 B = cross(T, N);

	float3x3 TBN = float3x3(T, B, N);

	float3 normal = normalize(mul(textureNormal, TBN));
	input.normal = lerp(input.normal, normal, hasNormalMap);

	//param for light calculation
	normal = input.normal;
	float3 wi = normalize(-light.direction);
	float3 wo = normalize(cameraPosition.xyz - input.position.xyz);

	//texture color
	float3 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
	surfaceColor = lerp(float3(1, 1, 1), surfaceColor, hasTexture);

	//roughness
	float roughness = roughnessMap.Sample(basicSampler, input.uv).r;
	roughness = lerp(0.0f, roughness, hasRoughnessMap);

	//metalness map
	float metalness = metalnessMap.Sample(basicSampler, input.uv).r;
	metalness = lerp(0.0f, metalness, hasMetalnessMap);

	//shadow map
	float2 shadowUV = input.shadowPos.xy / input.shadowPos.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;
	float depthFromLight = input.shadowPos.z / input.shadowPos.w;
	float shadowAmount = ShadowMap.SampleCmpLevelZero(shadowSampler, shadowUV, depthFromLight);

	//shadowAmount = lerp(1.0f, shadowAmount, 1);

	//calculate light
	//float3 result = directionalLightPBR(normal, wo, wi, roughness, metalness, surfaceColor.rgb, light) * shadowAmount;

	//Debug for shadow map
	//float3 result = ShadowMap.Sample(basicSampler, input.uv).rrr;
	float3 result = shadowAmount.rrr;


	return float4(result, 1.0f);
}