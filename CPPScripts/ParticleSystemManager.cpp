#include "ParticleSystemManager.h"
#include "ParticleSystem.h"
#include "ZShader.h"
#include "ZCamera.h"
#include "Transform.h"
#include "Resources.h"

namespace ZXEngine
{
	ParticleSystemManager* ParticleSystemManager::mInstance = nullptr;

	void ParticleSystemManager::Create()
	{
		mInstance = new ParticleSystemManager();
	}

	ParticleSystemManager* ParticleSystemManager::GetInstance()
	{
		return mInstance;
	}

	ParticleSystemManager::ParticleSystemManager()
	{
		VAO = RenderAPI::GetInstance()->GenerateParticleMesh();
		shader = new Shader(Resources::GetAssetFullPath("Shaders/Particle.zxshader").c_str());
	}

	void ParticleSystemManager::Update()
	{
		for (auto particleSystem : allParticleSystem)
		{
			particleSystem->Update();
		}
	}

	void ParticleSystemManager::Render(Camera* camera)
	{
		RenderAPI::GetInstance()->SetBlendMode(BlendOption::SRC_ALPHA, BlendOption::ONE);

		vec3 camPos = camera->GetTransform()->position;
		mat4 mat_V = camera->GetViewMatrix();
		mat4 mat_P = camera->GetProjectionMatrix();
		shader->Use();
		shader->SetMat4("view", mat_V);
		shader->SetMat4("projection", mat_P);

		for (auto particleSystem : allParticleSystem)
		{
			particleSystem->Render(shader, camPos);
		}

		RenderAPI::GetInstance()->SetBlendMode(BlendOption::SRC_ALPHA, BlendOption::ONE_MINUS_SRC_ALPHA);
	}

	void ParticleSystemManager::AddParticleSystem(ParticleSystem* ps)
	{
		allParticleSystem.push_back(ps);
	}

	void ParticleSystemManager::RemoveParticleSystem(ParticleSystem* ps)
	{
		auto res = std::find(allParticleSystem.begin(), allParticleSystem.end(), ps);
		allParticleSystem.erase(res);
	}
}