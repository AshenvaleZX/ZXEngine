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
#ifdef ZX_EDITOR
		friend class EditorInspectorPanel;
#endif
	public:
		static Shader* shader;
		static void Init();
		static ComponentType GetType();

	public:
		UITextureRenderer() {};
		~UITextureRenderer();

		virtual ComponentType GetInsType();

		void Render();
		void SetTexture(const char* path);

	private:
		Texture* texture = nullptr;
		Mesh* textureMesh = nullptr;

		Mesh* CreateTextureMesh();
	};
}