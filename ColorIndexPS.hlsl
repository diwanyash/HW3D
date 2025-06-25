

cbuffer cb2
{
    float4 face_color[9];
};

float4 main(uint tid : SV_PrimitiveID) : SV_TARGET
{
	return face_color[(tid/2) % 6 ];
}