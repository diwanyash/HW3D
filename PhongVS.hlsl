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

float4 main( float3 pos : POSITION, float3 n : Normal ) : SV_POSITION
{
    VSOut vsout;
    vsout.worldpos = (float3)mul(float4(pos, 1.0f), model);
    vsout.normal = mul(n, (float3x3)model);
    vsout.pos = mul((float4)(pos,1.0f),modelViewProj);
    return vsout;
}