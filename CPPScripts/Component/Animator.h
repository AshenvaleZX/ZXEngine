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
		// 模型骨骼根节点
		BoneNode* mRootBoneNode = nullptr;
		// 动画控制器
		AnimationController* mAnimationController = nullptr;

		~Animator();

		virtual ComponentType GetInsType();

		void Play(const string& name);
		void Update(const vector<Mesh*>& pMeshes);
	};
}