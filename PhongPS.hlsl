cbuffer cbuf
{
    float3 lightpos;
};

static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
static const float3 ambient = { 0.05f, 0.05f, 0.05f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuse_intensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin= 0.045f;
static const float attQuad = 0.0075f;

float4 main( float3 worldpos : Position, float3 n : Normal ) : SV_TARGET
{
    const float3 vToL = lightpos - worldpos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    float atten = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    float3 d = diffuseColor * diffuse_intensity * atten * max(0.0f, dot(dirToL,n));
    
    return float4(saturate( d + ambient) * materialColor ,1.0f);
}