#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh::StaticMesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;

		RenderAPI::GetInstance()->SetUpStaticMesh(VAO, vertices, indices);
	}
}