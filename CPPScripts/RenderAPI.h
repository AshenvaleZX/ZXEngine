#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	class RenderAPI
	{
	public:
		RenderAPI() {};
		~RenderAPI() {};

		static void Creat();
		static RenderAPI* GetInstance();

		// 渲染状态设置
		virtual void InitRenderSetting() = 0;
		virtual void EnableDepthWrite(bool enable) = 0;

		// 资源加载相关
		virtual unsigned int LoadTexture(const char* path) = 0;
		virtual unsigned int LoadCubeMap(vector<string> faces) = 0;
		virtual ShaderInfo LoadAndCompileShader(const char* path) = 0;
		virtual void CheckCompileErrors(unsigned int shader, std::string type) = 0;

		// DrawCall
		virtual void Draw() = 0;

		// Mesh设置
		virtual void SetMesh(unsigned int VAO, unsigned int size) = 0;
		virtual void SetUpMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices) = 0;

		// Shader设置
		virtual void UseShader(unsigned int ID) = 0;
		virtual void SetShaderBool(unsigned int ID, string name, bool value) = 0;
		virtual void SetShaderInt(unsigned int ID, string name, int value) = 0;
		virtual void SetShaderFloat(unsigned int ID, string name, float value) = 0;
		virtual void SetShaderVec2(unsigned int ID, string name, vec2 value) = 0;
		virtual void SetShaderVec2(unsigned int ID, string name, float x, float y) = 0;
		virtual void SetShaderVec3(unsigned int ID, string name, vec3 value) = 0;
		virtual void SetShaderVec3(unsigned int ID, string name, float x, float y, float z) = 0;
		virtual void SetShaderVec4(unsigned int ID, string name, vec4 value) = 0;
		virtual void SetShaderVec4(unsigned int ID, string name, float x, float y, float z, float w) = 0;
		virtual void SetShaderMat2(unsigned int ID, string name, mat2 value) = 0;
		virtual void SetShaderMat3(unsigned int ID, string name, mat3 value) = 0;
		virtual void SetShaderMat4(unsigned int ID, string name, mat4 value) = 0;
		virtual void SetShaderTexture(unsigned int ID, string name, unsigned int textureID, unsigned int idx) = 0;
		virtual void SetShaderCubeMap(unsigned int ID, string name, unsigned int textureID, unsigned int idx) = 0;

	private:
		static RenderAPI* mInstance;
	};
}