#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"

// ��������
struct Particle
{
	glm::vec3 Position;
	glm::vec3 Velocity;
	glm::vec4 Color;
	float Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(0.0f), Life(0.0f) { }
};

// ParticleGenerator�����һ���Particle���������ǵ���Ⱦ���ƶ�����������
class ParticleGenerator
{
public:
    // ���캯������ʼ�����ݺ�����
	ParticleGenerator(Shader shader, unsigned int textureID, int amount, glm::vec3 camPos) : shader(shader), textureID(textureID), amount(amount), camPos(camPos)
	{
        // Set up mesh and attribute properties
        unsigned int VBO;
        float particle_quad[] = {
            // pos            // tex coord
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        // Fill mesh buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
        // Set mesh attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        // Create this->amount default particle instances
        for (GLuint i = 0; i < amount; ++i)
            this->particles.push_back(Particle());
	}

    // ��������Particle��״̬
    void Update(float dt, glm::vec3 camPos, glm::vec3 Position, glm::vec3 Velocity, GLuint newParticles, glm::vec3 offset)
    {
        this->camPos = camPos;
        // Add new particles 
        for (int i = 0; i < newParticles; ++i)
        {
            int unusedParticle = this->firstUnusedParticle();
            this->respawnParticle(this->particles[unusedParticle], Position, Velocity, offset);
        }
        // Update all particles
        for (int i = 0; i < this->amount; ++i)
        {
            Particle& p = this->particles[i];
            p.Life -= dt; // reduce life
            if (p.Life > 0.0f)
            {	// particle is alive, thus update
                p.Position -= p.Velocity * dt;
                p.Color.a -= dt * 1;// 2.5;//�о�2.5������ʧ��̫����
            }
        }
    }

    // ��������Particle
    void Draw()
    {
        bool caculateAngle = true;
        float hypotenuse = 0;
        float angle = 0;
        // Use additive blending to give it a 'glow' effect
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        this->shader.use();
        for (Particle particle : this->particles)
        {
            if (particle.Life > 0.0f)
            {
                if (caculateAngle)
                {
                    hypotenuse = std::sqrt(std::pow(camPos.x - particle.Position.x, 2) + std::pow(camPos.z - particle.Position.z, 2));
                    if (camPos.z > particle.Position.x)
                    {
                        angle = glm::asin((camPos.x - particle.Position.x) / hypotenuse);
                    }
                    else
                    {
                        angle = glm::asin((particle.Position.x - camPos.x) / hypotenuse);
                    }
                    caculateAngle = false;
                }
                // ����任��ʱ�򣬼���˳���������ţ�����ת�����λ�ơ�����д�����˳��ͼ����˳���Ƿ��ŵģ�������дλ�ƣ���д��ת�����д����
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, particle.Position);
                model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
                // �����и�0.1��ƫ�ƣ�Ҫ����תǰ�ȼ��㣬������ת����������ӵ��ı�������Quad������ߣ��������м䣬ת��֮��λ�þ�ƫ��
                model = glm::translate(model, glm::vec3(-0.1f, -0.1f, 0.0f));
                model = glm::scale(model, glm::vec3(0.2f));
                this->shader.setMat4("model", model);
                this->shader.setVec4("color", particle.Color);
                glBindTexture(GL_TEXTURE_2D, this->textureID);
                glBindVertexArray(this->VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
        }
        // Don't forget to reset to default blending mode
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

private:
	std::vector<Particle> particles;
	int amount;
	Shader shader;
    glm::vec3 camPos;
	unsigned int textureID;
	unsigned int VAO;

    // Stores the index of the last particle used (for quick access to next dead particle)
    unsigned int lastUsedParticle = 0;
    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle()
    {
        // First search from last used particle, this will usually return almost instantly
        for (int i = lastUsedParticle; i < this->amount; ++i) {
            if (this->particles[i].Life <= 0.0f) {
                lastUsedParticle = i;
                return i;
            }
        }
        // Otherwise, do a linear search
        for (int i = 0; i < lastUsedParticle; ++i) {
            if (this->particles[i].Life <= 0.0f) {
                lastUsedParticle = i;
                return i;
            }
        }
        // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
        lastUsedParticle = 0;
        return 0;
    }

    void respawnParticle(Particle& particle, glm::vec3 Position, glm::vec3 Velocity, glm::vec3 offset)
    {
        float random = ((rand() % 100) - 50) / 10.0f;
        float rColor = 0.5 + ((rand() % 100) / 100.0f);
        particle.Position = Position + random * offset;
        particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
        particle.Life = 1.0f;
        particle.Velocity = Velocity * 0.1f;
    }
};