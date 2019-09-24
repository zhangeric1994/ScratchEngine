#include "Lighting.hlsli"


struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
    float4 shadowPos : SHADOW;
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
}


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
    // Fix for poor normals: re-normalizing interpolated normals
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    

	// Total color for this pixel
    float3 totalColor = float3(0, 0, 0);

	// Loop through all lights this frame
    for (int i = 0; i < numLights; i++)
    {
		// Which kind of light?
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalColor += DirLight(lights[i], input.normal, input.position.xyz, cameraPosition, 16, 0, float3(1, 1, 1));
                break;

            case LIGHT_TYPE_POINT:
                totalColor += PointLight(lights[i], input.normal, input.position.xyz, cameraPosition, 16, 0, float3(1, 1, 1));
                break;

            case LIGHT_TYPE_SPOT:
                totalColor += SpotLight(lights[i], input.normal, input.position.xyz, cameraPosition, 16, 0, float3(1, 1, 1));
                break;
        }
    }


	// Tint the surface color by the light
    totalColor *= tint.rgb;


	// Adjust the light color by the light amount
    return float4(lerp(totalColor, pow(totalColor, 1.0f / 2.2f), GammaCorrection), 1);
}