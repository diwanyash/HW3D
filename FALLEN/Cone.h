#pragma once
#include "IndexedTriangleList.h"
#include "DirectXMath.h"
#include "DeadMath.h"

class Cone
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated( int longdiv)
	{
		namespace dx = DirectX;

		assert( longdiv >= 3 );

		const float radius = 1.0f;
		const auto apexbase = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const auto apexop = dx::XMVectorSet(0.0f, radius, 0.0f, 0.0f);
		const auto apexcenter = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		const float LongitudeAngle = 2.0f * PI / longdiv;

		std::vector<V> vertices;

		//vertices.emplace_back();
		//dx::XMStoreFloat3( &vertices.back().pos, apexbase );


		for (int longi = 0; longi < longdiv; longi++)
		{
			const auto latBase = dx::XMVector3Transform(apexop, dx::XMMatrixRotationZ(LongitudeAngle * longi)); 

				vertices.emplace_back();
				//auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longi * LongitudeAngle));
				dx::XMStoreFloat3(&vertices.back().pos, latBase);
		}
		


		// northpole
		const auto iNorthpole = (unsigned short)vertices.size();
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, apexbase);
		//vertices.back().pos = base;
		// southpole
		const auto iSouthpole = (unsigned short)vertices.size();
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, apexcenter); 
		//vertices.back().pos = -base;


		std::vector<unsigned short> indices;

		// walls
		for ( unsigned short i = 0; i < longdiv - 1; i++ )
		{
			indices.push_back( iNorthpole );
			indices.push_back( i );
			indices.push_back( i + 1 );
		}

		// last triangle wall
		indices.push_back( iNorthpole );
		indices.push_back(longdiv - 1);
		indices.push_back( 0 );


		// bottom
		for ( unsigned short i = 0; i < longdiv - 1; i++ )
		{
			indices.push_back( iSouthpole );
			indices.push_back( i + 1 );
			indices.push_back( i );
		}

		// last triangle bottom
		indices.push_back(iSouthpole);
		indices.push_back(0);
		indices.push_back(longdiv - 1);

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> MakeTesselatedIndependentFaces(int longDiv)
	{
		namespace dx = DirectX;
		assert(longDiv >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;

		// cone vertices
		const auto iCone = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			const float thetas[] = {
				longitudeAngle * iLong,
				longitudeAngle * (((iLong + 1) == longDiv) ? 0 : (iLong + 1))
			};
			vertices.emplace_back();
			vertices.back().pos = { 0.0f,0.0f,1.0f };
			for (auto theta : thetas)
			{
				vertices.emplace_back();
				const auto v = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(theta)
				);
				dx::XMStoreFloat3(&vertices.back().pos, v);
			}
		}
		// base vertices
		const auto iBaseCenter = (unsigned short)vertices.size();
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iBaseEdge = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			dx::XMStoreFloat3(&vertices.back().pos, v);
		}

		std::vector<unsigned short> indices;

		// cone indices
		for (unsigned short i = 0; i < longDiv * 3; i++)
		{
			indices.push_back(i + iCone);
		}
		// base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iBaseCenter);
			indices.push_back((iLong + 1) % longDiv + iBaseEdge);
			indices.push_back(iLong + iBaseEdge);
		}

		return { std::move(vertices),std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>( 4 );
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