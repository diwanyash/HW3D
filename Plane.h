#pragma once

#include "IndexedTriangleList.h"
#include "DirectXMath.h"
#include <array>

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> GetPlain(int division = 7, float size = 1.0f)
	{
		namespace dx = DirectX;

		const int nVerticesSize = division + 1;

		std::vector<V> vertices(sq(nVerticesSize));

		{
			const float side = size / 2.0f;
			const float divisionSize = size / float(division);
			auto bottomLeft = dx::XMVectorSet( -side, -side, 0.0f, 0.0f );

			for (int y = 0, i = 0; y < nVerticesSize; y++)
			{
				const float y_pos = float(y) * divisionSize;
				for (int x = 0; x < nVerticesSize; x++, i++)
				{
					const auto v = dx::XMVectorAdd(
						bottomLeft,
						dx::XMVectorSet(float(x) * divisionSize, y_pos, 0.0f, 0.0f));
					
					dx::XMStoreFloat3(&vertices[i].pos, v);
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(sq(division) * 6);
		{
			const auto vxy2i = [nVerticesSize](unsigned short x, unsigned short y)
			{
				return y * nVerticesSize + x;
			};
			for (unsigned short y = 0; y < division; y++)
			{
				for (unsigned short x = 0; x < division; x++)
				{
					const std::array<unsigned short , 4> IndexArray =
					{ (unsigned short)vxy2i(x,y), (unsigned short)vxy2i(x + 1u,y),
						(unsigned short)vxy2i(x,y + 1u), (unsigned short)vxy2i(x + 1u,y + 1u) };
					indices.push_back(IndexArray[0u]);
					indices.push_back(IndexArray[2u]);
					indices.push_back(IndexArray[1u]);
					indices.push_back(IndexArray[1u]);
					indices.push_back(IndexArray[2u]);
					indices.push_back(IndexArray[3u]);
				}
			}
		}
		return { std::move(vertices), std::move(indices) };
	}


	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return GetPlain<V>( 1, 1.0f );
	}
};