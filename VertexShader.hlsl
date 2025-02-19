struct VOut
{
    float4 Pos : SV_POSITION;
    float2 texCoords : TEXCOORDS;
};

VOut main(float4 Pos : POS, float2 texCoords : TEXCOORDS)
{
    VOut output;

    output.Pos = Pos;
    output.texCoords = texCoords;

    return output;
}