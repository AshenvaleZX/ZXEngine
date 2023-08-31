#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh::StaticMesh(const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		mVertices = vertices;
		mIndices = indices;

		RenderAPI::GetInstance()->SetUpStaticMesh(VAO, vertices, indices);
	}
}