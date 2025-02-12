struct Vout
{
    float4 color : Color;
    float4 pos : SV_POSITION;
};

Vout main( float2 pos : Position, float4 color : Color )
{
    Vout vso;
    vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
    vso.color = color;
    return vso;
}