#include "ParticleRenderer.h"
#include "../RenderAPI.h"
#include "../ZShader.h"
#include "../Material.h"
#include "../Component/ZCamera.h"
#include "../Component/Transform.h"
#include "ParticleSystemManager.h"

namespace ZXEngine
{
	ParticleRenderer::~ParticleRenderer()
	{
		if (mVAO != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteMesh(mVAO);

		if (mTextureID != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteTexture(mTextureID);

		if (mInstanceBuffer != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteInstanceBuffer(mInstanceBuffer);

		if (mMaterial != nullptr)
			delete mMaterial;
	}

	void ParticleRenderer::Render(Camera* camera, const vector<Particle>& particles)
	{
		Vector3 camPos = camera->GetTransform()->GetPosition();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		UpdateInstanceData(camPos, particles);

		mMaterial->Use();
		mMaterial->SetMatrix("ENGINE_View", mat_V);
		mMaterial->SetMatrix("ENGINE_Projection", mat_P);

		RenderAPI::GetInstance()->UpdateDynamicInstanceBuffer(mInstanceBuffer, mInstanceDataSize, mInstanceActiveNum, mInstanceData.data());

		RenderAPI::GetInstance()->DrawInstanced(mVAO, mInstanceActiveNum, mInstanceBuffer);
	}

	void ParticleRenderer::SetTexture(const string& path)
	{
		if (mTextureID != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteTexture(mTextureID);

		int width, height;
		mTextureID = RenderAPI::GetInstance()->LoadTexture(path.c_str(), width, height);
	}

	void ParticleRenderer::Init(const ParticleSystemState& state)
	{
		SetTexture(Resources::GetAssetFullPath(mTexturePath));

		mInstanceData.resize(state.mMaxParticleNum);

		mMaterial = new Material(ParticleSystemManager::GetInstance()->shader);
		mMaterial->Use();
		mMaterial->SetTexture("_Sprite", mTextureID, 0, true);

		mInstanceDataSize = ParticleSystemManager::GetInstance()->shader->reference->shaderInfo.instanceInfo.size;

		RenderAPI::GetInstance()->GenerateParticleMesh(mVAO);

		mInstanceBuffer = RenderAPI::GetInstance()->CreateDynamicInstanceBuffer(mInstanceDataSize, state.mMaxParticleNum);

		RenderAPI::GetInstance()->SetUpInstanceBufferAttribute(mVAO, mInstanceBuffer, mInstanceDataSize);
	}

	void ParticleRenderer::UpdateInstanceData(const Vector3& camPos, const vector<Particle>& particles)
	{
		mInstanceActiveNum = 0;

		for (auto& particle : particles)
		{
			if (particle.life > 0)
			{
				Matrix4 model;
				model.Scale(Vector3(2.0f));
				model.Rotate(Vector3::Back, (camPos - particle.position).GetNormalized());
				model.Translate(particle.position);
				model.Transpose();

				mInstanceData[mInstanceActiveNum].model = std::move(model);
				mInstanceData[mInstanceActiveNum].color = particle.color;

				mInstanceActiveNum++;
			}
		}
	}
}