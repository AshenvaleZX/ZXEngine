#pragma once
#include "pubh.h"
#include "Component.h"
#include "Texture.h"

namespace ZXEngine
{
	class Shader;
	class Material;
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
		Material* material = nullptr;
		StaticMesh* textureMesh = nullptr;

		void CreateRenderData();
	};
}