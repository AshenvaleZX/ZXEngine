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
		// 切换到默认FBO，也就是直接渲染到输出的画面上
		RenderAPI::GetInstance()->SwitchFrameBuffer(0);
		// Todo: 其实这里可以不用每帧都设置，最开始设置一次就行
		RenderAPI::GetInstance()->EnableDepthTest(false);
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		// 清理上一帧数据
		RenderAPI::GetInstance()->ClearFrameBuffer();
		aeShader->Use();
		aeShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->mainFBO->ColorBuffer, 0);
		screenQuad->Use();
		RenderAPI::GetInstance()->Draw();
	}

	void RenderPassAfterEffectRendering::InitScreenQuad()
	{
		// 这里自己在代码里写一个Quad模型，就不从硬盘加载了
		vec3 points[4] =
		{
			vec3(1, 1, 0),
			vec3(1, -1, 0),
			vec3(-1, 1, 0),
			vec3(-1, -1, 0),
		};
		vec2 coords[4] =
		{
			vec2(1, 1),
			vec2(1, 0),
			vec2(0, 1),
			vec2(0, 0),
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
			vertex.Normal = vec3(1);
			vertex.Tangent = vec3(1);
			vertex.Bitangent = vec3(1);
			vertex.TexCoords = coords[i];
			vertices.push_back(vertex);
		}
		screenQuad = new Mesh(vertices, indices);
	}
}