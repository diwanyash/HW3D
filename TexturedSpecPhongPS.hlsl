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

Texture2D tex;
Texture2D spec;

SamplerState splr;

float4 main(float3 worldpos : Position, float3 n : Normal, float2 tc : TexCoord) : SV_TARGET
{
    const float3 vToL = lightpos - worldpos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    const float atten = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    const float3 d = diffuseColor * diffuse_intensity * atten * max(0.0f, dot(dirToL, n));
          
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularReflactionColor = specularSample.rgb;
    const float specularPower = specularSample.a * 100.0f;
    
    const float3 spec = atten * (diffuseColor * diffuse_intensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldpos))), specularPower);
    
    return float4(saturate((d + ambient) * tex.Sample(splr, tc).rgb + spec * specularReflactionColor), 1.0f);
}