#include "Animation.h"
#include "NodeAnimation.h"

namespace ZXEngine
{
	Animation::~Animation()
	{
		for (auto& iter : mNodeAnimations)
		{
			delete iter.second;
		}
	}

	void Animation::Play()
	{

	}

	void Animation::AddNodeAnimation(NodeAnimation* nodeAnimation)
	{
		if (mNodeAnimations.find(nodeAnimation->mName) == mNodeAnimations.end())
		{
			mNodeAnimations[nodeAnimation->mName] = nodeAnimation;
		}
		else
		{
			Debug::LogWarning("Animation try to add an existing node animation: %s", nodeAnimation->mName);
		}
	}
}