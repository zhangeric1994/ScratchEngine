struct VertexToPixel
{
    float4 svPosition : SV_POSITION;
};


float4 main(VertexToPixel input) : SV_TARGET
{
    return float4(input.svPosition.zzz, 1.0f);
}