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

cbuffer ObjectBuff
{
    float specular_intensity;
    float specular_power;
    float padding[2];
};

float4 main(float3 worldpos : Position, float3 n : Normal, float3 color : Color ) : SV_Target
{
    const float3 vToL = lightpos - worldpos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    const float atten = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    const float3 d = diffuseColor * diffuse_intensity * atten * max(0.0f, dot(dirToL, n));
    
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    
    const float3 spec = atten * (diffuseColor * diffuse_intensity) * specular_intensity * pow(max(0.0f, dot(normalize(-r), normalize(worldpos)) ), specular_power);
    
    return float4(saturate((d + ambient + spec) * color), 1.0f);
}