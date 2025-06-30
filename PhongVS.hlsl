cbuffer cbuf
{
    matrix model;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldpos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position, float3 n : Normal )
{
    VSOut vsout;
    vsout.worldpos = (float3)mul(float4(pos, 1.0f), model);
    vsout.normal = mul(n, (float3x3)model);
    vsout.pos = mul(float4(pos,1.0f),modelViewProj);
    return vsout;
}