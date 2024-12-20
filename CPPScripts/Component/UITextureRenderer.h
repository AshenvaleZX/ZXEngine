#pragma once
#include "../pubh.h"
#include "../Texture.h"
#include "Component.h"

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
		UITextureRenderer();
		~UITextureRenderer();

		virtual ComponentType GetInsType();

		void Render(const Matrix4& matVP);
		void SetTexture(const string& path);
		void OnWindowResize(const string& args);
		inline bool IsScreenSpace() const { return isScreenSpace; }

	private:
		bool isScreenSpace = true;
		Texture* texture = nullptr;
		Material* material = nullptr;
		StaticMesh* textureMesh = nullptr;
		uint32_t mWindowResizeCallbackKey = 0;

		void CreateRenderData();
	};
}