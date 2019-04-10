struct VertexToPixel
{
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

float4 Lambert(float4 ambientColor, float4 diffuseColor, float3 N, float3 L)
{
    return ambientColor + saturate(dot(N, L)) * diffuseColor;
}

float4 BlinnPhong(float3 N, float3 L, float3 V, float shininess)
{
    return pow(saturate(dot(-normalize(reflect(L, N)), V)), shininess).xxxx;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	//normal map
	float3 textureNormal = normalMap.Sample(basicSampler, input.uv).rgb * 2 - 1;

	float3 N = input.normal;
	float3 T = input.tangent;
	float3 B = cross(T, N);

	float3x3 TBN = float3x3(T, B, N);

	input.normal = normalize(mul(textureNormal, TBN));

	//param for light calculation
    N = normalize(input.normal);
    float3 L = -normalize(light.direction);
    float3 V = normalize(cameraPosition.xyz - input.position.xyz);

	//texture color
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	//shadow map
	float2 shadowUV = input.shadowPos.xy / input.shadowPos.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;
	float depthFromLight = input.shadowPos.z / input.shadowPos.w;
	float shadowAmount = ShadowMap.SampleCmpLevelZero(shadowSampler, shadowUV, depthFromLight);

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return surfaceColor * (Lambert(light.ambientColor, light.diffuseColor, N, L) + BlinnPhong(N, L, V, 16)) * shadowAmount;
}