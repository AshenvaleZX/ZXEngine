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
		unsigned int particleNum = 0;
		// ������������
		float lifeTime = 0;
		// �����ƶ�������ٶ�
		Vector3 velocity;
		// ��������λ��ƫ����
		Vector3 offset;

		ParticleSystem();
		~ParticleSystem();

		virtual ComponentType GetInsType();

		void Update();
		void Render(Camera* camera);

		void SetTexture(const char* path);
		void GenerateParticles();

	private:
		vector<Particle*> particles;
		unsigned int textureID = 0;
		unsigned int lastUsedIndex = 0;
		float lastGenTime = 0;
		Vector3 lastPos;
		// �����ص�GO��ǰ�ƶ�����
		Vector3 moveDir;

		unsigned int GetUnusedParticleIndex();
		void RespawnParticle(Particle* particle);
	};
}