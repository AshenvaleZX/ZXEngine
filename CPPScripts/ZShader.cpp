#include "ZShader.h"
#include "Resources.h"

namespace ZXEngine
{
	vector<ShaderInfo*> Shader::loadedShaders;

	Shader::Shader(const string& path)
	{
		name = Resources::GetAssetName(path);
		renderQueue = (int)RenderQueueType::Qpaque;

		for (auto shaderInfo : loadedShaders)
		{
			if (path == shaderInfo->path)
			{
				// 如果已加载过，直接引用
				info = shaderInfo;
				// 引用计数+1
				info->referenceCount++;
				break;
			}
		}
		// 如果没有加载过，执行真正的加载和编译
		if (info == nullptr)
		{
			info = RenderAPI::GetInstance()->LoadAndCompileShader(path.c_str());
			info->path = path;
			loadedShaders.push_back(info);
		}
	}

	Shader::~Shader()
	{
		info->referenceCount--;
		// 引用计数归零后执行真正的删除操作
		if (info->referenceCount == 0)
		{
			size_t pos = -1;
			for (size_t i = 0; i < loadedShaders.size(); i++)
			{
				if (loadedShaders[i]->ID == info->ID)
				{
					pos = i;
					break;
				}
			}
			if (pos != -1)
			{
				// 执行清理操作
				loadedShaders.erase(loadedShaders.begin() + pos);
				RenderAPI::GetInstance()->DeleteShaderProgram(info->ID);
				delete info;
			}
			else
			{
				Debug::LogWarning("Free shader failed: " + info->path);
			}
		}
	}

	unsigned int Shader::GetID()
	{
		return info->ID;
	}

	LightType Shader::GetLightType()
	{
		return info->lightType;
	}

	ShadowType Shader::GetShadowType()
	{
		return info->shadowType;
	}

	int Shader::GetRenderQueue()
	{
		return renderQueue;
	}

	void Shader::Use()
	{
		RenderAPI::GetInstance()->UseShader(info->ID);
	}
	void Shader::SetBool(string name, bool value)
	{
		RenderAPI::GetInstance()->SetShaderBool(info->ID, name, value);
	}
	void Shader::SetInt(string name, int value)
	{
		RenderAPI::GetInstance()->SetShaderInt(info->ID, name, value);
	}
	void Shader::SetFloat(string name, float value)
	{
		RenderAPI::GetInstance()->SetShaderFloat(info->ID, name, value);
	}
	void Shader::SetVec2(string name, Vector2 value)
	{
		RenderAPI::GetInstance()->SetShaderVec2(info->ID, name, value);
	}
	void Shader::SetVec2(string name, float x, float y)
	{
		RenderAPI::GetInstance()->SetShaderVec2(info->ID, name, x, y);
	}
	void Shader::SetVec3(string name, Vector3 value)
	{
		RenderAPI::GetInstance()->SetShaderVec3(info->ID, name, value);
	}
	void Shader::SetVec3(string name, float x, float y, float z)
	{
		RenderAPI::GetInstance()->SetShaderVec3(info->ID, name, x, y, z);
	}
	void Shader::SetVec4(string name, Vector4 value)
	{
		RenderAPI::GetInstance()->SetShaderVec4(info->ID, name, value);
	}
	void Shader::SetVec4(string name, float x, float y, float z, float w)
	{
		RenderAPI::GetInstance()->SetShaderVec4(info->ID, name, x, y, z, w);
	}
	void Shader::SetMat3(string name, Matrix3 value)
	{
		RenderAPI::GetInstance()->SetShaderMat3(info->ID, name, value);
	}
	void Shader::SetMat4(string name, Matrix4 value)
	{
		RenderAPI::GetInstance()->SetShaderMat4(info->ID, name, value);
	}
	void Shader::SetTexture(string name, unsigned int textureID, unsigned int idx)
	{
		RenderAPI::GetInstance()->SetShaderTexture(info->ID, name, textureID, idx);
	}
	void Shader::SetCubeMap(string name, unsigned int textureID, unsigned int idx)
	{
		RenderAPI::GetInstance()->SetShaderCubeMap(info->ID, name, textureID, idx);
	}
}