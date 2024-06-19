#pragma once
#include "../pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class Material;
	struct Particle
	{
		uint32_t VAO = 0;
		Vector3 position;
		Vector3 velocity;
		Vector4 color;
		Material* material = nullptr;
		float life = 0.0f;
	};

	class Camera;
	class ParticleSystem : public Component
	{
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
		void GenerateParticles();

	private:
		vector<Particle> mParticles;
		uint32_t mTextureID = 0;
		uint32_t mLastUsedIndex = 0;
		float mLastGenTime = 0;
		Vector3 mLastPos;
		// �����ص�GO��ǰ�ƶ�����
		Vector3 mMoveDir;

		uint32_t GetUnusedParticleIndex();
		void RespawnParticle(Particle& particle) const;
	};
}