cbuffer cbuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 campos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord)
{
    VSOut vsout;
    vsout.campos = (float3) mul(float4(pos, 1.0f), modelView);
    vsout.normal = mul(n, (float3x3) modelView);
    vsout.tan = mul(tan, (float3x3) modelView);
    vsout.bitan = mul(bitan, (float3x3) modelView);
    vsout.pos = mul(float4(pos, 1.0f), modelViewProj);
    vsout.tc = tc;
    return vsout;
}