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
	public:
		static ComponentType GetType();

	public:
		// 粒子总数
		unsigned int particleNum = 0;
		// 粒子生命周期
		float lifeTime = 0;
		// 粒子移动方向和速度
		Vector3 velocity;
		// 粒子生成位置偏移量
		Vector3 offset;

		ParticleSystem();
		~ParticleSystem();

		void Update();
		void Render(Shader* shader, Vector3 viewPos);

		void SetTexture(const char* path);
		void GenerateParticles();

	private:
		vector<Particle*> particles;
		unsigned int textureID = 0;
		unsigned int lastUsedIndex = 0;
		long long lastGenTime = 0;

		unsigned int GetUnusedParticleIndex();
		void RespawnParticle(Particle* particle);
	};
}