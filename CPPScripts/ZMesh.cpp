#include "ZMesh.h"

namespace ZXEngine
{
	Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;
	}
}