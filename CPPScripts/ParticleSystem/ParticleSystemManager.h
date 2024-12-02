#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Camera;
	class Shader;
	class ParticleSystem;
	class RenderStateSetting;
	class ParticleSystemManager
	{
	public:
		static void Create();
		static ParticleSystemManager* GetInstance();
	private:
		static ParticleSystemManager* mInstance;

	public:
		Shader* shader;

		ParticleSystemManager();
		~ParticleSystemManager() {};

		void Update();
		void Render(Camera* camera);

		void AddParticleSystem(ParticleSystem* ps);
		void RemoveParticleSystem(ParticleSystem* ps);

	private:
		list<ParticleSystem*> allParticleSystem;
		RenderStateSetting* renderState;
	};
}