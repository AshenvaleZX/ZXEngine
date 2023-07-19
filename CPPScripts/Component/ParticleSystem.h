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
		// 所挂载的GO当前移动方向
		Vector3 moveDir;

		unsigned int GetUnusedParticleIndex();
		void RespawnParticle(Particle* particle);
	};
}