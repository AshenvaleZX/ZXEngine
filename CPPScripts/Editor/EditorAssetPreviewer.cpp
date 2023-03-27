#include "EditorAssetPreviewer.h"
#include "EditorDataManager.h"
#include "../MeshRenderer.h"
#include "../GameObject.h"
#include "../ZCamera.h"
#include "../GlobalData.h"
#include "../ProjectSetting.h"
#include "../FBOManager.h"
#include "../CubeMap.h"
#include "../RenderStateSetting.h"
#include "../ZShader.h"
#include "../RenderEngineProperties.h"

namespace ZXEngine
{
	EditorAssetPreviewer::EditorAssetPreviewer()
	{
		PrefabStruct* prefab = Resources::LoadPrefab("Prefabs/AssetPreviewCamera.zxprefab", true);
		cameraGO = new GameObject(prefab);
		cameraGO->GetComponent<Camera>()->cameraType = CameraType::EditorCamera;
		delete prefab;

		materialSphere = new MeshRenderer();
		materialSphere->LoadModel(Resources::GetAssetFullPath("Models/sphere.obj", true));

		vector<string> cubeMapPath;
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		shadowCubeMap = new CubeMap(cubeMapPath);

		InitPreviewQuad();
		previewQuadShader = new Shader(Resources::GetAssetFullPath("Shaders/RenderTexture.zxshader", true), FrameBufferType::Present);
		previewModelShader = new Shader(Resources::GetAssetFullPath("Shaders/ModelPreview.zxshader", true), FrameBufferType::Normal);

		renderState = new RenderStateSetting();
		renderState->clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

		previewQuadRenderState = new RenderStateSetting();
		previewQuadRenderState->depthTest = false;
		previewQuadRenderState->depthWrite = false;

		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT | ZX_CLEAR_FRAME_BUFFER_STENCIL_BIT;
		clearInfo.color = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

		drawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand();

		FBOManager::GetInstance()->CreateFBO("AssetPreview", FrameBufferType::Normal, previewSize, previewSize);
	}

	EditorAssetPreviewer::~EditorAssetPreviewer()
	{
		delete cameraGO;
		delete shadowCubeMap;
		delete materialSphere;
		delete previewQuadShader;
		delete previewModelShader;
		delete renderState;
		delete previewQuadRenderState;
	}

	bool EditorAssetPreviewer::Check()
	{
		bool needPreview = false;
		auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
		if (curAsset != nullptr)
		{
			if (curAsset->type == AssetType::Material)
				needPreview = true;
			else if (curAsset->type == AssetType::Model)
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
		auto renderAPI = RenderAPI::GetInstance();
		// ViewPort的Size和偏移量是基于当前的FBO，而不是基于当前的进程窗口，这一点很重要
		renderAPI->SetViewPort(previewSize, previewSize);
		renderAPI->SetRenderState(renderState);
		// 清理上一帧数据，刷新背景颜色
		renderAPI->ClearFrameBuffer(clearInfo);

		auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
		auto curAssetInfo = EditorDataManager::GetInstance()->curAssetInfo;

		if (curAsset->type == AssetType::Material)
			RenderMaterialPreview(static_cast<AssetMaterialInfo*>(curAssetInfo));
		else if (curAsset->type == AssetType::Model)
			RenderModelPreview(static_cast<AssetModelInfo*>(curAssetInfo));

		renderAPI->GenerateDrawCommand(drawCommandID);

		RenderToQuad();
	}

	void EditorAssetPreviewer::RenderMaterialPreview(AssetMaterialInfo* info)
	{
		auto camera = cameraGO->GetComponent<Camera>();

		Matrix4 mat_M = GetModelMatrix();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		auto material = info->material;
		auto shader = material->shader;
		shader->Use();

		shader->SetMaterialProperties(material);

		auto engineProperties = RenderEngineProperties::GetInstance();
		engineProperties->matM = mat_M;
		engineProperties->matV = mat_V;
		engineProperties->matP = mat_P;

		// 固定参数模拟环境
		engineProperties->camPos = camera->GetTransform()->GetPosition();
		engineProperties->lightPos = Vector3(10.0f, 10.0f, -10.0f);
		engineProperties->lightDir = Vector3(1.0f, 1.0f, -1.0f).Normalize();
		engineProperties->lightColor = Vector3(1.0f, 1.0f, 1.0f);
		engineProperties->lightIntensity = 1.0f;
		engineProperties->shadowCubeMap = shadowCubeMap->GetID();

		shader->SetEngineProperties();

		materialSphere->Draw();
	}

	void EditorAssetPreviewer::RenderModelPreview(AssetModelInfo* info)
	{
		auto camera = cameraGO->GetComponent<Camera>();

		Matrix4 mat_M = GetModelMatrix();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		previewModelShader->Use();
		previewModelShader->SetMat4("ENGINE_Model", mat_M);
		previewModelShader->SetMat4("ENGINE_View", mat_V);
		previewModelShader->SetMat4("ENGINE_Projection", mat_P);
		previewModelShader->SetVec3("_Direction", Vector3(1.0f, 1.0f, -1.0f).Normalize());

		info->meshRenderer->Draw();
	}

	void EditorAssetPreviewer::RenderToQuad()
	{
		auto renderAPI = RenderAPI::GetInstance();
		renderAPI->SwitchFrameBuffer(UINT32_MAX);
		renderAPI->SetViewPort(ProjectSetting::inspectorWidth, ProjectSetting::inspectorWidth, ProjectSetting::srcWidth - ProjectSetting::inspectorWidth, 0);
		renderAPI->SetRenderState(previewQuadRenderState);
		previewQuadShader->Use();
		previewQuadShader->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("AssetPreview")->ColorBuffer, 0, true);
		renderAPI->Draw(previewQuad->VAO);
		renderAPI->GenerateDrawCommand(drawCommandID);
		renderAPI->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight, ProjectSetting::hierarchyWidth, ProjectSetting::projectHeight);
	}

	void EditorAssetPreviewer::Reset(float size)
	{
		yaw = 0.0f;
		pitch = 0.0f;
		scale = standardSize / size;
		minScale = scale * 0.1f;
		maxScale = scale * 10.0f;
		// 调整缩放敏感度，让不同大小的模型缩放速度的手感一直
		scaleSensitivity = scale * standardScaleSensitivity;
	}

	void EditorAssetPreviewer::UpdateModelScale(float delta)
	{
		scale += delta * scaleSensitivity;
		scale = Math::Clamp(scale, minScale, maxScale);
	}

	void EditorAssetPreviewer::UpdateModelRotation(float xOffset, float yOffset)
	{
		yaw -= xOffset * rotSensitivity;
		pitch -= yOffset * rotSensitivity;
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
		previewQuad = new StaticMesh(vertices, indices);
	}
}