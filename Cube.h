#pragma once
#include "IndexedTriangleList.h"
#include "DirectXMath.h"

class Cube {

public:
	template<class V>
	static IndexedTriangleList<V> Make(float size = 1.0f)
	{
		namespace dx = DirectX;

		const float side = size / 2.0f;

		std::vector<dx::XMFLOAT3> Vertices;

		Vertices.emplace_back(-side, -side, -side); // 0
		Vertices.emplace_back(side, -side, -side); // 1
		Vertices.emplace_back(-side, side, -side); // 2
		Vertices.emplace_back(side, side, -side); // 3
		Vertices.emplace_back(-side, -side, side); // 4
		Vertices.emplace_back(side, -side, side); // 5
		Vertices.emplace_back(-side, side, side); // 6
		Vertices.emplace_back(side, side, side); // 7 


		std::vector<V> verts(Vertices.size());
		for (size_t i = 0; i < Vertices.size(); i++)
		{
			verts[i].pos = Vertices[i];
		}

		return
		{
				std::move(verts),
				{
						0,2,1, 2,3,1,
						1,3,5, 3,7,5,
						2,6,3, 3,6,7,
						4,5,7, 4,7,6,
						0,4,2, 2,4,6,
						0,1,4, 1,5,4
				}
		};

	}
};