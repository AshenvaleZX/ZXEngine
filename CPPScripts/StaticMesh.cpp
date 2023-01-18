#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh::StaticMesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;

		RenderAPI::GetInstance()->SetUpMesh(VAO, VBO, EBO, vertices, indices);
	}

	void StaticMesh::Use()
	{
		RenderAPI::GetInstance()->SetMesh(VAO, (unsigned int)indices.size());
	}
}