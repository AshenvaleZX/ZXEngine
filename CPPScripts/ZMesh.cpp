#include "ZMesh.h"

namespace ZXEngine
{
	Mesh::~Mesh()
	{
		RenderAPI::GetInstance()->DeleteMesh(VAO);
		RenderAPI::GetInstance()->DeleteBuffer(VBO);
		RenderAPI::GetInstance()->DeleteBuffer(EBO);
	}
}