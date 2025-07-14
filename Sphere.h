#pragma once
#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include "DirectXMath.h"
#include "DeadMath.h"

class Sphere
{
public:
	static IndexedTriangleList MakeTesselated( Dvtx::VertexLayout layout ,int latdiv, int longdiv )
	{
		namespace dx = DirectX;
		assert( latdiv >= 3 );

		const auto base = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f );
		const float LattitudeAngle = PI / latdiv;
		const float LongitudeAngle = 2.0f * PI / longdiv;

		Dvtx::VertexBuffer vb{ std::move(layout) };
		for (int lati = 1; lati < latdiv; lati++)
		{
			const auto latBase = dx::XMVector3Transform(base , dx::XMMatrixRotationX(LattitudeAngle * lati));

			for (int longi = 0; longi < longdiv; longi++)
			{
				dx::XMFLOAT3 calculatedpos;
				auto v = dx::XMVector3Transform(latBase , dx::XMMatrixRotationZ(longi * LongitudeAngle));
				dx::XMStoreFloat3( &calculatedpos, v );
				vb.EmplaceBack( calculatedpos );
			}
		}
		const auto iNorthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 northPos;
			dx::XMStoreFloat3(&northPos, base);
			vb.EmplaceBack(northPos);
		}
		const auto iSouthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 southPos;
			dx::XMStoreFloat3(&southPos, dx::XMVectorNegate(base));
			vb.EmplaceBack(southPos);
		}

		// lamda func
		const auto calculateInd = [latdiv, longdiv](unsigned short inlat, unsigned short inlong)
		{ return inlat * longdiv + inlong; };

		std::vector<unsigned short> indices;
		for (unsigned short inlat = 0; inlat < latdiv - 2; inlat++)
		{
			for (unsigned short inlong = 0; inlong < longdiv - 1; inlong++)
			{
				indices.push_back(calculateInd(inlat, inlong));
				indices.push_back(calculateInd(inlat + 1, inlong));
				indices.push_back(calculateInd(inlat, inlong + 1));
				indices.push_back(calculateInd(inlat, inlong + 1));
				indices.push_back(calculateInd(inlat + 1, inlong));
				indices.push_back(calculateInd(inlat + 1, inlong + 1));
			}

			indices.push_back(calculateInd(inlat, longdiv - 1));
			indices.push_back(calculateInd(inlat + 1, longdiv - 1));
			indices.push_back(calculateInd(inlat, 0));
			////////////////////////////////////////////////
			indices.push_back(calculateInd(inlat, 0));
			indices.push_back(calculateInd(inlat + 1, longdiv - 1));
			indices.push_back(calculateInd(inlat + 1, 0));

		}

		for (unsigned short ilong = 0; ilong < longdiv - 1; ilong++)
		{
			// Northpole
			indices.push_back(iNorthPole);
			indices.push_back(calculateInd(0, ilong));
			indices.push_back(calculateInd(0, ilong + 1));

			// Southpole
			indices.push_back(iSouthPole);
			indices.push_back(calculateInd(latdiv - 2, ilong + 1));
			indices.push_back(calculateInd(latdiv - 2, ilong));
		}
		// north wrap
		indices.push_back(iNorthPole);
		indices.push_back(calculateInd(0, longdiv - 1));
		indices.push_back(calculateInd(0, 0));

		// south wrap
		indices.push_back(calculateInd(latdiv - 2, 0));
		indices.push_back(calculateInd(latdiv - 2, longdiv - 1));
		indices.push_back(iSouthPole);

		return { std::move( vb ), std::move( indices ) };
	}



	static IndexedTriangleList Make( std::optional<Dvtx::VertexLayout> layout = std::nullopt )
	{
		using Element = Dvtx::VertexLayout::ElementType;
		if ( !layout )
		{
			layout = Dvtx::VertexLayout{}.Append(Element::Position3D);
		}
		return MakeTesselated( std::move( *layout ), 12, 24 );
	}
};