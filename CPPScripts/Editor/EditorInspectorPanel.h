#pragma once
#include "EditorPanel.h"
#include "EditorDataManager.h"

namespace ZXEngine
{
	class Material;
	class Transform;
	class RectTransform;
	class MeshRenderer;
	class Camera;
	class Light;
	class GameLogic;
	class UITextRenderer;
	class UITextureRenderer;
	class UIButton;
	class ParticleSystem;
	class BoxCollider;
	class PlaneCollider;
	class SphereCollider;
	class Circle2DCollider;
	class ZRigidBody;
	class Animator;
	class SpringJoint;
	class ZDistanceJoint;
	class Cloth;
	class AudioSource;
	class AudioListener;
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
		void DrawRectTransform(RectTransform* component);
		void DrawMeshRenderer(MeshRenderer* component);
		void DrawCamera(Camera* component);
		void DrawLight(Light* component);
		void DrawGameLogic(GameLogic* component);
		void DrawUITextRenderer(UITextRenderer* component);
		void DrawUITextureRenderer(UITextureRenderer* component);
		void DrawUIButton(UIButton* component);
		void DrawParticleSystem(ParticleSystem* component);
		void DrawBoxCollider(BoxCollider* component);
		void DrawPlaneCollider(PlaneCollider* component);
		void DrawSphereCollider(SphereCollider* component);
		void DrawCircle2DCollider(Circle2DCollider* component);
		void DrawRigidBody(ZRigidBody* component);
		void DrawAnimator(Animator* component);
		void DrawSpringJoint(SpringJoint* component);
		void DrawDistanceJoint(ZDistanceJoint* component);
		void DrawCloth(Cloth* component);
		void DrawAudioSource(AudioSource* component);
		void DrawAudioListener(AudioListener* component);
		
		// Asset
		void DrawScript(AssetScriptInfo* info);
		void DrawShader(AssetShaderInfo* info);
		void DrawTexture(AssetTextureInfo* info);
		void DrawMaterial(AssetMaterialInfo* info);
		void DrawModel(AssetModelInfo* info);
		void DrawAudio(AssetAudioInfo* info);
	};
}