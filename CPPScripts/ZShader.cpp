#include "ZShader.h"

namespace ZXEngine
{
	Shader::Shader(const char* path)
	{
		ID = RenderEngine::LoadAndCompileShader(path);
	}

	unsigned int Shader::GetID()
	{
		return ID;
	}

	void Shader::Use()
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->UseShader(this->ID);
	}
	void Shader::SetBool(string name, bool value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderBool(this->ID, name, value);
	}
	void Shader::SetInt(string name, int value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderInt(this->ID, name, value);
	}
	void Shader::SetFloat(string name, float value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderFloat(this->ID, name, value);
	}
	void Shader::SetVec2(string name, vec2 value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderVec2(this->ID, name, value);
	}
	void Shader::SetVec2(string name, float x, float y)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderVec2(this->ID, name, x, y);
	}
	void Shader::SetVec3(string name, vec3 value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderVec3(this->ID, name, value);
	}
	void Shader::SetVec3(string name, float x, float y, float z)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderVec3(this->ID, name, x, y, z);
	}
	void Shader::SetVec4(string name, vec4 value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderVec4(this->ID, name, value);
	}
	void Shader::SetVec4(string name, float x, float y, float z, float w)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderVec4(this->ID, name, x, y, z, w);
	}
	void Shader::SetMat2(string name, mat2 value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderMat2(this->ID, name, value);
	}
	void Shader::SetMat3(string name, mat3 value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderMat3(this->ID, name, value);
	}
	void Shader::SetMat4(string name, mat4 value)
	{
		auto api = RenderEngine::mInstance->GetRenderAPI();
		api->SetShaderMat4(this->ID, name, value);
	}
}