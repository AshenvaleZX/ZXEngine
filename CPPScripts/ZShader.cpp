#include "ZShader.h"

namespace ZXEngine
{
	Shader::Shader(const char* path)
	{
		renderQueue = RenderQueueType::Qpaque;
		ID = RenderAPI::GetInstance()->LoadAndCompileShader(path);
	}

	unsigned int Shader::GetID()
	{
		return ID;
	}

	int Shader::GetRenderQueue()
	{
		return renderQueue;
	}

	void Shader::Use()
	{
		RenderAPI::GetInstance()->UseShader(this->ID);
	}
	void Shader::SetBool(string name, bool value)
	{
		RenderAPI::GetInstance()->SetShaderBool(this->ID, name, value);
	}
	void Shader::SetInt(string name, int value)
	{
		RenderAPI::GetInstance()->SetShaderInt(this->ID, name, value);
	}
	void Shader::SetFloat(string name, float value)
	{
		RenderAPI::GetInstance()->SetShaderFloat(this->ID, name, value);
	}
	void Shader::SetVec2(string name, vec2 value)
	{
		RenderAPI::GetInstance()->SetShaderVec2(this->ID, name, value);
	}
	void Shader::SetVec2(string name, float x, float y)
	{
		RenderAPI::GetInstance()->SetShaderVec2(this->ID, name, x, y);
	}
	void Shader::SetVec3(string name, vec3 value)
	{
		RenderAPI::GetInstance()->SetShaderVec3(this->ID, name, value);
	}
	void Shader::SetVec3(string name, float x, float y, float z)
	{
		RenderAPI::GetInstance()->SetShaderVec3(this->ID, name, x, y, z);
	}
	void Shader::SetVec4(string name, vec4 value)
	{
		RenderAPI::GetInstance()->SetShaderVec4(this->ID, name, value);
	}
	void Shader::SetVec4(string name, float x, float y, float z, float w)
	{
		RenderAPI::GetInstance()->SetShaderVec4(this->ID, name, x, y, z, w);
	}
	void Shader::SetMat2(string name, mat2 value)
	{
		RenderAPI::GetInstance()->SetShaderMat2(this->ID, name, value);
	}
	void Shader::SetMat3(string name, mat3 value)
	{
		RenderAPI::GetInstance()->SetShaderMat3(this->ID, name, value);
	}
	void Shader::SetMat4(string name, mat4 value)
	{
		RenderAPI::GetInstance()->SetShaderMat4(this->ID, name, value);
	}
	void Shader::SetTexture(string name, unsigned int textureID, unsigned int idx)
	{
		RenderAPI::GetInstance()->SetShaderTexture(this->ID, name, textureID, idx);
	}
}