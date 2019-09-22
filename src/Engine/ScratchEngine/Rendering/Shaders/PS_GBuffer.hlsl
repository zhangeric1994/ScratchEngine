// Need for some helper functions
#include "Lighting.hlsli"


// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
    float4 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 shadowPosition : SHADOW;
};


struct GBuffer
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float depth : SV_TARGET2;
    float4 material : SV_TARGET3;
};


// Data that only changes once per frame
cbuffer FrameData : register(b0)
{
	// Runtime options
    int GammaCorrection;
};


// Data that can change per material
cbuffer MaterialData : register(b1)
{
	// Surface color
    float4 tint;
    float shininess;
    int hasTexture;
    int hasNormalMap;
    int hasShadowMap;
    int hasMetalnessMap;
    int hasRoughnessMap;
};


cbuffer CameraData : register(b2)
{
    float3 cameraPosition;
};


Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D roughnessMap : register(t3);
Texture2D metalnessMap : register(t4);


SamplerState basicSampler : register(s0);


// Entry point for this pixel shader
GBuffer main(VertexToPixel input)
{
	// Fix for poor normals: re-normalizing interpolated normals
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Use normal mapping
    input.normal = lerp(input.normal, NormalMapping(normalMap, basicSampler, input.uv, input.normal, input.tangent), hasNormalMap);

	// Sample the roughness map
    float roughness = lerp(0.0f, roughnessMap.Sample(basicSampler, input.uv).r, hasRoughnessMap);

	// Sample the metal map
	float metalness = lerp(0.0f, metalnessMap.Sample(basicSampler, input.uv).r, hasMetalnessMap);

	// Sample texture
    float4 albedo = diffuseMap.Sample(basicSampler, input.uv);
    albedo.rgb = lerp(albedo.rgb, pow(albedo.rgb, 2.2), GammaCorrection);
    albedo.rgb = lerp(tint.rgb, albedo.rgb, hasTexture); // Actually using texture?


	// Set up the pixel shader output for the g-buffer
	GBuffer output;
    output.albedo   = albedo;
	output.normal   = float4(input.normal * 0.5f + 0.5f, 0); // Pack into 0-1 range
	output.depth    = length(cameraPosition - input.worldPosition.xyz); // Linear depth
    output.material = float4(metalness, roughness, shininess / 1024.0f, 0); // Pack shininess into 0-1 range


	return output;
}