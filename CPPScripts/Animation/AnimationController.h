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

		void Update(BoneNode* pBoneNode, const vector<Mesh*>& pMeshes);
		void AddAnimation(Animation* anim);
		void PlayAnimation(string name);

	private:
		Animation* mCurAnimation;
		unordered_map<string, Animation*> mAnimations;
	};
}