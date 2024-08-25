#include "RenderPassAfterEffectRendering.h"
#include "RenderAPI.h"
#include "StaticMesh.h"
#include "ZShader.h"
#include "Material.h"
#include "Resources.h"
#include "FBOManager.h"
#include "RenderStateSetting.h"
#include "ProjectSetting.h"
#include "Component/ZCamera.h"
#include "GeometryGenerator.h"

const string ExtractBrightArea = "ExtractBrightArea";
const string GaussianBlur = "GaussianBlur";
const string BloomBlend = "BloomBlend";
const string CopyTexture = "CopyTexture";

#ifdef ZX_EDITOR
#define OutputBuffer "GameView"
#else
#define OutputBuffer ScreenBuffer
#endif

namespace ZXEngine
{
	RenderPassAfterEffectRendering::RenderPassAfterEffectRendering()
	{
		// InitGaussianBlur();
		InitKawaseBlur();
		InitExtractBrightArea();
		InitBloomBlend(true);
		InitCopy(true);

		screenQuad = GeometryGenerator::CreateScreenQuad();

		renderState = new RenderStateSetting();
		renderState->depthTest = false;
		renderState->depthWrite = false;

#ifdef ZX_EDITOR
		FBOManager::GetInstance()->CreateFBO("GameView", FrameBufferType::Color);
#endif
	}

	void RenderPassAfterEffectRendering::Render(Camera* camera)
	{
		// �л�������Ⱦ����
		RenderAPI::GetInstance()->SetRenderState(renderState);
		// �������������������Ļ���ı�������Ⱦ

		string finalFBO = "Forward";
		if (ProjectSetting::renderPipelineType == RenderPipelineType::Deferred)
			finalFBO = "Deferred";
		else if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
			finalFBO = "RayTracing";

		if (camera->enableAfterEffects)
		{
			// ��ȡ�����������
			string res1 = BlitExtractBrightArea(finalFBO);

			// ��˹ģ����������
			//string res2 = BlitGaussianBlur(res1, 1, 3.0f);

			// Kawaseģ����������
			string res2 = BlitKawaseBlur(res1, 2, 2.0f);

			// ���ԭͼ�͸���ģ��
			string res3 = BlitBloomBlend(finalFBO, res2, true);
		}
		else
		{
			// ֱ�����ԭBufferͼ��
			string res = BlitCopy(OutputBuffer, finalFBO, true);
		}
	}

	void RenderPassAfterEffectRendering::CreateCommand(const string& name)
	{
		if (aeCommands.count(name) > 0)
		{
			Debug::LogError("Try to add an existing command");
			return;
		}
		aeCommands.insert(pair(name, RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::AfterEffectRendering)));
	}

	uint32_t RenderPassAfterEffectRendering::GetCommand(const string& name)
	{
		map<string, uint32_t>::iterator iter = aeCommands.find(name);
		if (iter != aeCommands.end())
			return iter->second;
		else
		{
			Debug::LogError("Try to get an nonexistent command");
			return 0;
		}
	}

	void RenderPassAfterEffectRendering::CreateMaterial(const string& name, const string& path, FrameBufferType type)
	{
		if (aeMaterials.count(name) > 0)
		{
			Debug::LogError("Try to add an existing shader");
			return;
		}
		aeMaterials.insert(pair<string, Material*>(name, new Material(new Shader(Resources::GetAssetFullPath(path, true), type))));
	}

	Material* RenderPassAfterEffectRendering::GetMaterial(const string& name)
	{
		map<string, Material*>::iterator iter = aeMaterials.find(name);
		if (iter != aeMaterials.end())
			return iter->second;
		else
			return nullptr;
	}

	void RenderPassAfterEffectRendering::InitExtractBrightArea()
	{
		CreateCommand(ExtractBrightArea);
		CreateMaterial(ExtractBrightArea, "Shaders/PostProcess/ExtractBrightArea.zxshader", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO(ExtractBrightArea, FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitExtractBrightArea(const string& sourceFBO)
	{
		FBOManager::GetInstance()->SwitchFBO(ExtractBrightArea);
		auto material = GetMaterial(ExtractBrightArea);
		material->Use();
		material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(sourceFBO)->ColorBuffer, 0, false, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(GetCommand(ExtractBrightArea));
		// ���������FBO����
		return ExtractBrightArea;
	}

	void RenderPassAfterEffectRendering::InitGaussianBlur()
	{
		CreateMaterial(GaussianBlur, "Shaders/PostProcess/GaussianBlur.zxshader", FrameBufferType::Color);
		CreateCommand("GaussianBlurVertical");
		CreateCommand("GaussianBlurHorizontal");
		FBOManager::GetInstance()->CreateFBO("GaussianBlurVertical", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurHorizontal", FrameBufferType::Color);
	}

	// blurTimes ����ģ������������Խ��Խģ�������������Ӱ�����ܣ���̫��
	// texOffset ����ƫ�ƾ��룬1����ƫ��1���أ�Խ��Խģ���������Ӱ�����ܣ�����̫����Ч���᲻����
	string RenderPassAfterEffectRendering::BlitGaussianBlur(const string& sourceFBO, int blurTimes, float texOffset)
	{
		bool isHorizontal = true;
		string pingpongBuffer[2] = { "GaussianBlurHorizontal", "GaussianBlurVertical" };
		auto material = GetMaterial(GaussianBlur);
		material->Use();
		material->SetScalar("_TexOffset", texOffset);
		for (int i = 0; i < blurTimes * 2; i++)
		{
			FBOManager::GetInstance()->SwitchFBO(pingpongBuffer[isHorizontal]);
			string colorFBO = i == 0 ? sourceFBO : pingpongBuffer[!isHorizontal];
			material->SetScalar("_Horizontal", isHorizontal);
			material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0, false, true);
			RenderAPI::GetInstance()->Draw(screenQuad->VAO);
			RenderAPI::GetInstance()->GenerateDrawCommand(GetCommand(pingpongBuffer[isHorizontal]));
			isHorizontal = !isHorizontal;
		}
		// �������������FBO����
		return pingpongBuffer[!isHorizontal];
	}

	void RenderPassAfterEffectRendering::InitKawaseBlur()
	{
		CreateMaterial("KawaseBlur0", "Shaders/PostProcess/KawaseBlur.zxshader", FrameBufferType::Color);
		CreateMaterial("KawaseBlur1", "Shaders/PostProcess/KawaseBlur.zxshader", FrameBufferType::Color);
		CreateCommand("KawaseBlur0");
		CreateCommand("KawaseBlur1");
		FBOManager::GetInstance()->CreateFBO("KawaseBlur0", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("KawaseBlur1", FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitKawaseBlur(const string& sourceFBO, int blurTimes, float texOffset)
	{
		bool isSwitch = true;
		string pingpongBuffer[2] = { "KawaseBlur0", "KawaseBlur1" };
		for (int i = 0; i < blurTimes; i++)
		{
			FBOManager::GetInstance()->SwitchFBO(pingpongBuffer[isSwitch]);
			string colorFBO = i == 0 ? sourceFBO : pingpongBuffer[!isSwitch];

			auto material = GetMaterial(pingpongBuffer[isSwitch]);
			material->Use();
			material->SetScalar("_TexOffset", texOffset);
			material->SetScalar("_BlurTimes", i+1);
			material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0, false, true);

			RenderAPI::GetInstance()->Draw(screenQuad->VAO);
			RenderAPI::GetInstance()->GenerateDrawCommand(GetCommand(pingpongBuffer[isSwitch]));
			isSwitch = !isSwitch;
		}
		// �������������FBO����
		return pingpongBuffer[!isSwitch];
	}

	void RenderPassAfterEffectRendering::InitBloomBlend(bool isFinal)
	{
		CreateCommand(BloomBlend);
#ifdef ZX_EDITOR
		CreateMaterial(BloomBlend, "Shaders/PostProcess/BloomBlend.zxshader", FrameBufferType::Color);
#else
		CreateMaterial(BloomBlend, "Shaders/PostProcess/BloomBlend.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
#endif
		if (!isFinal)
			FBOManager::GetInstance()->CreateFBO(BloomBlend, FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitBloomBlend(const string& originFBO, const string& blurFBO, bool isFinal)
	{
		FBOManager::GetInstance()->SwitchFBO(isFinal ? OutputBuffer : BloomBlend);
		auto material = GetMaterial(BloomBlend);
		material->Use();
		material->SetTexture("_BrightBlur", FBOManager::GetInstance()->GetFBO(blurFBO)->ColorBuffer, 0, false, true);
		material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(originFBO)->ColorBuffer, 1, false, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(GetCommand(BloomBlend));
		return BloomBlend;
	}

	void RenderPassAfterEffectRendering::InitCopy(bool isFinal)
	{
		CreateCommand(CopyTexture);
#ifdef ZX_EDITOR
		CreateMaterial(CopyTexture, "Shaders/RenderTexture.zxshader", FrameBufferType::Color);
#else
		CreateMaterial(CopyTexture, "Shaders/RenderTexture.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
#endif
	}

	string RenderPassAfterEffectRendering::BlitCopy(const string& targetFBO, const string& sourceFBO, bool isFinal)
	{
		FBOManager::GetInstance()->SwitchFBO(targetFBO);
		auto material = GetMaterial(CopyTexture);
		material->Use();
		material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(sourceFBO)->ColorBuffer, 0, false, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(GetCommand(CopyTexture));
		return targetFBO;
	}
}