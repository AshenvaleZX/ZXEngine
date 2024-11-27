#pragma once
#include "../pubh.h"
#include "Component.h"
#include "../ParticleSystem/ParticleEmitter.h"
#include "../ParticleSystem/ParticleRenderer.h"
#include "../ParticleSystem/ParticleSystemDefinition.h"

namespace ZXEngine
{
	class Camera;
	class ParticleSystem : public Component
	{
		friend class EditorInspectorPanel;
	public:
		static ComponentType GetType();

	public:
		ParticleSystemState mState;

		ParticleSystem();
		~ParticleSystem();

		virtual ComponentType GetInsType();

		void Update();
		void Render(Camera* camera);

		void SetTexture(const string& path);
		void InternalGeneration();

	private:
		ParticleEmitter mEmitter;
		ParticleRenderer mRenderer;

		vector<Particle> mParticles;
	};
}