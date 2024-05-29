#pragma once
#include "../pubh.h"
#include "../PublicStruct.h"

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
		float mDuration = 0.0f;
		float mTicksPerSecond = 0.0f;

		~Animation();

		void Play(bool isReplay = true);
		void Stop();
		void Reset();
		bool IsPlaying() const;

		// 更新所有骨骼节点列表里的动画
		void Update(float deltaTime);
		// 更新所有骨骼节点列表里的动画(手动传入Tick)
		void ForceUpdate(float tick);

		// 将当前这一帧的骨骼变换矩阵更新到所有Mesh里(仅适用于单个动画播放)
		void UpdateMeshes(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes);
		void UpdateFinalTransforms(const BoneNode* pBoneNode, 
			const vector<vector<Matrix4>>& bonesOffsets, 
			const vector<unordered_map<string, uint32_t>>& boneNameToIndexMaps, 
			vector<vector<Matrix4>>& bonesFinalTransforms);

		void AddNodeAnimation(NodeAnimation* nodeAnimation);
		bool GetCurFrameByNode(const string& nodeName, KeyFrame& keyFrame);

	private:
		bool mIsPlaying = false;
		// 所有骨骼结点的动画
		unordered_map<string, NodeAnimation*> mNodeAnimations;
		// 所有骨骼结点的最终变换矩阵(仅播放单个动画时有用)
		unordered_map<string, Matrix4> mBoneTransforms;

		void UpdateNodeAnimations();
		void UpdateBoneTransforms(const BoneNode* pBoneNode, const Matrix4& parentTransform);
		NodeAnimation* GetNodeAnimation(const string& nodeName);
	};
}