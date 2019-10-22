#include "Lighting.hlsli"


struct VertexToPixel
{
	float4 svPosition : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;
};


cbuffer FrameData : register(b0)
{
    int GammaCorrection;
}

cbuffer LightSourceData : register(b1)
{
	LightSource lights[128];
    int numLights;
};

cbuffer CameraData : register(b2)
{
	float3 cameraPosition;
};

cbuffer MaterialData : register(b3)
{
    float4 tint;
    int hasTexture;
    int hasNormalMap;
    int hasMetalnessMap;
    int hasRoughnessMap;
}


Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D roughnessMap : register(t2);
Texture2D metalnessMap : register(t3);
Texture2D shadowMap : register(t10);


SamplerState basicSampler : register(s0);
SamplerComparisonState shadowSampler : register(s10);


float4 main(VertexToPixel input) : SV_TARGET
{	
    // Fix for poor normals: re-normalizing interpolated normals
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);

	// Use normal mapping
    input.normal = lerp(input.normal, NormalMapping(normalMap, basicSampler, input.uv, input.normal, input.tangent), hasNormalMap);


	// Sample the roughness map
    float roughness = lerp(0.0f, roughnessMap.Sample(basicSampler, input.uv).r, hasRoughnessMap);


	// Sample the metal map
    float metal = lerp(0.0f, metalnessMap.Sample(basicSampler, input.uv).r, hasMetalnessMap);


	// Sample texture
    float4 surfaceColor = diffuseMap.Sample(basicSampler, input.uv);
    surfaceColor.rgb = lerp(surfaceColor.rgb, pow(surfaceColor.rgb, 2.2), GammaCorrection);

	// Actually using texture?
    surfaceColor.rgb = lerp(tint.rgb, surfaceColor.rgb, hasTexture);


	// Specular color - Assuming albedo texture is actually holding specular color if metal == 1
    float3 specColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metal);


	// Total color for this pixel
    float3 totalColor = float3(0, 0, 0);


	// Loop through all lights this frame
    for (int i = 0; i < numLights; i++)
    {
        float4 shadowPosition = mul(float4(input.worldPosition.xyz, 1), lights[i].shadowViewProjection[0]);
        float2 shadowUV = shadowPosition.xy / shadowPosition.w * 0.5f + 0.5f;
        shadowUV.y = 1.0f - shadowUV.y;
        float depthFromLight = shadowPosition.z / shadowPosition.w;
        float shadowAmount = lerp(1.0f, shadowMap.SampleCmpLevelZero(shadowSampler, shadowUV, depthFromLight), 0);


		// Which kind of light?
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalColor += DirectionalLightPBR(lights[i], input.normal, input.worldPosition.xyz, cameraPosition, roughness, metal, surfaceColor.rgb, specColor) * shadowAmount;
                break;


            case LIGHT_TYPE_POINT:
                totalColor += PointLightPBR(lights[i], input.normal, input.worldPosition.xyz, cameraPosition, roughness, metal, surfaceColor.rgb, specColor) * shadowAmount;
                break;


            case LIGHT_TYPE_SPOT:
                totalColor += SpotLightPBR(lights[i], input.normal, input.worldPosition.xyz, cameraPosition, roughness, metal, surfaceColor.rgb, specColor) * shadowAmount;
                break;
        }
    }


    return float4(lerp(totalColor, pow(totalColor, 1.0 / 2.2), GammaCorrection), 1);
}