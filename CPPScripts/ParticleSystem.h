#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
	struct Particle
	{
		Vector3 position;
		Vector3 velocity;
		Vector4 color;
		float life;

		Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
	};

	class Shader;
	class ParticleSystem : public Component
	{
#ifdef ZX_EDITOR
		friend class EditorInspectorPanel;
#endif
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
		void Render(Shader* shader, Vector3 viewPos);

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