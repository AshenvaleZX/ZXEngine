#include "EditorAssetPreviewer.h"
#include "EditorDataManager.h"
#include "../MeshRenderer.h"
#include "../GameObject.h"
#include "../ZCamera.h"
#include "../GlobalData.h"
#include "../ProjectSetting.h"
#include "../FBOManager.h"

namespace ZXEngine
{
	EditorAssetPreviewer::EditorAssetPreviewer()
	{
		camera = new Camera();
		camera->Fov = 45.0f;
		camera->nearClipDis = 1.0f;
		camera->farClipDis = 100.0f;
		camera->cameraType = CameraType::EditorCamera;

		renderer = new MeshRenderer();
		renderer->LoadModel(Resources::GetAssetFullPath("Models/sphere.obj"));

		InitPreviewQuad();
		previewShader = new Shader(Resources::GetBuiltInAssetPath("Shaders/RenderTexture.zxshader").c_str());

		FBOManager::GetInstance()->CreateFBO("MaterialPreview", FrameBufferType::Normal, 256, 256);
	}

	EditorAssetPreviewer::~EditorAssetPreviewer()
	{
		delete camera;
		delete renderer;
	}

	bool EditorAssetPreviewer::Check()
	{
		bool needPreview = false;
		auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
		if (curAsset != nullptr)
		{
			if (curAsset->type == AssetType::AT_Material)
				needPreview = true;
			else if (curAsset->type == AssetType::AT_Model)
				needPreview = true;
		}

		if (needPreview)
		{
			ProjectSetting::inspectorHeight = GlobalData::srcHeight + ProjectSetting::projectHeight - ProjectSetting::inspectorWidth;
			return true;
		}
		else
		{
			ProjectSetting::inspectorHeight = GlobalData::srcHeight + ProjectSetting::projectHeight;
			return false;
		}
	}

	void EditorAssetPreviewer::Draw()
	{
		FBOManager::GetInstance()->SwitchFBO("MaterialPreview");
		RenderAPI::GetInstance()->SetClearColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
		RenderAPI::GetInstance()->ClearFrameBuffer();
		RenderAPI::GetInstance()->SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

		auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
		auto curAssetInfo = EditorDataManager::GetInstance()->curAssetInfo;

		if (curAsset->type == AssetType::AT_Material)
			RenderMaterialPreview(static_cast<AssetMaterialInfo*>(curAssetInfo));
		else if (curAsset->type == AssetType::AT_Model)
			RenderModelPreview(static_cast<AssetModelInfo*>(curAssetInfo));

		RenderToQuad();
	}

	void EditorAssetPreviewer::RenderMaterialPreview(AssetMaterialInfo* info)
	{

	}

	void EditorAssetPreviewer::RenderModelPreview(AssetModelInfo* info)
	{

	}

	void EditorAssetPreviewer::RenderToQuad()
	{
		RenderAPI::GetInstance()->SwitchFrameBuffer(0);
		RenderAPI::GetInstance()->SetViewPort(ProjectSetting::inspectorWidth, ProjectSetting::inspectorWidth, ProjectSetting::projectWidth, 0);
		previewQuad->Use();
		previewShader->Use();
		previewShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("MaterialPreview")->ColorBuffer, 0);
		RenderAPI::GetInstance()->Draw();
		RenderAPI::GetInstance()->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight, ProjectSetting::hierarchyWidth, ProjectSetting::projectHeight);
	}

	void EditorAssetPreviewer::InitPreviewQuad()
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
		previewQuad = new Mesh(vertices, indices);
	}
}