#include "UITextureRenderer.h"
#include "Transform.h"
#include "../StaticMesh.h"
#include "../ZShader.h"
#include "../Material.h"
#include "../Resources.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	Shader* UITextureRenderer::shader = nullptr;

	void UITextureRenderer::Init()
	{
		shader = new Shader(Resources::GetAssetFullPath("Shaders/UITextureRenderer.zxshader", true), FrameBufferType::Present);
	}

	ComponentType UITextureRenderer::GetType()
	{
		return ComponentType::UITextureRenderer;
	}

	UITextureRenderer::~UITextureRenderer()
	{
		if (texture != nullptr)
			delete texture;
		if (material != nullptr)
			delete material;
		if (textureMesh != nullptr)
			delete textureMesh;
	}

	ComponentType UITextureRenderer::GetInsType()
	{
		return ComponentType::UITextureRenderer;
	}

	void UITextureRenderer::Render()
	{
		Matrix4 mat_M = GetTransform()->GetModelMatrix();
		material->Use();
		material->SetMatrix("ENGINE_Model", mat_M);
		RenderAPI::GetInstance()->Draw(textureMesh->VAO);
	}

	void UITextureRenderer::SetTexture(const char* path)
	{
		if (texture != nullptr)
			delete texture;
		texture = new Texture(path);

		if (material != nullptr)
			delete material;
		if (textureMesh != nullptr)
			delete textureMesh;

		CreateRenderData();
	}

	void UITextureRenderer::CreateRenderData()
	{
		if (texture == nullptr)
			return;

		Matrix4 mat_P = Math::Orthographic(-static_cast<float>(GlobalData::srcWidth) / 2.0f, static_cast<float>(GlobalData::srcWidth) / 2.0f, -static_cast<float>(GlobalData::srcHeight) / 2.0f, static_cast<float>(GlobalData::srcHeight) / 2.0f);
		material = new Material(shader);
		material->Use();
		material->SetMatrix("ENGINE_Projection", mat_P, true);
		material->SetTexture("_Texture", texture->GetID(), 0, true);

		float width = (float)texture->width;
		float height = (float)texture->height;

		// 根据图片尺寸构建Mesh
		Vector3 points[4] =
		{
			Vector3(  width / 2 ,   height / 2 , 0),
			Vector3(  width / 2 , -(height / 2), 0),
			Vector3(-(width / 2),   height / 2 , 0),
			Vector3(-(width / 2), -(height / 2), 0),
		};
		// GLSL的纹理采样坐标系Y轴是朝下的，HLSL是朝上的
		Vector2 coords[4] =
		{
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 0),
			Vector2(0, 1),
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
		textureMesh = new StaticMesh(vertices, indices);
	}
}