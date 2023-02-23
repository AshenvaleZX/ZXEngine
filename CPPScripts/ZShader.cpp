#include "ZShader.h"
#include "Resources.h"

namespace ZXEngine
{
	vector<ShaderReference*> Shader::loadedShaders;

	Shader::Shader(const string& path)
	{
		name = Resources::GetAssetName(path);
		renderQueue = (int)RenderQueueType::Qpaque;

		for (auto shaderReference : loadedShaders)
		{
			if (path == shaderReference->path)
			{
				// 如果已加载过，直接引用
				reference = shaderReference;
				// 引用计数+1
				reference->referenceCount++;
				break;
			}
		}
		// 如果没有加载过，执行真正的加载和编译
		if (reference == nullptr)
		{
			reference = RenderAPI::GetInstance()->LoadAndCompileShader(path.c_str());
			reference->path = path;
			loadedShaders.push_back(reference);
		}
	}

	Shader::~Shader()
	{
		reference->referenceCount--;
		// 引用计数归零后执行真正的删除操作
		if (reference->referenceCount == 0)
		{
			size_t pos = -1;
			for (size_t i = 0; i < loadedShaders.size(); i++)
			{
				if (loadedShaders[i]->ID == reference->ID)
				{
					pos = i;
					break;
				}
			}
			if (pos != -1)
			{
				// 执行清理操作
				loadedShaders.erase(loadedShaders.begin() + pos);
				RenderAPI::GetInstance()->DeleteShaderProgram(reference->ID);
				delete reference;
			}
			else
			{
				Debug::LogWarning("Free shader failed: " + reference->path);
			}
		}
	}

	unsigned int Shader::GetID()
	{
		return reference->ID;
	}

	LightType Shader::GetLightType()
	{
		return reference->shaderInfo.lightType;
	}

	ShadowType Shader::GetShadowType()
	{
		return reference->shaderInfo.shadowType;
	}

	int Shader::GetRenderQueue()
	{
		return renderQueue;
	}

	void Shader::Use()
	{
		RenderAPI::GetInstance()->UseShader(reference->ID);
	}
	void Shader::SetBool(string name, bool value)
	{
		RenderAPI::GetInstance()->SetShaderBool(reference->ID, name, value);
	}
	void Shader::SetInt(string name, int value)
	{
		RenderAPI::GetInstance()->SetShaderInt(reference->ID, name, value);
	}
	void Shader::SetFloat(string name, float value)
	{
		RenderAPI::GetInstance()->SetShaderFloat(reference->ID, name, value);
	}
	void Shader::SetVec2(string name, Vector2 value)
	{
		RenderAPI::GetInstance()->SetShaderVec2(reference->ID, name, value);
	}
	void Shader::SetVec2(string name, float x, float y)
	{
		RenderAPI::GetInstance()->SetShaderVec2(reference->ID, name, x, y);
	}
	void Shader::SetVec3(string name, Vector3 value)
	{
		RenderAPI::GetInstance()->SetShaderVec3(reference->ID, name, value);
	}
	void Shader::SetVec3(string name, float x, float y, float z)
	{
		RenderAPI::GetInstance()->SetShaderVec3(reference->ID, name, x, y, z);
	}
	void Shader::SetVec4(string name, Vector4 value)
	{
		RenderAPI::GetInstance()->SetShaderVec4(reference->ID, name, value);
	}
	void Shader::SetVec4(string name, float x, float y, float z, float w)
	{
		RenderAPI::GetInstance()->SetShaderVec4(reference->ID, name, x, y, z, w);
	}
	void Shader::SetMat3(string name, Matrix3 value)
	{
		RenderAPI::GetInstance()->SetShaderMat3(reference->ID, name, value);
	}
	void Shader::SetMat4(string name, Matrix4 value)
	{
		RenderAPI::GetInstance()->SetShaderMat4(reference->ID, name, value);
	}
	void Shader::SetTexture(string name, unsigned int textureID, unsigned int idx)
	{
		RenderAPI::GetInstance()->SetShaderTexture(reference->ID, name, textureID, idx);
	}
	void Shader::SetCubeMap(string name, unsigned int textureID, unsigned int idx)
	{
		RenderAPI::GetInstance()->SetShaderCubeMap(reference->ID, name, textureID, idx);
	}
}