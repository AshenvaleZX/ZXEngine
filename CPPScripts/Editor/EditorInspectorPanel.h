#pragma once
#include "EditorPanel.h"
#include "EditorDataManager.h"

namespace ZXEngine
{
	class Material;
	class Transform;
	class MeshRenderer;
	class Camera;
	class Light;
	class GameLogic;
	class UITextRenderer;
	class UITextureRenderer;
	class ParticleSystem;
	class EditorInspectorPanel : public EditorPanel
	{
	public:
		EditorInspectorPanel() {};
		~EditorInspectorPanel() {};

		virtual void DrawPanel();

	private:
		// Component
		void DrawMaterial(Material* material);
		void DrawTransform(Transform* component);
		void DrawMeshRenderer(MeshRenderer* component);
		void DrawCamera(Camera* component);
		void DrawLight(Light* component);
		void DrawGameLogic(GameLogic* component);
		void DrawUITextRenderer(UITextRenderer* component);
		void DrawUITextureRenderer(UITextureRenderer* component);
		void DrawParticleSystem(ParticleSystem* component);
		
		// Asset
		void DrawScript(AssetScriptInfo* info);
		void DrawShader(AssetShaderInfo* info);
		void DrawTexture(AssetTextureInfo* info);
		void DrawMaterial(AssetMaterialInfo* info);
	};
}