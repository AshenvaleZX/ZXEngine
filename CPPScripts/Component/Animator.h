#pragma once
#include "Component.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	class Mesh;
	class MeshRenderer;
	class AnimationController;
	class Animator : public Component
	{
	public:
		static ComponentType GetType();
		static void Update();

	private:
		static vector<Animator*> mAnimators;
		
		static void SyncUpdate();
		static void AsyncUpdate();

	public:
		string mAvatarName;
		// ģ�͹������ڵ�
		BoneNode* mRootBoneNode = nullptr;
		// ����������
		AnimationController* mAnimationController = nullptr;
		// ��Ӧ��MeshRenderer
		MeshRenderer* mMeshRenderer = nullptr;

		Animator();
		~Animator();

		virtual ComponentType GetInsType();

		void Play(const string& name);
		void Switch(const string& name, float time = 1.0f);

	private:
		void UpdateMeshes(bool async);
		void AccomplishUpdate();
	};
}