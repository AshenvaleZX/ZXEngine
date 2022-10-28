#pragma once
#include "Component.h"
#include "ZMesh.h"
#include "Material.h"

namespace ZXEngine
{
	class MeshRenderer : public Component
	{
	public:
		Mesh* mesh = nullptr;
		Material* matetrial = nullptr;

		MeshRenderer() {};
		~MeshRenderer() {};

	private:

	};
}