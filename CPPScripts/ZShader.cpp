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
			if (path == shaderReference->path)
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
			reference = RenderAPI::GetInstance()->LoadAndSetUpShader(path.c_str(), type);
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

	void Shader::Use()
	{
		RenderAPI::GetInstance()->UseShader(reference->ID);
	}
}