#include "ZShader.h"
#include "RenderAPI.h"
#include "Resources.h"
#include "GlobalData.h"

namespace ZXEngine
{
	vector<ShaderReference*> Shader::loadedShaders;

	// �ڶ�����������ΪVulkan����Pipeline��ʱ����Ҫ���ú��ʵ�RenderPass�����֮��Vulkan�ĳ���Dynamic Rendering����������Ϳ���ȥ����
	Shader::Shader(const string& path, FrameBufferType type)
	{
		name = Resources::GetAssetName(path);

		for (auto shaderReference : loadedShaders)
		{
			if (path == shaderReference->path && type == shaderReference->targetFrameBufferType)
			{
				// ����Ѽ��ع���ֱ������
				reference = shaderReference;
				// ���ü���+1
				reference->referenceCount++;
				break;
			}
		}
		// ���û�м��ع���ִ�������ļ��غͱ���
		if (reference == nullptr)
		{
			reference = RenderAPI::GetInstance()->LoadAndSetUpShader(path, type);
			reference->path = path;
			loadedShaders.push_back(reference);
		}
	}

	Shader::Shader(const string& path, const string& shaderCode, FrameBufferType type)
	{
		name = Resources::GetAssetName(path);

		for (auto shaderReference : loadedShaders)
		{
			if (path == shaderReference->path)
			{
				reference = shaderReference;
				reference->referenceCount++;
				break;
			}
		}
		if (reference == nullptr)
		{
			reference = RenderAPI::GetInstance()->SetUpShader(path, shaderCode, type);
			reference->path = path;
			loadedShaders.push_back(reference);
		}
	}

	Shader::~Shader()
	{
		reference->referenceCount--;
		// ���ü��������ִ��������ɾ������
		if (reference->referenceCount == 0)
		{
			size_t pos = SIZE_MAX;
			for (size_t i = 0; i < loadedShaders.size(); i++)
			{
				if (loadedShaders[i]->ID == reference->ID)
				{
					pos = i;
					break;
				}
			}
			if (pos != SIZE_MAX)
			{
				// ִ���������
				loadedShaders.erase(loadedShaders.begin() + pos);
				RenderAPI::GetInstance()->DeleteShader(reference->ID);
				delete reference;
			}
			else
			{
				Debug::LogWarning("Free shader failed: " + reference->path);
			}
		}
	}

	unsigned int Shader::GetID() const
	{
		return reference->ID;
	}

	LightType Shader::GetLightType() const
	{
		return reference->shaderInfo.lightType;
	}

	ShadowType Shader::GetShadowType() const
	{
		return reference->shaderInfo.shadowType;
	}

	void Shader::Use() const
	{
		RenderAPI::GetInstance()->UseShader(reference->ID);
	}
}