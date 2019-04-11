struct VertexToPixel {
	float4 svPosition : SV_POSITION;
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;
	float4 shadowPos : SHADOW;
};

//PBR functions


//End of PBR functions

float4 main(VertexToPixel input) : SV_TARGET {
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}