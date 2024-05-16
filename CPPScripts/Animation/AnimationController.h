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

		void Update(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes);
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

		void UpdateBlendAnimation(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes, const Matrix4& parentTransform);
	};
}