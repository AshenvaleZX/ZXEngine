#include "DynamicMesh.h"

namespace ZXEngine
{
	DynamicMesh::DynamicMesh(unsigned int vertexSize, unsigned int indexSize)
	{
		this->vertexSize = vertexSize;
		this->indexSize = indexSize;

		RenderAPI::GetInstance()->SetUpDynamicMesh(VAO, vertexSize, indexSize);
	}

	void DynamicMesh::UpdateData(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		if (vertices.size() > vertexSize || indices.size() > indexSize)
		{
			Debug::LogError("Update dynamic mesh data failed, error data size!");
			return;
		}

		this->mVertices = vertices;
		this->mIndices = indices;

		RenderAPI::GetInstance()->UpdateDynamicMesh(VAO, vertices, indices);
	}
}