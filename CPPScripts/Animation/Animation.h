#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class NodeAnimation;
	class Animation
	{
	public:
		string mName;
		float mDuration;
		float mTicksPerSecond;

		~Animation();

		void Play();
		void AddNodeAnimation(NodeAnimation* nodeAnimation);

	private:
		unordered_map<string, NodeAnimation*> mNodeAnimations;
	};
}