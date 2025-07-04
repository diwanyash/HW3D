cbuffer LightBuff
{
    float3 lightpos;
    float3 ambient;
    float3 diffuseColor;
    float diffuse_intensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer MaterialBuff
{
    float specular_intensity;
    float specular_power;
};

Texture2D tex;

SamplerState splr;

float4 main(float3 worldpos : Position, float3 n : Normal,float2 tc : TexCoord) : SV_TARGET
{
    const float3 vToL = lightpos - worldpos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    float atten = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    float3 d = diffuseColor * diffuse_intensity * atten * max(0.0f, dot(dirToL, n));
    
    float3 w = n * dot(vToL, n);
    float3 r = w * 2.0f - vToL;
    
    const float3 spec = atten * (diffuseColor * specular_intensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldpos))), specular_power);
    
    return float4(saturate(d + ambient + spec), 1.0f) * tex.Sample(splr, tc);
}