#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Mesh;
	struct BoneNode;
	class NodeAnimation;
	class Animation
	{
	public:
		string mName;
		bool mIsLoop = true;
		float mSpeed = 1.0f;
		float mCurTick = 0.0f;
		float mFullTick = 0.0f;
		float mTicksPerSecond = 0.0f;

		~Animation();

		void Play(bool isReplay = true);
		void Stop();
		void Reset();
		bool IsPlaying() const;
		void Update(BoneNode* pBoneNode, const vector<Mesh*>& pMeshes);
		void AddNodeAnimation(NodeAnimation* nodeAnimation);

	private:
		bool mIsPlaying = false;
		unordered_map<string, NodeAnimation*> mNodeAnimations;

		NodeAnimation* GetNodeAnimation(const string& nodeName);
		void GetNodeTransform(const BoneNode* pNode, const Matrix4& parentTransform, const vector<Mesh*>& pMeshes);
	};
}