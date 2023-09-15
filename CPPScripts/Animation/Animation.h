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
		// �������й����ڵ��б���Ķ���
		void Update();
		// ����ǰ��һ֡�Ĺ����任������µ�����Mesh��(�������ڵ�����������)
		void UpdateMeshes(const BoneNode* pBoneNode, const vector<Mesh*>& pMeshes);
		void AddNodeAnimation(NodeAnimation* nodeAnimation);

	private:
		bool mIsPlaying = false;
		// ���й������Ķ���
		unordered_map<string, NodeAnimation*> mNodeAnimations;
		// ���й����������ձ任����(�����ŵ�������ʱ����)
		unordered_map<string, Matrix4> mBoneTransforms;

		void UpdateNodeAnimations();
		void UpdateBoneTransforms(const BoneNode* pBoneNode, const Matrix4& parentTransform);
		NodeAnimation* GetNodeAnimation(const string& nodeName);
	};
}