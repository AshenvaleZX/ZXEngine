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

		void Update(const BoneNode* pBoneNode, const vector<Mesh*>& pMeshes);
		void AddAnimation(Animation* anim);
		void PlayAnimation(const string& name);

	private:
		Animation* mCurAnimation = nullptr;
		unordered_map<string, Animation*> mAnimations;
	};
}