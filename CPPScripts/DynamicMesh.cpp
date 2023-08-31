#include "DynamicMesh.h"

namespace ZXEngine
{
	DynamicMesh::DynamicMesh(uint32_t vertexSize, uint32_t indexSize)
	{
		this->vertexSize = vertexSize;
		this->indexSize = indexSize;

		RenderAPI::GetInstance()->SetUpDynamicMesh(VAO, vertexSize, indexSize);
	}

	void DynamicMesh::UpdateData(const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		if (vertices.size() > vertexSize || indices.size() > indexSize)
		{
			Debug::LogError("Update dynamic mesh data failed, error data size!");
			return;
		}

		mVertices = vertices;
		mIndices = indices;

		RenderAPI::GetInstance()->UpdateDynamicMesh(VAO, vertices, indices);
	}
}