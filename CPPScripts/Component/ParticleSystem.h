#pragma once
#include "../pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class Camera;
	class Material;
	class ParticleSystem : public Component
	{
		struct Particle
		{
			Vector3 position;
			Vector3 velocity;
			Vector4 color;
			float life = 0.0f;
		};

		struct InstanceData
		{
			Matrix4 model;
			Vector4 color;
		};

		friend class EditorInspectorPanel;
	public:
		static ComponentType GetType();

	public:
		// 粒子总数
		uint32_t mParticleNum = 0;
		// 粒子生命周期
		float mLifeTime = 0;
		// 粒子移动方向和速度
		Vector3 mVelocity;
		// 粒子生成位置偏移量
		Vector3 mOffset;

		ParticleSystem();
		~ParticleSystem();

		virtual ComponentType GetInsType();

		void Update();
		void Render(Camera* camera);

		void SetTexture(const char* path);
		void InternalGeneration();

	private:
		vector<Particle> mParticles;
		vector<InstanceData> mInstanceData;

		uint32_t mVAO = UINT32_MAX;
		uint32_t mTextureID = UINT32_MAX;
		uint32_t mLastUsedIndex = 0;
		uint32_t mInstanceBuffer = UINT32_MAX;
		uint32_t mInstanceDataSize = 0; // 以Vector4为单位
		Material* mMaterial = nullptr;

		float mLastGenTime = 0;
		Vector3 mLastPos;
		// 所挂载的GO当前移动方向
		Vector3 mMoveDir;

		uint32_t GetUnusedParticleIndex();
		void RespawnParticle(Particle& particle) const;
	};
}