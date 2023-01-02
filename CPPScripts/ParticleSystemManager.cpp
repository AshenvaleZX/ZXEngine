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
		shader = new Shader(Resources::GetAssetFullPath("Shaders/Particle.zxshader", true).c_str());
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
		// 不写入深度，否则粒子之间会相互遮挡。但是要做深度测试，否则会错误遮挡前面的对象
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		// 渲染粒子时只向颜色缓冲区叠加颜色
		RenderAPI::GetInstance()->SetBlendMode(BlendOption::SRC_ALPHA, BlendOption::ONE);

		Vector3 camPos = camera->GetTransform()->GetPosition();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();
		shader->Use();
		shader->SetMat4("view", mat_V);
		shader->SetMat4("projection", mat_P);

		for (auto particleSystem : allParticleSystem)
		{
			particleSystem->Render(shader, camPos);
		}

		// 还原设置
		RenderAPI::GetInstance()->EnableDepthWrite(true);
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