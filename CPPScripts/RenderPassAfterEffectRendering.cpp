#include "RenderPassAfterEffectRendering.h"
#include "RenderAPI.h"
#include "StaticMesh.h"
#include "ZShader.h"
#include "Resources.h"
#include "FBOManager.h"
#include "RenderStateSetting.h"

namespace ZXEngine
{
	RenderPassAfterEffectRendering::RenderPassAfterEffectRendering()
	{
		InitScreenQuad();
		//InitGaussianBlur();
		InitKawaseBlur();
		InitExtractBrightArea();
		InitBloomBlend(true);

		renderState = new RenderStateSetting();
		renderState->depthTest = false;
		renderState->depthWrite = false;

		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT;

		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand();
	}

	void RenderPassAfterEffectRendering::Render(Camera* camera)
	{
		// �л�������Ⱦ����
		RenderAPI::GetInstance()->SetRenderState(renderState);
		// �������������������Ļ���ı�������Ⱦ

		// ��ȡ�����������
		string res1 = BlitExtractBrightArea("Main");
		
		// ��˹ģ����������
		//string res2 = BlitGaussianBlur(res1, 1, 3.0f);
		
		// Kawaseģ����������
		string res2 = BlitKawaseBlur(res1, 2, 2.0f);

		// ���ԭͼ�͸���ģ��
		string res3 = BlitBloomBlend("Main", res2, true);
	}

	void RenderPassAfterEffectRendering::CreateShader(string name, string path, FrameBufferType type)
	{
		if (aeShaders.count(name) > 0)
		{
			Debug::LogError("Try to add an existing shader");
			return;
		}
		aeShaders.insert(pair<string, Shader*>(name, new Shader(Resources::GetAssetFullPath(path), type)));
	}

	Shader* RenderPassAfterEffectRendering::GetShader(string name)
	{
		map<string, Shader*>::iterator iter = aeShaders.find(name);
		if (iter != aeShaders.end())
			return iter->second;
		else
			return nullptr;
	}

	void RenderPassAfterEffectRendering::InitScreenQuad()
	{
		// �����Լ��ڴ�����дһ��Quadģ�ͣ��Ͳ���Ӳ�̼�����
		Vector3 points[4] =
		{
			Vector3(1, 1, 0),
			Vector3(1, -1, 0),
			Vector3(-1, 1, 0),
			Vector3(-1, -1, 0),
		};
		Vector2 coords[4] =
		{
			Vector2(1, 1),
			Vector2(1, 0),
			Vector2(0, 1),
			Vector2(0, 0),
		};
		vector<Vertex> vertices;
		vector<unsigned int> indices =
		{
			2, 3, 1,
			2, 1, 0,
		};
		for (unsigned int i = 0; i < 4; i++)
		{
			Vertex vertex;
			vertex.Position = points[i];
			vertex.Normal = Vector3(1);
			vertex.Tangent = Vector3(1);
			vertex.Bitangent = Vector3(1);
			vertex.TexCoords = coords[i];
			vertices.push_back(vertex);
		}
		screenQuad = new StaticMesh(vertices, indices);
	}

	void RenderPassAfterEffectRendering::InitExtractBrightArea(bool isFinal)
	{
		CreateShader(ExtractBrightArea, "Shaders/ExtractBrightArea.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		if (!isFinal)
			FBOManager::GetInstance()->CreateFBO(ExtractBrightArea, FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitExtractBrightArea(string sourceFBO, bool isFinal)
	{
		FBOManager::GetInstance()->SwitchFBO(isFinal ? ScreenBuffer : ExtractBrightArea);
		auto shader = GetShader(ExtractBrightArea);
		shader->Use();
		shader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(sourceFBO)->ColorBuffer, 0, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
		// ���������FBO����
		return ExtractBrightArea;
	}

	void RenderPassAfterEffectRendering::InitGaussianBlur(bool isFinal)
	{
		CreateShader(GaussianBlur, "Shaders/GaussianBlur.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurVertical", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurHorizontal", FrameBufferType::Color);
	}

	// blurTimes ����ģ������������Խ��Խģ�������������Ӱ�����ܣ���̫��
	// texOffset ����ƫ�ƾ��룬1����ƫ��1���أ�Խ��Խģ���������Ӱ�����ܣ�����̫����Ч���᲻����
	string RenderPassAfterEffectRendering::BlitGaussianBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal)
	{
		bool isHorizontal = true;
		string pingpongBuffer[2] = { "GaussianBlurHorizontal", "GaussianBlurVertical" };
		auto shader = GetShader(GaussianBlur);
		shader->Use();
		shader->SetFloat("_TexOffset", texOffset);
		for (int i = 0; i < blurTimes * 2; i++)
		{
			FBOManager::GetInstance()->SwitchFBO(pingpongBuffer[isHorizontal]);
			string colorFBO = i == 0 ? sourceFBO : pingpongBuffer[!isHorizontal];
			shader->SetBool("_Horizontal", isHorizontal);
			shader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0, true);
			RenderAPI::GetInstance()->Draw(screenQuad->VAO);
			RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
			isHorizontal = !isHorizontal;
		}
		// �������������FBO����
		return pingpongBuffer[!isHorizontal];
	}

	void RenderPassAfterEffectRendering::InitKawaseBlur(bool isFinal)
	{
		CreateShader(KawaseBlur, "Shaders/KawaseBlur.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("KawaseBlur0", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("KawaseBlur1", FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitKawaseBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal)
	{
		bool isSwitch = true;
		string pingpongBuffer[2] = { "KawaseBlur0", "KawaseBlur1" };
		auto shader = GetShader(KawaseBlur);
		shader->Use();
		shader->SetFloat("_TexOffset", texOffset);
		for (int i = 0; i < blurTimes; i++)
		{
			FBOManager::GetInstance()->SwitchFBO(pingpongBuffer[isSwitch]);
			string colorFBO = i == 0 ? sourceFBO : pingpongBuffer[!isSwitch];
			shader->SetInt("_BlurTimes", i+1);
			shader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0, true);
			RenderAPI::GetInstance()->Draw(screenQuad->VAO);
			RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
			isSwitch = !isSwitch;
		}
		// �������������FBO����
		return pingpongBuffer[!isSwitch];
	}

	void RenderPassAfterEffectRendering::InitBloomBlend(bool isFinal)
	{
		CreateShader(BloomBlend, "Shaders/BloomBlend.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		if (!isFinal)
			FBOManager::GetInstance()->CreateFBO(BloomBlend, FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitBloomBlend(string originFBO, string blurFBO, bool isFinal)
	{
		FBOManager::GetInstance()->SwitchFBO(isFinal ? ScreenBuffer : BloomBlend);
		auto shader = GetShader(BloomBlend);
		shader->Use();
		shader->SetTexture("_BrightBlur", FBOManager::GetInstance()->GetFBO(blurFBO)->ColorBuffer, 0, true);
		shader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(originFBO)->ColorBuffer, 1, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
		return BloomBlend;
	}
}