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
		// �ر���Ȳ��Ժ�д�룬��������Pass������Ҫ�������
		RenderAPI::GetInstance()->EnableDepthTest(false);
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		// �������������������Ļ���ı�������Ⱦ
		screenQuad->Use();

		// -------------------- ��ȡ����������� --------------------
		FBOManager::GetInstance()->SwitchFBO("ExtractBrightArea");
		RenderAPI::GetInstance()->ClearFrameBuffer();
		extractBrightShader->Use();
		extractBrightShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("Main")->ColorBuffer, 0);
		RenderAPI::GetInstance()->Draw();
		
		// -------------------- ��˹ģ���������� --------------------
		// ����ģ������������Խ��Խģ�������������Ӱ�����ܣ���̫��
		int blurTimes = 1;
		// ����ƫ�ƾ��룬1����ƫ��1���أ�Խ��Խģ���������Ӱ�����ܣ�����̫����Ч���᲻����
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

		// �л���Ĭ��FBO��Ҳ����ֱ����Ⱦ������Ļ�����
		FBOManager::GetInstance()->SwitchFBO("Screen");
		// ������һ֡����
		RenderAPI::GetInstance()->ClearFrameBuffer();
		bloomBlendShader->Use();
		bloomBlendShader->SetTexture("_BrightBlur", FBOManager::GetInstance()->GetFBO(pingpongBuffer[!isHorizontal])->ColorBuffer, 1);
		bloomBlendShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("Main")->ColorBuffer, 2);
		RenderAPI::GetInstance()->Draw();
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
		screenQuad = new Mesh(vertices, indices);
	}
}