#pragma once
#include "Component.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	class Mesh;
	class AnimationController;
	class Animator : public Component
	{
	public:
		static ComponentType GetType();

	public:
		string mAvatarName;
		// ģ�͹������ڵ�
		BoneNode* mRootBoneNode = nullptr;
		// ����������
		AnimationController* mAnimationController = nullptr;

		~Animator();

		virtual ComponentType GetInsType();

		void Play(const string& name);
		void Switch(const string& name, float time = 1.0f);
		void Update(const vector<Mesh*>& pMeshes);
	};
}