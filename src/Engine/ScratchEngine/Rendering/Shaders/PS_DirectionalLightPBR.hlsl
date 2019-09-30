#include "Lighting.hlsli"


struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 viewRay : TEXCOORD0;
};


// Data that only changes once per frame
cbuffer FrameData : register(b0)
{
	// Runtime options
    int GammaCorrection;
};

cbuffer CameraData : register(b2)
{
    float3 cameraPosition;
};

// Data that can change per light
cbuffer LightData : register(b3)
{
    LightSource light;
};


// Textures and such
Texture2D gBufferAlbedo : register(t0);
Texture2D gBufferNormal : register(t1);
Texture2D gBufferDepth : register(t2);
Texture2D gBufferMaterial : register(t3);
Texture2D shadowMap : register(t10);


SamplerComparisonState shadowSampler : register(s10);


float4 main(VertexToPixel input) : SV_TARGET
{
    float3 pixelIndex = float3(input.position.xy, 0);

	// Load pixels from G-buffer (faster than sampling)
    float3 albedo = gBufferAlbedo.Load(pixelIndex).rgb; // Already gamma correct
    float3 normal = gBufferNormal.Load(pixelIndex).rgb;
    float4 worldPosition = gBufferDepth.Load(pixelIndex);
    float3 metalRoughSpec = gBufferMaterial.Load(pixelIndex).rgb;
	

	// Unpack normal
	normal = normal * 2 - 1;


	// Calculate world position using depth and view ray
	//input.viewRay = normalize(input.viewRay);
	//float3 worldPosition = cameraPosition + input.viewRay * depth;
	

	// Handle lighting calculation (using regular albedo here due to energy conservation calculation inside DirLightPBR(),
	// so the deferred light buffer will already have the albedo taken into account - no need for a combine later)
	float metalness = metalRoughSpec.r; 
	float roughness = metalRoughSpec.g;
    float3 specularColor = lerp(F0_NON_METAL.rrr, albedo.rgb, metalness); // If the surface is metal, we're assuming albedo.rgb is actually the specular color!
    float3 color = DirectionalLightPBR(light, normal, worldPosition.xyz, cameraPosition, roughness, metalness, albedo, specularColor);


    float4 shadowPosition = mul(worldPosition, light.shadowViewProjection);
    float2 shadowUV = shadowPosition.xy / shadowPosition.w * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;
    float depthFromLight = shadowPosition.z / shadowPosition.w;
    float shadowAmount = shadowMap.SampleCmpLevelZero(shadowSampler, shadowUV, depthFromLight);
    shadowAmount = lerp(1.0f, shadowAmount, saturate(light.hasShadow));


    return float4(color * shadowAmount, 1);
}