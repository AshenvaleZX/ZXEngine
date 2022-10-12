#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class RenderAPIOpenGL : public RenderAPI
	{
	public:
		RenderAPIOpenGL() {};
		~RenderAPIOpenGL() {};

		// Shader…Ë÷√
		virtual void UseShader(unsigned int ID);
		virtual void SetShaderBool(unsigned int ID, string name, bool value);
		virtual void SetShaderInt(unsigned int ID, string name, int value);
		virtual void SetShaderFloat(unsigned int ID, string name, float value);
		virtual void SetShaderVec2(unsigned int ID, string name, vec2 value);
		virtual void SetShaderVec2(unsigned int ID, string name, float x, float y);
		virtual void SetShaderVec3(unsigned int ID, string name, vec3 value);
		virtual void SetShaderVec3(unsigned int ID, string name, float x, float y, float z);
		virtual void SetShaderVec4(unsigned int ID, string name, vec4 value);
		virtual void SetShaderVec4(unsigned int ID, string name, float x, float y, float z, float w);
		virtual void SetShaderMat2(unsigned int ID, string name, mat2 value);
		virtual void SetShaderMat3(unsigned int ID, string name, mat3 value);
		virtual void SetShaderMat4(unsigned int ID, string name, mat4 value);
	};
}