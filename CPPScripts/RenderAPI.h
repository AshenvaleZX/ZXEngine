#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ZXEngine
{
	class RenderAPI
	{
	public:
		RenderAPI() {};
		~RenderAPI() {};

		static RenderAPI* Creat();

		// Shader…Ë÷√
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
	};
}