// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};


cbuffer CameraData : register(b2)
{
    matrix view;
    matrix projection;
    matrix viewProjection;
    matrix inverseViewProjection;
    float3 cameraPosition;
};

cbuffer ObjectData : register(b3)
{
    matrix world;
};


// The entry point for our vertex shader
float4 main(VertexShaderInput input) : SV_POSITION
{
    return mul(float4(input.position.xyz, 1), mul(mul(world, view), projection));
}