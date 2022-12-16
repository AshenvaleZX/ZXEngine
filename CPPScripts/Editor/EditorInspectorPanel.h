#pragma once
#include "EditorPanel.h"

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
		void DrawMaterial(Material* material);
		void DrawTransform(Transform* component);
		void DrawMeshRenderer(MeshRenderer* component);
		void DrawCamera(Camera* component);
		void DrawLight(Light* component);
		void DrawGameLogic(GameLogic* component);
		void DrawUITextRenderer(UITextRenderer* component);
		void DrawUITextureRenderer(UITextureRenderer* component);
		void DrawParticleSystem(ParticleSystem* component);
	};
}