#include "DynamicMesh.h"

namespace ZXEngine
{
	DynamicMesh::DynamicMesh(unsigned int vertexSize, unsigned int indexSize)
	{
		this->vertexSize = vertexSize;
		this->indexSize = indexSize;

		RenderAPI::GetInstance()->SetUpDynamicMesh(VAO, VBO, EBO, vertexSize, indexSize);
	}

	void DynamicMesh::Use()
	{
		RenderAPI::GetInstance()->SetMesh(VAO, indexSize);
	}

	void DynamicMesh::UpdateData(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		if (vertices.size() != vertexSize || indices.size() != indexSize)
		{
			Debug::LogError("Update dynamic mesh data failed, error data size!");
			return;
		}

		this->vertices = vertices;
		this->indices = indices;

		RenderAPI::GetInstance()->UpdateDynamicMesh(VAO, VBO, EBO, vertices, indices);
	}
}