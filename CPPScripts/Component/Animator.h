#pragma once
#include "Component.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	class Mesh;
	class MeshRenderer;
	class AnimationController;
	class Animator : public Component
	{
	public:
		static ComponentType GetType();
		static void Update();
		static ComputeShaderReference* mComputeShader;

	private:
		static vector<Animator*> mAnimators;
		
		static void SyncUpdate();
		static void AsyncUpdate();

	public:
		string mAvatarName;
		// 模型骨骼根节点
		BoneNode* mRootBoneNode = nullptr;
		// 动画控制器
		AnimationController* mAnimationController = nullptr;
		// 对应的MeshRenderer
		MeshRenderer* mMeshRenderer = nullptr;

		Animator();
		~Animator();

		virtual ComponentType GetInsType();

		void Play(const string& name);
		void Switch(const string& name, float time = 1.0f);

	private:
		uint32_t mCommand = 0;

		void UpdateMeshes(bool async);
		void AccomplishUpdate();
		void UpdateVertices();
	};
}