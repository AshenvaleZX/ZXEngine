#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Mesh;
	struct BoneNode;
	class Animation;
	class AnimationController
	{
	public:
		~AnimationController();

		void Update(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes);
		void Add(Animation* anim);
		void Play(const string& name);
		void Switch(const string& name, float time = 1.0f);

	private:
		bool mIsBlending = false;
		float mBlendFactor = 0.0f;
		// 混合已消耗的时间
		float mBlendTime = 0.0f;
		// 当前动画混合到目标动画的总时间
		float mBlendDuration = 0.0f;
		// 当前这个混合动画(过渡状态下的临时动画)的播放时间
		float mCurBlendAnimTime = 0.0f;
		
		Animation* mCurAnimation = nullptr;
		Animation* mTargetAnimation = nullptr;
		unordered_map<string, Animation*> mAnimations;

		void UpdateBlendAnimation(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes, const Matrix4& parentTransform);
	};
}