cbuffer cbuf
{
    float3 lightpos;
    float3 materialColor;
    float3 ambient ;
    float3 diffuseColor ;
    float diffuse_intensity;
    float attConst;
    float attLin;
    float attQuad;
};

float4 main( float3 worldpos : Position, float3 n : Normal ) : SV_TARGET
{
    const float3 vToL = lightpos - worldpos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    float atten = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    float3 d = diffuseColor * diffuse_intensity * atten * max(0.0f, dot(dirToL,n));
    
    return float4(saturate( d + ambient) * materialColor ,1.0f);
}