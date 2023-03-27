#include "ParticleSystemManager.h"
#include "ParticleSystem.h"
#include "ZShader.h"
#include "ZCamera.h"
#include "Transform.h"
#include "Resources.h"
#include "RenderStateSetting.h"

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
		RenderAPI::GetInstance()->GenerateParticleMesh(VAO);
		shader = new Shader(Resources::GetAssetFullPath("Shaders/Particle.zxshader", true), FrameBufferType::Normal);

		renderState = new RenderStateSetting();
		renderState->depthWrite = false;
		renderState->srcFactor = BlendFactor::SRC_ALPHA;
		renderState->dstFactor = BlendFactor::ONE;
		renderState->faceCull = false;
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
		// 切换粒子系统渲染设置
		RenderAPI::GetInstance()->SetRenderState(renderState);

		Vector3 camPos = camera->GetTransform()->GetPosition();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();
		shader->Use();
		shader->SetMat4("ENGINE_View", mat_V);
		shader->SetMat4("ENGINE_Projection", mat_P);

		for (auto particleSystem : allParticleSystem)
		{
			particleSystem->Render(shader, camPos);
		}
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