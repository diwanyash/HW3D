#pragma once
#include "IndexedTriangleList.h"
#include "DirectXMath.h"
#include "DeadMath.h"

class Prism
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int div , float size = 2.0f)
	{
		namespace dx = DirectX;

		assert(div >= 3);

		const float side = size / 2.0f;
		const float radius = side;
		const auto apexbase = dx::XMVectorSet(0.0f, 0.0f, size, 0.0f);
		const auto apexVecOfRot = dx::XMVectorSet(0.0f, radius, 0.0f, 0.0f); 
		const auto apexop = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); 
		const float LongitudeAngle = 2.0f * PI / div;

		std::vector<V> vertices;

		//vertices.emplace_back();
		//dx::XMStoreFloat3( &vertices.back().pos, apexbase );
		vertices.reserve( div * 2 );

		for (int rot = 0; rot < div; rot++)
		{
			const auto latBase = dx::XMVector3Transform(
				dx::XMVectorAdd(apexbase, apexVecOfRot), dx::XMMatrixRotationZ( (-LongitudeAngle) * rot));

			vertices.emplace_back();
			//auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longi * LongitudeAngle));
			dx::XMStoreFloat3(&vertices.back().pos, latBase);
		}
		for (int rot = 0; rot < div; rot++)
		{
			const auto latBase = dx::XMVector3Transform(
				dx::XMVectorAdd(apexop, apexVecOfRot), dx::XMMatrixRotationZ( (-LongitudeAngle) * rot ));

			vertices.emplace_back();
			//auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longi * LongitudeAngle));
			dx::XMStoreFloat3(&vertices.back().pos, latBase);
		}



		// northpole
		const auto iFarpole = (unsigned short)vertices.size();
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, apexbase);
		//vertices.back().pos = base;
		// southpole
		const auto iNearpole = (unsigned short)vertices.size();
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, apexop);
		//vertices.back().pos = -base;


		std::vector<unsigned short> indices;

		indices.reserve( div * 4 );
		const int nWalls = div * 2;

		// walls
		for ( unsigned short i = 0; i < div - 1; i++ )
		{
			indices.push_back( i );
			indices.push_back( i + div + 1);
			indices.push_back( i + div );

			indices.push_back( i );
			indices.push_back( i + 1 );
			indices.push_back( i + div + 1);
		}

		// last walls
		indices.push_back( div - 1); //2
		indices.push_back(div); // 3
		indices.push_back((div * 2) - 1); // 5

		indices.push_back( div - 1);
		indices.push_back( 0 );
		indices.push_back( div );

		// NEED TO DO NEAR AND FAR///////////////////////////////////////


		// Far base

		for (unsigned short i = 0; i < div - 1; i++)
		{
			indices.push_back( i + 1 );
			indices.push_back( i );
			indices.push_back( iFarpole );
		}


		for (unsigned short i = div; i < (div * 2) - 1; i++)
		{
			indices.push_back(iNearpole);
			indices.push_back( i );
			indices.push_back( i + 1 );
		}

		// last triangle for farside
		indices.push_back( 0 );
		indices.push_back( div - 1 );
		indices.push_back(iFarpole);

		// last triangle for nearside
		indices.push_back(iNearpole);
		indices.push_back( (div * 2) - 1 );
		indices.push_back( div ); 

		return { std::move(vertices), std::move(indices) };
	}



	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>( 3 );
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