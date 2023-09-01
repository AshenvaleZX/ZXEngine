#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Animation;
	class AnimationController
	{
	public:
		~AnimationController();

		void AddAnimation(Animation* anim);
		void PlayAnimation(string name);

	private:
		Animation* mCurAnimation;
		unordered_map<string, Animation*> mAnimations;
	};
}