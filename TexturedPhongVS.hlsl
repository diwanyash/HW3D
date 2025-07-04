cbuffer cbuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 campos : Position;
    float3 normal : Normal;
    float2 tc : TexCoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : TexCoord)
{
    VSOut vsout;
    vsout.campos = (float3) mul(float4(pos, 1.0f), modelView);
    vsout.normal = mul(n, (float3x3) modelView);
    vsout.pos = mul(float4(pos, 1.0f), modelViewProj);
    vsout.tc = tc;
    return vsout;
}