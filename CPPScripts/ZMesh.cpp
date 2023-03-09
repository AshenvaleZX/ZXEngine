#include "ZMesh.h"

namespace ZXEngine
{
	Mesh::~Mesh()
	{
		RenderAPI::GetInstance()->DeleteMesh(VAO);
	}
}