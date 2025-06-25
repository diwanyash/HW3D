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

		std::vector<V> vertices(sq(nVerticesSize + 1));
		{
			const float side = size / 2.0f;
			const float divisionSize = size / float(division);
			auto bottomLeft = dx::XMVectorSet( - side, -side, 0.0f, 0.0f );

			for (int y = 0, i = 0; y < nVerticesSize; y++)
			{
				const float y_pos = float(y) * divisionSize;
				for (int x = 0; x < nVerticesSize; x++, i++)
				{
					dx::XMStoreFloat3(&vertices[i].pos, dx::XMVectorAdd(bottomLeft , dx::XMVectorSet(float(x) * divisionSize, y_pos, 0.0f, 0.0f)));
				}
			}
		}

		std::vector<size_t> indices;
		indices.reserve(sq(division) * 6);
		{
			const auto vxy2i = [nVerticesSize](size_t x, size_t y)
			{
				return y * nVerticesSize + x;
			};
			for (size_t y = 0; y < division; y++)
			{
				for (size_t x = 0; x < division; x++)
				{
					const std::array<size_t, 4> IndexArray =
					{ vxy2i(x,y), vxy2i(x + 1,y), vxy2i(x,y + 1), vxy2i(x + 1,y + 1) };
					indices.push_back(IndexArray[0]);
					indices.push_back(IndexArray[2]);
					indices.push_back(IndexArray[1]);
					indices.push_back(IndexArray[1]);
					indices.push_back(IndexArray[2]);
					indices.push_back(IndexArray[3]);
				}
			}
		}
		return { std::move(vertices), std::move(indices) };
	}
};