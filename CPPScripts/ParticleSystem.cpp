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
		return ComponentType::T_ParticleSystem;
	}

	ParticleSystem::ParticleSystem() : offset(0), velocity(0)
	{
		ParticleSystemManager::GetInstance()->AddParticleSystem(this);
	}

	ParticleSystem::~ParticleSystem()
	{
		ParticleSystemManager::GetInstance()->RemoveParticleSystem(this);
	}

	void ParticleSystem::Update()
	{
		// 粒子生产的时间间隔
		float interval = lifeTime / (float)particleNum;
		long long curTime = Time::curTime_milli;
		int genNum = 0;
		if (lastGenTime == 0)
		{
			// 第一帧先初始化数据
			lastGenTime = curTime;
		}
		else
		{
			// 上一次生产粒子的时间
			float delta = (float)(curTime - lastGenTime) / 1000.0f;
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
			Particle& p = particles[i];
			p.life -= dt;
			if (p.life > 0.0f)
			{
				p.position -= p.velocity * dt;
				p.color.a -= dt * 1;
			}
		}
	}

	void ParticleSystem::Render(Shader* shader, vec3 viewPos)
	{
		unsigned int VAO = ParticleSystemManager::GetInstance()->VAO;
		shader->SetTexture("_Sprite", textureID, 0);
		bool caculateAngle = true;
		float hypotenuse = 0;
		float angle = 0;
		//int count = 0;
		for (auto& particle : particles)
		{
			if (particle.life > 0)
			{
				//if (caculateAngle)
				{
					hypotenuse = (float)sqrt(pow(viewPos.x - particle.position.x, 2) + pow(viewPos.z - particle.position.z, 2));
					if (viewPos.z > particle.position.x)
					{
						angle = asin((viewPos.x - particle.position.x) / hypotenuse);
					}
					else
					{
						angle = asin((particle.position.x - viewPos.x) / hypotenuse);
					}
					caculateAngle = false;
				}

				mat4 model = mat4(1);
				model = Math::Translate(model, particle.position);
				mat4 rotate = GetTransform()->GetRotationMatrix();
				rotate = Math::Rotate(rotate, angle, vec3(0.0f, 1.0f, 0.0f));
				mat4 scale = GetTransform()->GetScaleMatrix();
				scale = Math::Scale(scale, vec3(2.0f));
				//mat4 mat_M = model * rotate * scale;
				mat4 mat_M = model * scale;;

				shader->SetMat4("model", mat_M);
				shader->SetVec4("_Color", particle.color);

				//count++;
				RenderAPI::GetInstance()->Draw(VAO, 6, DrawType::OpenGLDrawArrays);
			}
		}
		//Debug::Log("Count " + to_string(count));
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
			particles.push_back(Particle());
	}

	unsigned int ParticleSystem::GetUnusedParticleIndex()
	{
		// 从上一个用过的向后找
		for (unsigned int i = lastUsedIndex; i < particleNum; ++i) 
		{
			if (particles[i].life <= 0.0f)
			{
				lastUsedIndex = i;
				return i;
			}
		}
		// 没找到就从头找
		for (unsigned int i = 0; i < lastUsedIndex; ++i) 
		{
			if (particles[i].life <= 0.0f)
			{
				lastUsedIndex = i;
				return i;
			}
		}
		// 都没找到就从头开始
		lastUsedIndex = 0;
		return 0;
	}

	void ParticleSystem::RespawnParticle(Particle& particle)
	{
		float random = (float)((rand() % 100) - 50) / 100.0f;
		float rColor = 0.5f + ((rand() % 100) / 100.0f);
		particle.position = GetTransform()->position + random * offset;
		//particle.color = vec4(rColor, rColor, rColor, 1.0f);
		particle.color = vec4(1.0f);
		particle.life = lifeTime;
		particle.velocity = velocity;
	}
}