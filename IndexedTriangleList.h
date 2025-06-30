#pragma once
#include "DeadMath.h"
#include <vector>
#include <cassert>

template <typename T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> vertices_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(vertices_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}


	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(
				&v.pos,
				DirectX::XMVector3Transform(pos,matrix)
			);
		}
	}

	void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
	{
		namespace dx = DirectX;
		assert( indices.size() % 3 == 0 && indices.size() > 0);
		for ( size_t i = 0; i < indices.size(); i += 3 )
		{
			auto& v0 = vertices[ indices[ i ]];
			auto& v1 = vertices[ indices[ i + 1 ]];
			auto& v2 = vertices[ indices[ i + 2 ]];
			const auto p0 = dx::XMStoreFloat3( &v0.pos );
			const auto p1 = dx::XMStoreFloat3( &v1.pos );
			const auto p2 = dx::XMStoreFloat3( &v2.pos );

			const auto n = dx::XMVector3Normalize( dx::XMVector3Cross( (p1 - p0), (p2 - p0) ));

			dx::XMStoreFloat3( &v0.n, n );
			dx::XMStoreFloat3( &v1.n, n );
			dx::XMStoreFloat3( &v2.n, n );
		}
	}

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};