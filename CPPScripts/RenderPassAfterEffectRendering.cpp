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
	}

	void RenderPassAfterEffectRendering::Render(Camera* camera)
	{
		// �л���Ĭ��FBO��Ҳ����ֱ����Ⱦ������Ļ�����
		FBOManager::GetInstance()->SwitchFBO("Screen");
		// �ر���Ȳ��Ժ�д��
		RenderAPI::GetInstance()->EnableDepthTest(false);
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		// ������һ֡����
		RenderAPI::GetInstance()->ClearFrameBuffer();
		aeShader->Use();
		aeShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("Main")->ColorBuffer, 0);
		screenQuad->Use();
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