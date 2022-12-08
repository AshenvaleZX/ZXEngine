#include "RenderPassAfterEffectRendering.h"
#include "RenderAPI.h"
#include "ZMesh.h"
#include "ZShader.h"
#include "Resources.h"
#include "FBOManager.h"

namespace ZXEngine
{
	RenderPassAfterEffectRendering::RenderPassAfterEffectRendering()
	{
		InitScreenQuad();
		aeShader = new Shader(Resources::GetAssetFullPath("Shaders/RenderTexture.zxshader").c_str());
		extractBrightShader = new Shader(Resources::GetAssetFullPath("Shaders/ExtractBrightArea.zxshader").c_str());
		gaussianBlurShader = new Shader(Resources::GetAssetFullPath("Shaders/GaussianBlur.zxshader").c_str());
		bloomBlendShader = new Shader(Resources::GetAssetFullPath("Shaders/BloomBlend.zxshader").c_str());
		FBOManager::GetInstance()->CreateFBO("ExtractBrightArea", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurVertical", FrameBufferType::Color);
		FBOManager::GetInstance()->CreateFBO("GaussianBlurHorizontal", FrameBufferType::Color);
	}

	void RenderPassAfterEffectRendering::Render(Camera* camera)
	{
		// 关闭深度测试和写入，整个后处理Pass都不需要关心深度
		RenderAPI::GetInstance()->EnableDepthTest(false);
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		// 整个后处理都在这个覆盖屏幕的四边形上渲染
		screenQuad->Use();

		// -------------------- 提取画面高亮部分 --------------------
		FBOManager::GetInstance()->SwitchFBO("ExtractBrightArea");
		RenderAPI::GetInstance()->ClearFrameBuffer();
		extractBrightShader->Use();
		extractBrightShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("Main")->ColorBuffer, 0);
		RenderAPI::GetInstance()->Draw();
		
		// -------------------- 高斯模糊高亮区域 --------------------
		// 反复模糊次数，次数越多越模糊，但是这个很影响性能，别开太高
		int blurTimes = 1;
		// 采样偏移距离，1代表偏移1像素，越大越模糊，这个不影响性能，但是太大了效果会不正常
		float texOffset = 3.0f;
		bool isHorizontal = true;
		string pingpongBuffer[2] = { "GaussianBlurHorizontal", "GaussianBlurVertical" };
		gaussianBlurShader->Use();
		gaussianBlurShader->SetFloat("_TexOffset", texOffset);
		for (int i = 0; i < blurTimes * 2; i++)
		{
			FBOManager::GetInstance()->SwitchFBO(pingpongBuffer[isHorizontal]);
			string colorFBO = i == 0 ? "ExtractBrightArea" : pingpongBuffer[!isHorizontal];
			gaussianBlurShader->SetBool("_Horizontal", isHorizontal);
			gaussianBlurShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO(colorFBO)->ColorBuffer, 0);
			RenderAPI::GetInstance()->Draw();
			isHorizontal = !isHorizontal;
		}

		// 切换到默认FBO，也就是直接渲染到输出的画面上
		FBOManager::GetInstance()->SwitchFBO("Screen");
		// 清理上一帧数据
		RenderAPI::GetInstance()->ClearFrameBuffer();
		bloomBlendShader->Use();
		bloomBlendShader->SetTexture("_BrightBlur", FBOManager::GetInstance()->GetFBO(pingpongBuffer[!isHorizontal])->ColorBuffer, 1);
		bloomBlendShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("Main")->ColorBuffer, 2);
		RenderAPI::GetInstance()->Draw();
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
		screenQuad = new Mesh(vertices, indices);
	}
}