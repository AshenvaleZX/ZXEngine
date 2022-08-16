#pragma once
#include "Component.h"
#include "ZMesh.h"

namespace ZXEngine
{
	class MeshRenderer : public Component
	{
	public:
		Mesh* mesh = nullptr;

		MeshRenderer() {};
		~MeshRenderer() {};

	private:

	};
}