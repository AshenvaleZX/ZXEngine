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
		PrefabStruct* prefab = Resources::LoadPrefab("Prefabs/AssetPreviewCamera.zxprefab", true);
		cameraGO = new GameObject(prefab);
		cameraGO->GetComponent<Camera>()->cameraType = CameraType::EditorCamera;
		delete prefab;

		renderer = new MeshRenderer();
		renderer->LoadModel(Resources::GetAssetFullPath("Models/sphere.obj"));

		InitPreviewQuad();
		previewQuadShader = new Shader(Resources::GetAssetFullPath("Shaders/RenderTexture.zxshader", true).c_str());
		previewModelShader = new Shader(Resources::GetAssetFullPath("Shaders/ModelPreview.zxshader", true).c_str());

		FBOManager::GetInstance()->CreateFBO("AssetPreview", FrameBufferType::Normal, previewSize, previewSize);
	}

	EditorAssetPreviewer::~EditorAssetPreviewer()
	{
		delete cameraGO;
		delete renderer;
		delete previewQuadShader;
		delete previewModelShader;
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
		FBOManager::GetInstance()->SwitchFBO("AssetPreview");
		// ViewPort的Size和偏移量是基于当前的FBO，而不是基于当前的进程窗口，这一点很重要
		RenderAPI::GetInstance()->SetViewPort(previewSize, previewSize);
		RenderAPI::GetInstance()->EnableDepthTest(true);
		RenderAPI::GetInstance()->EnableDepthWrite(true);
		RenderAPI::GetInstance()->SetClearColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f));
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
		auto camera = cameraGO->GetComponent<Camera>();

		Matrix4 mat_M = GetModelMatrix();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		previewModelShader->Use();
		previewModelShader->SetMat4("model", mat_M);
		previewModelShader->SetMat4("view", mat_V);
		previewModelShader->SetMat4("projection", mat_P);
		previewModelShader->SetVec3("_Direction", Vector3(1.0f, 1.0f, -1.0f).Normalize());

		for (auto mesh : info->meshRenderer->meshes)
		{
			mesh->Use();
			RenderAPI::GetInstance()->Draw();
		}
	}

	void EditorAssetPreviewer::RenderToQuad()
	{
		RenderAPI::GetInstance()->SwitchFrameBuffer(0);
		RenderAPI::GetInstance()->SetViewPort(ProjectSetting::inspectorWidth, ProjectSetting::inspectorWidth, ProjectSetting::projectWidth, 0);
		RenderAPI::GetInstance()->EnableDepthTest(false);
		RenderAPI::GetInstance()->EnableDepthWrite(false);
		previewQuad->Use();
		previewQuadShader->Use();
		previewQuadShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("AssetPreview")->ColorBuffer, 0);
		RenderAPI::GetInstance()->Draw();
		RenderAPI::GetInstance()->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight, ProjectSetting::hierarchyWidth, ProjectSetting::projectHeight);
	}

	void EditorAssetPreviewer::UpdateModelScale(float delta)
	{
		scale += delta * scaleSensitivity;
		scale = Math::Clamp(scale, 0.1f, 10.0f);
	}

	void EditorAssetPreviewer::UpdateModelRotation(float xOffset, float yOffset)
	{
		yaw += xOffset * rotSensitivity;
		pitch += yOffset * rotSensitivity;
	}

	Matrix4 EditorAssetPreviewer::GetModelMatrix()
	{
		Matrix4 mat = Matrix4(scale);
		mat = Math::Rotate(mat, yaw, Vector3(0.0f, 1.0f, 0.0f));
		mat = Math::Rotate(mat, pitch, Vector3(1.0f, 0.0f, 0.0f));
		return mat;
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