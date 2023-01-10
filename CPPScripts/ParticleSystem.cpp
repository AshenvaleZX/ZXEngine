#include "ParticleSystem.h"
#include "ParticleSystemManager.h"
#include "Time.h"
#include "RenderAPI.h"
#include "ZShader.h"
#include "Transform.h"

namespace ZXEngine
{
	ComponentType ParticleSystem::GetType()
	{
		return ComponentType::ParticleSystem;
	}

	ParticleSystem::ParticleSystem() : offset(0), velocity(0), moveDir(0), lastPos(0)
	{
		ParticleSystemManager::GetInstance()->AddParticleSystem(this);
	}

	ParticleSystem::~ParticleSystem()
	{
		ParticleSystemManager::GetInstance()->RemoveParticleSystem(this);

		for (auto particle : particles)
			delete particle;
	}

	ComponentType ParticleSystem::GetInsType()
	{
		return ComponentType::ParticleSystem;
	}

	void ParticleSystem::Update()
	{
		// 更新当前位置和移动方向
		Vector3 curPos = GetTransform()->GetPosition();
		moveDir = (curPos - lastPos).Normalize();
		lastPos = curPos;

		// 粒子生产的时间间隔
		float interval = lifeTime / (float)particleNum;
		float curTime = Time::curTime;
		int genNum = 0;
		if (lastGenTime == 0)
		{
			// 第一帧先初始化数据
			lastGenTime = curTime;
		}
		else
		{
			// 上一次生产粒子的时间
			float delta = curTime - lastGenTime;
			// 如果相差时间大于了生产间隔，那么根据相差的这段时间，计算这一帧应该生产的数量
			if (delta > interval)
				genNum = (int)(delta / interval);
			// 如果这一帧生产了粒子，那么更新生产时间
			if (genNum > 0)
				lastGenTime = curTime;
		}

		for (auto i = 0; i < genNum; i++)
		{
			auto idx = GetUnusedParticleIndex();
			RespawnParticle(particles[idx]);
		}

		float dt = Time::deltaTime;
		for (unsigned int i = 0; i < particleNum; i++)
		{
			Particle* p = particles[i];
			p->life -= dt;
			if (p->life > 0.0f)
			{
				p->position += p->velocity * dt;
				p->color.a -= dt * 1;
			}
		}
	}

	void ParticleSystem::Render(Shader* shader, Vector3 viewPos)
	{
		unsigned int VAO = ParticleSystemManager::GetInstance()->VAO;
		shader->SetTexture("_Sprite", textureID, 0);
		bool caculateAngle = true;
		float hypotenuse = 0;
		float angle = 0;
		for (auto particle : particles)
		{
			if (particle->life > 0)
			{
				if (caculateAngle)
				{
					hypotenuse = (float)sqrt(pow(viewPos.x - particle->position.x, 2) + pow(viewPos.z - particle->position.z, 2));
					if (viewPos.z > particle->position.x)
					{
						angle = asin((viewPos.x - particle->position.x) / hypotenuse);
					}
					else
					{
						angle = asin((particle->position.x - viewPos.x) / hypotenuse);
					}
					caculateAngle = false;
				}

				Matrix4 model = Math::Translate(Matrix4(1), particle->position);
				Matrix4 rotate = Math::Rotate(Matrix4(1), angle, Vector3(0.0f, 1.0f, 0.0f));
				Matrix4 scale = Math::Scale(Matrix4(1), Vector3(2.0f));
				Matrix4 mat_M = model * rotate * scale;

				shader->SetMat4("model", mat_M);
				shader->SetVec4("_Color", particle->color);

				RenderAPI::GetInstance()->Draw(VAO, 6, DrawType::OpenGLDrawArrays);
			}
		}
	}

	void ParticleSystem::SetTexture(const char* path)
	{
		if (textureID != 0)
			RenderAPI::GetInstance()->DeleteTexture(textureID);

		int width, height;
		textureID = RenderAPI::GetInstance()->LoadTexture(path, width, height);
	}

	void ParticleSystem::GenerateParticles()
	{
		for (unsigned int i = 0; i < particleNum; ++i)
			particles.push_back(new Particle());
	}

	unsigned int ParticleSystem::GetUnusedParticleIndex()
	{
		// 从上一个用过的向后找
		for (unsigned int i = lastUsedIndex; i < particleNum; ++i) 
		{
			if (particles[i]->life <= 0.0f)
			{
				lastUsedIndex = i;
				return i;
			}
		}
		// 没找到就从头找
		for (unsigned int i = 0; i < lastUsedIndex; ++i) 
		{
			if (particles[i]->life <= 0.0f)
			{
				lastUsedIndex = i;
				return i;
			}
		}
		// 都没找到就从头开始
		lastUsedIndex = 0;
		return 0;
	}

	void ParticleSystem::RespawnParticle(Particle* particle)
	{
		float xRandom = Math::RandomFloat(-0.5f, 0.5f);
		float yRandom = Math::RandomFloat(-0.5f, 0.5f);
		float zRandom = Math::RandomFloat(-0.5f, 0.5f);
		float rColor = Math::RandomFloat(0.5f, 1.0f);
		float gColor = Math::RandomFloat(0.5f, 1.0f);
		float bColor = Math::RandomFloat(0.5f, 1.0f);
		particle->position = GetTransform()->GetPosition() + Vector3(xRandom * offset.x, yRandom * offset.y, zRandom * offset.z);
		particle->color = Vector4(rColor, gColor, bColor, 1.0f);
		particle->life = lifeTime;
		particle->velocity = Math::GetRandomPerpendicular(moveDir) * 10;
	}
}