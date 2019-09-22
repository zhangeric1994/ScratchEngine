#include "Lighting.hlsli"


struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float3 viewRay	: TEXCOORD0;
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


float4 main(VertexToPixel input) : SV_TARGET
{
    float3 pixelIndex = float3(input.position.xy, 0);

	// Load pixels from G-buffer (faster than sampling)
    float3 normal = gBufferNormal.Load(pixelIndex).rgb;
    float depth = gBufferDepth.Load(pixelIndex).r;
    float3 metalRoughSpec = gBufferMaterial.Load(pixelIndex).rgb;
	

	// Unpack normal
	normal = normal * 2 - 1;


	// Calc world position using depth and view ray
	input.viewRay = normalize(input.viewRay);
	float3 worldPos = cameraPosition + input.viewRay * depth;
	

	// Handle lighting calculation (assuming albedo of {1,1,1} here)
	float shininess = metalRoughSpec.b * 1024.0f; // Back to the 0-1024 range
	float roughness = metalRoughSpec.g;
	float3 color = DirLight(light, normal, worldPos, cameraPosition, shininess, roughness, float3(1, 1, 1));
	
    
    return float4(color, 1);
}