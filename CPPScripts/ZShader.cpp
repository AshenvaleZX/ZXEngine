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
		renderQueue = (int)RenderQueueType::Qpaque;

		// Vulkan����Ϊ�漰VkPipeline��һЩ�����Դ�������⣬���紴��VkDescriptorSet��VkDescriptorPool��Ҫ��ǰ���÷�������
		// ����������ʱ���Ȳ��ö�����ʹ���һ��Shaderʵ����
		if (GlobalData::shaderReferenceOptimalEnabled)
		{
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
		else
		{
			reference = RenderAPI::GetInstance()->LoadAndSetUpShader(path.c_str(), type);
			reference->path = path;
		}
	}

	Shader::~Shader()
	{
		if (GlobalData::shaderReferenceOptimalEnabled)
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
		else
		{
			RenderAPI::GetInstance()->DeleteShader(reference->ID);
			delete reference;
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
}