#include "ZMesh.h"

namespace ZXEngine
{
	Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;

		RenderAPI::GetInstance()->SetUpMesh(VAO, VBO, EBO, vertices, indices);
	}

	Mesh::~Mesh()
	{
		RenderAPI::GetInstance()->DeleteMesh(VAO);
		RenderAPI::GetInstance()->DeleteBuffer(VBO);
		RenderAPI::GetInstance()->DeleteBuffer(EBO);
	}

	void Mesh::Use()
	{
		RenderAPI::GetInstance()->SetMesh(VAO, indices.size());
	}
}