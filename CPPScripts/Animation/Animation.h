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
		bool mIsLoop = false;
		float mSpeed = 1.0f;
		float mCurTime = 0.0f;
		float mDuration = 0.0f;
		float mTicksPerSecond = 0.0f;

		~Animation();

		void Update(BoneNode* pBoneNode, const vector<Mesh*>& pMeshes);
		void AddNodeAnimation(NodeAnimation* nodeAnimation);

	private:
		unordered_map<string, NodeAnimation*> mNodeAnimations;

		NodeAnimation* GetNodeAnimation(const string& nodeName);
		void GetNodeTransform(const BoneNode* pNode, const Matrix4& parentTransform, const vector<Mesh*>& pMeshes);
	};
}