#pragma once
#include "pubh.h"
#include "Component.h"
#include "Texture.h"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class UITextureRenderer : public Component
	{
	public:
		static Shader* shader;
		static void Init();

		UITextureRenderer() {};
		~UITextureRenderer() {};

		void Render();
		void SetTexture(const char* path);

	private:
		Texture* texture = nullptr;
		Mesh* textureMesh = nullptr;

		Mesh* CreateTextureMesh();
	};
}