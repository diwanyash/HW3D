#pragma once
#include "IndexedTriangleList.h"
#include "DirectXMath.h"
#include "DeadMath.h"

class Sphere
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated( int latdiv, int longdiv )
	{
		namespace dx = DirectX;

		const auto base = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f );
		const float LattitudeAngle = PI / latdiv;
		const float LongitudeAngle = 2.0f * PI / longdiv;

		std::vector<V> vertices;
		for (int lati = 1; lati < latdiv; lati++)
		{
			const auto latBase = dx::XMVector3Transform(base , dx::XMMatrixRotationX(LattitudeAngle * lati));

			for (int longi = 0; longi < longdiv; longi++)
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(latBase , dx::XMMatrixRotationZ(longi * LongitudeAngle));
				dx::XMStoreFloat3( &vertices.back().pos, v );
			}
		}

		// northpole
		const auto iNorthpole = (unsigned short)vertices.size();
		vertices.emplace_back();
		dx::XMStoreFloat3( &vertices.back().pos, base );
		//vertices.back().pos = base;
		// southpole
		const auto iSouthpole = (unsigned short)vertices.size();
		vertices.emplace_back();
		dx::XMStoreFloat3( &vertices.back().pos, dx::XMVectorNegate(base) );
		//vertices.back().pos = -base;

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
			indices.push_back(iNorthpole);
			indices.push_back(calculateInd(0, ilong));
			indices.push_back(calculateInd(0, ilong + 1));

			// Southpole
			indices.push_back(iSouthpole);
			indices.push_back(calculateInd(latdiv - 2, ilong + 1));
			indices.push_back(calculateInd(latdiv - 2, ilong));
		}
		// north wrap
		indices.push_back(iNorthpole);
		indices.push_back(calculateInd(0, longdiv - 1));
		indices.push_back(calculateInd(0, 0));

		// south wrap
		indices.push_back(calculateInd(latdiv - 2, 0));
		indices.push_back(calculateInd(latdiv - 2, longdiv - 1));
		indices.push_back(iSouthpole);

		return { std::move(vertices), std::move(indices) };
	}



	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>( 12, 24 );
	}

	//template<class V>
	//static IndexedTriangleList<V> GetPlainNormals(float radius = 1.0f, int latdiv = 12, int longdiv = 24)
	//{
	//	auto sphere = GetPlain<V>(radius, latdiv, longdiv);
	//
	//	for (auto& v : sphere.vertices)
	//	{
	//		v.n = v.pos.GetNormalized();
	//	}
	//
	//	return sphere;
	//}
};