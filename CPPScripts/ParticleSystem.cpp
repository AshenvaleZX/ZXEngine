#include "ParticleSystem.h"
#include "ParticleSystemManager.h"
#include "Time.h"
#include "RenderAPI.h"
#include "ZCamera.h"
#include "ZShader.h"
#include "Material.h"
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
		{
			delete particle->material;
			delete particle;
		}

		if (textureID != 0)
			RenderAPI::GetInstance()->DeleteTexture(textureID);
	}

	ComponentType ParticleSystem::GetInsType()
	{
		return ComponentType::ParticleSystem;
	}

	void ParticleSystem::Update()
	{
		// ���µ�ǰλ�ú��ƶ�����
		Vector3 curPos = GetTransform()->GetPosition();
		moveDir = (curPos - lastPos).GetNormalized();
		lastPos = curPos;

		// ����������ʱ����
		float interval = lifeTime / (float)particleNum;
		float curTime = Time::curTime;
		int genNum = 0;
		if (lastGenTime == 0)
		{
			// ��һ֡�ȳ�ʼ������
			lastGenTime = curTime;
		}
		else
		{
			// ��һ���������ӵ�ʱ��
			float delta = curTime - lastGenTime;
			// ������ʱ������������������ô�����������ʱ�䣬������һ֡Ӧ������������
			if (delta > interval)
				genNum = (int)(delta / interval);
			// �����һ֡���������ӣ���ô��������ʱ��
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

	void ParticleSystem::Render(Camera* camera)
	{
		Vector3 camPos = camera->GetTransform()->GetPosition();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		bool caculateAngle = true;
		float hypotenuse = 0;
		float angle = 0;
		for (auto particle : particles)
		{
			if (particle->life > 0)
			{
				if (caculateAngle)
				{
					hypotenuse = (float)sqrt(pow(camPos.x - particle->position.x, 2) + pow(camPos.z - particle->position.z, 2));
					if (camPos.z > particle->position.x)
					{
						angle = asin((camPos.x - particle->position.x) / hypotenuse);
					}
					else
					{
						angle = asin((particle->position.x - camPos.x) / hypotenuse);
					}
					caculateAngle = false;
				}

				Matrix4 model = Math::Translate(Matrix4(1), particle->position);
				Matrix4 rotate = Math::Rotate(Matrix4(1), angle, Vector3(0.0f, 1.0f, 0.0f));
				Matrix4 scale = Math::Scale(Matrix4(1), Vector3(2.0f));
				Matrix4 mat_M = model * rotate * scale;

				particle->material->Use();
				particle->material->SetMatrix("ENGINE_Model", mat_M);
				particle->material->SetMatrix("ENGINE_View", mat_V);
				particle->material->SetMatrix("ENGINE_Projection", mat_P);
				particle->material->SetVector("_Color", particle->color);

				RenderAPI::GetInstance()->Draw(particle->VAO);
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
		{
			auto particle = new Particle();
			RenderAPI::GetInstance()->GenerateParticleMesh(particle->VAO);
			particle->material = new Material(ParticleSystemManager::GetInstance()->shader);
			particle->material->Use();
			particle->material->SetTexture("_Sprite", textureID, 0, true);
			particles.push_back(particle);
		}
	}

	unsigned int ParticleSystem::GetUnusedParticleIndex()
	{
		// ����һ���ù��������
		for (unsigned int i = lastUsedIndex; i < particleNum; ++i) 
		{
			if (particles[i]->life <= 0.0f)
			{
				lastUsedIndex = i;
				return i;
			}
		}
		// û�ҵ��ʹ�ͷ��
		for (unsigned int i = 0; i < lastUsedIndex; ++i) 
		{
			if (particles[i]->life <= 0.0f)
			{
				lastUsedIndex = i;
				return i;
			}
		}
		// ��û�ҵ��ʹ�ͷ��ʼ
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