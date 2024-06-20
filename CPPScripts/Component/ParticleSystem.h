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
		// ��������
		uint32_t mParticleNum = 0;
		// ������������
		float mLifeTime = 0;
		// �����ƶ�������ٶ�
		Vector3 mVelocity;
		// ��������λ��ƫ����
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
		uint32_t mInstanceDataSize = 0; // ��Vector4Ϊ��λ
		Material* mMaterial = nullptr;

		float mLastGenTime = 0;
		Vector3 mLastPos;
		// �����ص�GO��ǰ�ƶ�����
		Vector3 mMoveDir;

		uint32_t GetUnusedParticleIndex();
		void RespawnParticle(Particle& particle) const;
	};
}