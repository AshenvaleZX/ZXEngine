#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
	class Shader;
	class ParticleSystem;
	class ParticleSystemManager
	{
	public:
		static void Create();
		static ParticleSystemManager* GetInstance();
	private:
		static ParticleSystemManager* mInstance;

	public:
		unsigned int VAO;

		ParticleSystemManager();
		~ParticleSystemManager() {};

		void Update();
		void Render(Camera* camera);

		void AddParticleSystem(ParticleSystem* ps);
		void RemoveParticleSystem(ParticleSystem* ps);

	private:
		Shader* shader;
		list<ParticleSystem*> allParticleSystem;
	};
}