#include "UITextureRenderer.h"
#include "StaticMesh.h"
#include "ZShader.h"
#include "Resources.h"
#include "GlobalData.h"
#include "Transform.h"

namespace ZXEngine
{
	Shader* UITextureRenderer::shader = nullptr;

	void UITextureRenderer::Init()
	{
		shader = new Shader(Resources::GetAssetFullPath("Shaders/UITextureRenderer.zxshader", true), FrameBufferType::Normal);

		Matrix4 mat_P = Math::Orthographic(0.0f, (float)GlobalData::srcWidth, 0.0f, (float)GlobalData::srcHeight);
		shader->Use();
		shader->SetMat4("ENGINE_Projection", mat_P);
	}

	ComponentType UITextureRenderer::GetType()
	{
		return ComponentType::UITextureRenderer;
	}

	UITextureRenderer::~UITextureRenderer()
	{
		if (texture != nullptr)
			delete texture;
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
		shader->Use();
		shader->SetMat4("ENGINE_Model", mat_M);
		shader->SetTexture("_Texture", texture->GetID(), 0);
		RenderAPI::GetInstance()->Draw(textureMesh->VAO);
	}

	void UITextureRenderer::SetTexture(const char* path)
	{
		if (texture != nullptr)
			delete texture;
		texture = new Texture(path);

		if (textureMesh != nullptr)
			delete textureMesh;
		textureMesh = CreateTextureMesh();
	}

	StaticMesh* UITextureRenderer::CreateTextureMesh()
	{
		if (texture == nullptr)
			return nullptr;

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
		return new StaticMesh(vertices, indices);
	}
}