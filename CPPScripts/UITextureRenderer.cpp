#include "UITextureRenderer.h"
#include "ZMesh.h"
#include "ZShader.h"
#include "Resources.h"
#include "GlobalData.h"
#include "Transform.h"

namespace ZXEngine
{
	Shader* UITextureRenderer::shader = nullptr;

	void UITextureRenderer::Init()
	{
		shader = new Shader(Resources::GetAssetFullPath("Shaders/UITextureRenderer.zxshader").c_str());

		mat4 mat_P = glm::ortho(0.0f, (float)GlobalData::srcWidth, 0.0f, (float)GlobalData::srcHeight);
		shader->Use();
		shader->SetMat4("projection", mat_P);
	}

	ComponentType UITextureRenderer::GetType()
	{
		return ComponentType::T_UITextureRenderer;
	}

	void UITextureRenderer::Render()
	{
		mat4 mat_M = GetTransform()->GetModelMatrix();
		shader->Use();
		shader->SetMat4("model", mat_M);
		shader->SetTexture("_Texture", texture->GetID(), 0);
		textureMesh->Use();
		RenderAPI::GetInstance()->Draw();
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

	Mesh* UITextureRenderer::CreateTextureMesh()
	{
		if (texture == nullptr)
			return nullptr;

		float width = (float)texture->width;
		float height = (float)texture->height;

		// 根据图片尺寸构建Mesh
		vec3 points[4] =
		{
			vec3(  width / 2 ,   height / 2 , 0),
			vec3(  width / 2 , -(height / 2), 0),
			vec3(-(width / 2),   height / 2 , 0),
			vec3(-(width / 2), -(height / 2), 0),
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
		return new Mesh(vertices, indices);
	}
}