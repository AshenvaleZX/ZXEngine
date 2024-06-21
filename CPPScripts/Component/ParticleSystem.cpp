#include "ParticleSystem.h"
#include "ZCamera.h"
#include "Transform.h"
#include "../Time.h"
#include "../ZShader.h"
#include "../Material.h"
#include "../RenderAPI.h"
#include "../ParticleSystemManager.h"

namespace ZXEngine
{
	ComponentType ParticleSystem::GetType()
	{
		return ComponentType::ParticleSystem;
	}

	ParticleSystem::ParticleSystem() : mOffset(0), mVelocity(0), mMoveDir(0), mLastPos(0)
	{
		ParticleSystemManager::GetInstance()->AddParticleSystem(this);
	}

	ParticleSystem::~ParticleSystem()
	{
		ParticleSystemManager::GetInstance()->RemoveParticleSystem(this);

		if (mVAO != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteMesh(mVAO);

		if (mTextureID != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteTexture(mTextureID);

		if (mInstanceBuffer != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteInstanceBuffer(mInstanceBuffer);

		if (mMaterial != nullptr)
			delete mMaterial;
	}

	ComponentType ParticleSystem::GetInsType()
	{
		return ComponentType::ParticleSystem;
	}

	void ParticleSystem::Update()
	{
		// 更新当前位置和移动方向
		Vector3 curPos = GetTransform()->GetPosition();
		mMoveDir = (curPos - mLastPos).GetNormalized();
		mLastPos = curPos;

		// 粒子生产的时间间隔
		float interval = mLifeTime / static_cast<float>(mParticleNum);
		float curTime = Time::curTime;
		uint32_t genNum = 0;
		if (mLastGenTime == 0)
		{
			// 第一帧先初始化数据
			mLastGenTime = curTime;
		}
		else
		{
			// 上一次生产粒子的时间
			float delta = curTime - mLastGenTime;
			// 如果相差时间大于了生产间隔，那么根据相差的这段时间，计算这一帧应该生产的数量
			if (delta > interval)
				genNum = static_cast<uint32_t>(delta / interval);
			// 如果这一帧生产了粒子，那么更新生产时间
			if (genNum > 0)
				mLastGenTime = curTime;
		}

		for (uint32_t i = 0; i < genNum; i++)
		{
			auto idx = GetUnusedParticleIndex();
			RespawnParticle(mParticles[idx]);
		}

		float dt = Time::deltaTime;
		for (uint32_t i = 0; i < mParticleNum; i++)
		{
			Particle& p = mParticles[i];
			p.life -= dt;
			if (p.life > 0.0f)
			{
				p.position += p.velocity * dt;
				p.color.a -= dt * 1;
			}
		}
	}

	void ParticleSystem::Render(Camera* camera)
	{
		Vector3 camPos = camera->GetTransform()->GetPosition();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		bool caculateAngle = true;
		float hypotenuse = 0;
		float angle = 0;
		uint32_t activeNum = 0;
		for (auto& particle : mParticles)
		{
			if (particle.life > 0)
			{
				if (caculateAngle)
				{
					hypotenuse = static_cast<float>(sqrt(pow(camPos.x - particle.position.x, 2) + pow(camPos.z - particle.position.z, 2)));
					if (camPos.z > particle.position.x)
					{
						angle = asin((camPos.x - particle.position.x) / hypotenuse);
					}
					else
					{
						angle = asin((particle.position.x - camPos.x) / hypotenuse);
					}
					caculateAngle = false;
				}

				Matrix4 model = Math::Translate(Matrix4(1), particle.position);
				Matrix4 rotate = Math::Rotate(Matrix4(1), angle, Vector3(0.0f, 1.0f, 0.0f));
				Matrix4 scale = Math::Scale(Matrix4(1), Vector3(2.0f));
				Matrix4 mat_M = model * rotate * scale;
				mat_M.Transpose();

				mInstanceData[activeNum].model = std::move(mat_M);
				mInstanceData[activeNum].color = particle.color;

				activeNum++;
			}
		}

		mMaterial->Use();
		mMaterial->SetMatrix("ENGINE_View", mat_V);
		mMaterial->SetMatrix("ENGINE_Projection", mat_P);

		RenderAPI::GetInstance()->UpdateDynamicInstanceBuffer(mInstanceBuffer, mInstanceDataSize, activeNum, mInstanceData.data());

		RenderAPI::GetInstance()->DrawInstanced(mVAO, activeNum, mInstanceBuffer);
	}

	void ParticleSystem::SetTexture(const char* path)
	{
		if (mTextureID != UINT32_MAX)
			RenderAPI::GetInstance()->DeleteTexture(mTextureID);

		int width, height;
		mTextureID = RenderAPI::GetInstance()->LoadTexture(path, width, height);
	}

	void ParticleSystem::InternalGeneration()
	{
		mParticles.resize(mParticleNum);
		mInstanceData.resize(mParticleNum);

		mMaterial = new Material(ParticleSystemManager::GetInstance()->shader);
		mMaterial->Use();
		mMaterial->SetTexture("_Sprite", mTextureID, 0, true);

		mInstanceDataSize = ParticleSystemManager::GetInstance()->shader->reference->shaderInfo.instanceSize;

		RenderAPI::GetInstance()->GenerateParticleMesh(mVAO);
		
		mInstanceBuffer = RenderAPI::GetInstance()->CreateDynamicInstanceBuffer(mInstanceDataSize, mParticleNum);
		
		RenderAPI::GetInstance()->SetUpInstanceBufferAttribute(mVAO, mInstanceBuffer, mInstanceDataSize);
	}

	uint32_t ParticleSystem::GetUnusedParticleIndex()
	{
		// 从上一个用过的向后找
		for (uint32_t i = mLastUsedIndex; i < mParticleNum; ++i)
		{
			if (mParticles[i].life <= 0.0f)
			{
				mLastUsedIndex = i;
				return i;
			}
		}
		// 没找到就从头找
		for (uint32_t i = 0; i < mLastUsedIndex; ++i)
		{
			if (mParticles[i].life <= 0.0f)
			{
				mLastUsedIndex = i;
				return i;
			}
		}
		// 都没找到就从头开始
		mLastUsedIndex = 0;
		return 0;
	}

	void ParticleSystem::RespawnParticle(Particle& particle) const
	{
		float xRandom = Math::RandomFloat(-0.5f, 0.5f);
		float yRandom = Math::RandomFloat(-0.5f, 0.5f);
		float zRandom = Math::RandomFloat(-0.5f, 0.5f);
		float rColor = Math::RandomFloat(0.5f, 1.0f);
		float gColor = Math::RandomFloat(0.5f, 1.0f);
		float bColor = Math::RandomFloat(0.5f, 1.0f);
		particle.position = GetTransform()->GetPosition() + Vector3(xRandom * mOffset.x, yRandom * mOffset.y, zRandom * mOffset.z);
		particle.color = Vector4(rColor, gColor, bColor, 1.0f);
		particle.life = mLifeTime;
		particle.velocity = Math::GetRandomPerpendicular(mMoveDir) * 10;
	}
}