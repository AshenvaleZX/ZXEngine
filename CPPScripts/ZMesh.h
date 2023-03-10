#pragma once
#include "pubh.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	class Mesh
	{
	public:
		unsigned int VAO;

		vector<Vertex> vertices;
		vector<unsigned int> indices;

		~Mesh();
	};
}