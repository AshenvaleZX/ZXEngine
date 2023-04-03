#include "RenderPassAfterEffectRendering.h"
#include "RenderAPI.h"
#include "StaticMesh.h"
#include "ZShader.h"
#include "Material.h"
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
		// 切换后处理渲染设置
		RenderAPI::GetInstance()->SetRenderState(renderState);
		// 整个后处理都在这个覆盖屏幕的四边形上渲染

		// 提取画面高亮部分
		string res1 = BlitExtractBrightArea("Main");
		
		// 高斯模糊高亮区域
		//string res2 = BlitGaussianBlur(res1, 1, 3.0f);
		
		// Kawase模糊高亮区域
		string res2 = BlitKawaseBlur(res1, 2, 2.0f);

		// 混合原图和高亮模糊
		string res3 = BlitBloomBlend("Main", res2, true);
	}

	void RenderPassAfterEffectRendering::CreateMaterial(string name, string path, FrameBufferType type)
	{
		if (aeMaterials.count(name) > 0)
		{
			Debug::LogError("Try to add an existing shader");
			return;
		}
		aeMaterials.insert(pair<string, Material*>(name, new Material(new Shader(Resources::GetAssetFullPath(path), type))));
	}

	Material* RenderPassAfterEffectRendering::GetMaterial(string name)
	{
		map<string, Material*>::iterator iter = aeMaterials.find(name);
		if (iter != aeMaterials.end())
			return iter->second;
		else
			return nullptr;
	}

	void RenderPassAfterEffectRendering::InitScreenQuad()
	{
		// 这里自己在代码里写一个Quad模型，就不从硬盘加载了
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
		// 这里是直接手写的NDC坐标，所以需要考虑不同API的NDC坐标系差异，目前工程里OpenGL和Vulkan都以逆时针为图元正面
		// 但是OpenGL的(-1,-1)在左下角，Y轴向上为正，Vulkan的(-1,-1)在左上角，Y轴向下为正，所以这里的顶点顺序要区分一下
		vector<unsigned int> indices =
		{
#ifdef ZX_API_OPENGL
			2, 3, 1,
			2, 1, 0,
#endif
#ifdef ZX_API_VULKAN
			3, 2, 0,
			3, 0, 1,
#endif
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
		CreateMaterial(ExtractBrightArea, "Shaders/ExtractBrightArea.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		if (!isFinal)
			FBOManager::GetInstance()->CreateFBO(ExtractBrightArea, FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitExtractBrightArea(string sourceFBO, bool isFinal)
	{
		FBOManager::GetInstance()->SwitchFBO(isFinal ? ScreenBuffer : ExtractBrightArea);
		auto material = GetMaterial(ExtractBrightArea);
		material->Use();
		material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(sourceFBO)->ColorBuffer, 0, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
		// 返回输出的FBO名字
		return ExtractBrightArea;
	}

	void RenderPassAfterEffectRendering::InitGaussianBlur(bool isFinal)
	{
		CreateMaterial(GaussianBlur, "Shaders/GaussianBlur.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurVertical", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurHorizontal", FrameBufferType::Color);
	}

	// blurTimes 反复模糊次数，次数越多越模糊，但是这个很影响性能，别开太高
	// texOffset 采样偏移距离，1代表偏移1像素，越大越模糊，这个不影响性能，但是太大了效果会不正常
	string RenderPassAfterEffectRendering::BlitGaussianBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal)
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
			material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0, true);
			RenderAPI::GetInstance()->Draw(screenQuad->VAO);
			RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
			isHorizontal = !isHorizontal;
		}
		// 返回最终输出的FBO名字
		return pingpongBuffer[!isHorizontal];
	}

	void RenderPassAfterEffectRendering::InitKawaseBlur(bool isFinal)
	{
		CreateMaterial(KawaseBlur, "Shaders/KawaseBlur.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("KawaseBlur0", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("KawaseBlur1", FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitKawaseBlur(string sourceFBO, int blurTimes, float texOffset, bool isFinal)
	{
		bool isSwitch = true;
		string pingpongBuffer[2] = { "KawaseBlur0", "KawaseBlur1" };
		auto material = GetMaterial(KawaseBlur);
		material->Use();
		material->SetScalar("_TexOffset", texOffset);
		for (int i = 0; i < blurTimes; i++)
		{
			FBOManager::GetInstance()->SwitchFBO(pingpongBuffer[isSwitch]);
			string colorFBO = i == 0 ? sourceFBO : pingpongBuffer[!isSwitch];
			material->SetScalar("_BlurTimes", i+1);
			material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0, true);
			RenderAPI::GetInstance()->Draw(screenQuad->VAO);
			RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
			isSwitch = !isSwitch;
		}
		// 返回最终输出的FBO名字
		return pingpongBuffer[!isSwitch];
	}

	void RenderPassAfterEffectRendering::InitBloomBlend(bool isFinal)
	{
		CreateMaterial(BloomBlend, "Shaders/BloomBlend.zxshader", isFinal ? FrameBufferType::Present : FrameBufferType::Color);
		if (!isFinal)
			FBOManager::GetInstance()->CreateFBO(BloomBlend, FrameBufferType::Color);
	}

	string RenderPassAfterEffectRendering::BlitBloomBlend(string originFBO, string blurFBO, bool isFinal)
	{
		FBOManager::GetInstance()->SwitchFBO(isFinal ? ScreenBuffer : BloomBlend);
		auto material = GetMaterial(BloomBlend);
		material->Use();
		material->SetTexture("_BrightBlur", FBOManager::GetInstance()->GetFBO(blurFBO)->ColorBuffer, 0, true);
		material->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(originFBO)->ColorBuffer, 1, true);
		RenderAPI::GetInstance()->Draw(screenQuad->VAO);
		RenderAPI::GetInstance()->GenerateDrawCommand(drawCommandID);
		return BloomBlend;
	}
}