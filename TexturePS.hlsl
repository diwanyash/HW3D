Texture2D tex;

SamplerState splr;

float4 main( float2 tc : TexCoord ) : SV_Target
{
    float4 rs = tex.Sample( splr, tc );
    
    return rs;
}