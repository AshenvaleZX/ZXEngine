#pragma once
#include "pubh.h"
#include "Component.h"
#include "Texture.h"

namespace ZXEngine
{
	class Shader;
	class StaticMesh;
	class UITextureRenderer : public Component
	{
		friend class EditorInspectorPanel;
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
		StaticMesh* textureMesh = nullptr;

		StaticMesh* CreateTextureMesh();
	};
}