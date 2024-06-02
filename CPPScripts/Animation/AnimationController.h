#pragma once
#include "../pubh.h"
#include "../Concurrent/Job.h"
#include "../Concurrent/ThreadSafeData.h"

namespace ZXEngine
{
	struct BoneNode;
	class Animation;
	
	class AnimationUpdateJob : public Job
	{
	public:
		struct Data
		{
			const BoneNode* mBoneNode = nullptr;
			vector<vector<Matrix4>> mBonesOffsets;
			vector<vector<Matrix4>> mBonesFinalTransforms;
			vector<unordered_map<string, uint32_t>> mBoneNameToIndexMaps;

			float mDeltaTime = 0.0f;
			Animation* mAnimation = nullptr;
		};

		ThreadSafeData<Data> mSafeData;

		void Execute() override;
	};

	class AnimationBlendUpdateJob : public Job
	{
	public:
		struct Data
		{
			const BoneNode* mBoneNode = nullptr;
			vector<vector<Matrix4>> mBonesOffsets;
			vector<vector<Matrix4>> mBonesFinalTransforms;
			vector<unordered_map<string, uint32_t>> mBoneNameToIndexMaps;

			float mBlendFactor = 0.0f;
			float mCurAnimTick = 0.0f;
			float mTargetAnimTick = 0.0f;
			Animation* mCurAnimation = nullptr;
			Animation* mTargetAnimation = nullptr;
		};

		ThreadSafeData<Data> mSafeData;

		void Execute() override;
		void UpdateBlendTransforms(const BoneNode* pBoneNode, Data& data, const Matrix4& parentTransform);
	};

	class Mesh;
	class AnimationController
	{
	public:
		~AnimationController();

		void Update(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes, bool async);
		void AccomplishUpdate(const vector<shared_ptr<Mesh>>& pMeshes);
		void Add(Animation* anim);
		void Play(const string& name);
		void Switch(const string& name, float time = 1.0f);

	private:
		bool mIsBlending = false;
		float mBlendFactor = 0.0f;
		// ��������ĵ�ʱ��
		float mBlendTime = 0.0f;
		// ��ǰ������ϵ�Ŀ�궯������ʱ��
		float mBlendDuration = 0.0f;
		// ��ǰ�����϶���(����״̬�µ���ʱ����)�Ĳ���ʱ��
		float mCurBlendAnimTime = 0.0f;
		
		Animation* mCurAnimation = nullptr;
		Animation* mTargetAnimation = nullptr;
		unordered_map<string, Animation*> mAnimations;

		AnimationUpdateJob mUpdateJob;
		AnimationBlendUpdateJob mBlendUpdateJob;
		std::optional<JobHandle> mUpdateHandle = std::nullopt;
		std::optional<JobHandle> mBlendUpdateHandle = std::nullopt;

		void UpdateBlendAnimation(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes, const Matrix4& parentTransform);
	};
}