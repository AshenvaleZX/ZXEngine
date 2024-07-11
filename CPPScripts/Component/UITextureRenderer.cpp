#include "UITextureRenderer.h"
#include "Transform.h"
#include "../StaticMesh.h"
#include "../ZShader.h"
#include "../Material.h"
#include "../Resources.h"
#include "../GlobalData.h"
#include "../GeometryGenerator.h"
#include "../EventManager.h"
#include "../GameObject.h"

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

	UITextureRenderer::UITextureRenderer()
	{
		mWindowResizeCallbackKey = EventManager::GetInstance()->AddEventHandler(EventType::WINDOW_RESIZE, std::bind(&UITextureRenderer::OnWindowResize, this, std::placeholders::_1));
	}

	UITextureRenderer::~UITextureRenderer()
	{
		if (texture != nullptr)
			delete texture;
		if (material != nullptr)
			delete material;
		if (textureMesh != nullptr)
			delete textureMesh;

		EventManager::GetInstance()->RemoveEventHandler(EventType::WINDOW_RESIZE, mWindowResizeCallbackKey);
	}

	ComponentType UITextureRenderer::GetInsType()
	{
		return ComponentType::UITextureRenderer;
	}

	void UITextureRenderer::Render(const Matrix4& matVP)
	{
		Matrix4 mat_M = GetTransform()->GetModelMatrix();
		material->Use();
		material->SetMatrix("ENGINE_Model", mat_M);

		if (!isScreenSpace)
			material->SetMatrix("ENGINE_Projection", matVP);

		RenderAPI::GetInstance()->Draw(textureMesh->VAO);
	}

	void UITextureRenderer::SetTexture(const string& path)
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

	void UITextureRenderer::OnWindowResize(const string& args)
	{
		if (isScreenSpace)
		{
			Matrix4 mat_P = Math::Orthographic(-static_cast<float>(GlobalData::srcWidth) / 2.0f, static_cast<float>(GlobalData::srcWidth) / 2.0f, -static_cast<float>(GlobalData::srcHeight) / 2.0f, static_cast<float>(GlobalData::srcHeight) / 2.0f);
			material->SetMatrix("ENGINE_Projection", mat_P, true);
		}
	}

	void UITextureRenderer::CreateRenderData()
	{
		if (texture == nullptr)
			return;

		isScreenSpace = GetTransform()->GetInsType() == ComponentType::RectTransform;

		material = new Material(shader);
		material->Use();
		material->SetTexture("_Texture", texture->GetID(), 0, true);

		if (isScreenSpace)
		{
			Matrix4 mat_P = Math::Orthographic(-static_cast<float>(GlobalData::srcWidth) / 2.0f, static_cast<float>(GlobalData::srcWidth) / 2.0f, -static_cast<float>(GlobalData::srcHeight) / 2.0f, static_cast<float>(GlobalData::srcHeight) / 2.0f);
			material->SetMatrix("ENGINE_Projection", mat_P, true);
		}

		float width = static_cast<float>(texture->width);
		float height = static_cast<float>(texture->height);

		if (isScreenSpace)
			textureMesh = GeometryGenerator::CreateQuad(width, height);
		else
			textureMesh = GeometryGenerator::CreateQuad(1.0f, 1.0f);
	}
}