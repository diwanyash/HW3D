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

	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};