Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 main(float4 Pos : SV_POSITION, float2 texCoords : TEXCOORD0) : SV_TARGET
{
    return tex.Sample(sam, texCoords);
}
