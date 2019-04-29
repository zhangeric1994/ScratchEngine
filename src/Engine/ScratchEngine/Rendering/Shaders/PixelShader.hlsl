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


cbuffer ShaderData : register(b0)
{
    float4 tint;
}

cbuffer LightSourceData : register(b1)
{
	LightSource light;
};

cbuffer CameraData : register(b2)
{
    float3 cameraPosition;
};


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
    float4 albedo = tint;

    float3 N = normalize(input.normal);
    float3 L = -normalize(light.direction);
    float3 V = normalize(cameraPosition.xyz - input.position.xyz);

    return albedo * Lambert(light.ambientColor, light.diffuseColor, N, L) + BlinnPhong(N, L, V, 16);
}