static const float PI = 3.14159265f;

struct VertexToPixel {
	float4 svPosition : SV_POSITION;
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;
	float4 shadowPos : SHADOW;
};

struct LightSource
{
	float4 ambientColor;
	float4 diffuseColor;
	int type;
	float3 position;
	float range;
	float3 direction;
};

cbuffer LightSourceData : register(b1)
{
	LightSource light;
};

cbuffer CameraData : register(b2)
{
	float3 cameraPosition;
};

Texture2D diffuseTexture : register(t0);

Texture2D normalMap : register(t1);

Texture2D ShadowMap	: register(t2);

SamplerState basicSampler : register(s0);

SamplerComparisonState shadowSampler : register(s1);

//PBR functions
float DistributionGGX(float3 n, float3 h, float a) {
	float a2 = a * a;
	float NdotH = max(dot(n, h), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2; 
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k) {
	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float k) {
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);

	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GEometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

float fresnelSchlick(float cosTheta, float3 f0) {
	return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
}

//End of PBR functions

float4 main(VertexToPixel input) : SV_TARGET {
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}