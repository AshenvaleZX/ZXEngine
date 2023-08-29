#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh::StaticMesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->mVertices = vertices;
		this->mIndices = indices;

		RenderAPI::GetInstance()->SetUpStaticMesh(VAO, vertices, indices);
	}
}