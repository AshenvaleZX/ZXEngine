#pragma once
#include "pubh.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	class Mesh
	{
	public:
		// Vertex Array Object
		unsigned int VAO;
		// Vertex Buffer Objects
		unsigned int VBO;
		// Element Buffer Objects
		unsigned int EBO;

		vector<Vertex> vertices;
		vector<unsigned int> indices;

		~Mesh();

		virtual void Use() = 0;
	};
}