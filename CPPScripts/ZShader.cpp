#include "ZShader.h"

namespace ZXEngine
{
	Shader::Shader(const char* path)
	{
		ID = RenderAPI::Get()->LoadAndCompileShader(path);
	}

	unsigned int Shader::GetID()
	{
		return ID;
	}

	void Shader::Use()
	{
		RenderAPI::Get()->UseShader(this->ID);
	}
	void Shader::SetBool(string name, bool value)
	{
		RenderAPI::Get()->SetShaderBool(this->ID, name, value);
	}
	void Shader::SetInt(string name, int value)
	{
		RenderAPI::Get()->SetShaderInt(this->ID, name, value);
	}
	void Shader::SetFloat(string name, float value)
	{
		RenderAPI::Get()->SetShaderFloat(this->ID, name, value);
	}
	void Shader::SetVec2(string name, vec2 value)
	{
		RenderAPI::Get()->SetShaderVec2(this->ID, name, value);
	}
	void Shader::SetVec2(string name, float x, float y)
	{
		RenderAPI::Get()->SetShaderVec2(this->ID, name, x, y);
	}
	void Shader::SetVec3(string name, vec3 value)
	{
		RenderAPI::Get()->SetShaderVec3(this->ID, name, value);
	}
	void Shader::SetVec3(string name, float x, float y, float z)
	{
		RenderAPI::Get()->SetShaderVec3(this->ID, name, x, y, z);
	}
	void Shader::SetVec4(string name, vec4 value)
	{
		RenderAPI::Get()->SetShaderVec4(this->ID, name, value);
	}
	void Shader::SetVec4(string name, float x, float y, float z, float w)
	{
		RenderAPI::Get()->SetShaderVec4(this->ID, name, x, y, z, w);
	}
	void Shader::SetMat2(string name, mat2 value)
	{
		RenderAPI::Get()->SetShaderMat2(this->ID, name, value);
	}
	void Shader::SetMat3(string name, mat3 value)
	{
		RenderAPI::Get()->SetShaderMat3(this->ID, name, value);
	}
	void Shader::SetMat4(string name, mat4 value)
	{
		RenderAPI::Get()->SetShaderMat4(this->ID, name, value);
	}
}