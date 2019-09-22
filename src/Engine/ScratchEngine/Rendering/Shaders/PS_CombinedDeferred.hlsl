// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


cbuffer externalData : register(b0)
{
    int GammaCorrection;
};


// Textures and such
Texture2D lightBuffer : register(t0);


// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 pixelIndex = float3(input.position.xy, 0);

    float3 color = lightBuffer.Load(pixelIndex).rgb;


	// Gamma correction for final screen display?
    return float4(lerp(color, pow(color, 1.0f / 2.2f), GammaCorrection), 1);
}