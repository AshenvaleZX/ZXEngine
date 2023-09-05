#include "EditorAssetPreviewer.h"
#include "EditorDataManager.h"
#include "../Component/ZCamera.h"
#include "../Component/MeshRenderer.h"
#include "../GameObject.h"
#include "../GlobalData.h"
#include "../ProjectSetting.h"
#include "../FBOManager.h"
#include "../CubeMap.h"
#include "../RenderStateSetting.h"
#include "../ZShader.h"
#include "../RenderEngineProperties.h"
#include "../Material.h"
#include "../StaticMesh.h"

namespace ZXEngine
{
	EditorAssetPreviewer::EditorAssetPreviewer()
	{
		PrefabStruct* prefab = Resources::LoadPrefab("Prefabs/AssetPreviewCamera.zxprefab", true);
		cameraGO = new GameObject(prefab);
		cameraGO->GetComponent<Camera>()->aspect = 1.0f;
		cameraGO->GetComponent<Camera>()->cameraType = CameraType::EditorCamera;
		delete prefab;

		materialSphere = new MeshRenderer();
		materialSphere->GenerateModel(GeometryType::Sphere);

		vector<string> cubeMapPath;
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		shadowCubeMap = new CubeMap(cubeMapPath);

		InitPreviewQuad();
		previewQuadMaterial = new Material(new Shader(Resources::GetAssetFullPath("Shaders/RenderTexture.zxshader", true), FrameBufferType::Present));
		previewModelMaterial = new Material(new Shader(Resources::GetAssetFullPath("Shaders/ModelPreview.zxshader", true), FrameBufferType::Normal));

		renderState = new RenderStateSetting();
		renderState->clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

		previewQuadRenderState = new RenderStateSetting();
		previewQuadRenderState->depthTest = false;
		previewQuadRenderState->depthWrite = false;

		drawQuadCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::AssetPreviewer);
		drawPreviewCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::AssetPreviewer);

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		clearInfo.color = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
		FBOManager::GetInstance()->CreateFBO("AssetPreview", FrameBufferType::Normal, clearInfo, previewSize, previewSize);
	}

	EditorAssetPreviewer::~EditorAssetPreviewer()
	{
		delete cameraGO;
		delete shadowCubeMap;
		delete materialSphere;
		delete previewQuadMaterial;
		delete previewModelMaterial;
		delete renderState;
		delete previewQuadRenderState;
	}

	bool EditorAssetPreviewer::Check()
	{
		bool needPreview = false;
		auto curGO = EditorDataManager::GetInstance()->selectedGO;
		if (curGO == nullptr)
		{
			auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
			if (curAsset != nullptr)
			{
				if (curAsset->type == AssetType::Material)
					needPreview = true;
				else if (curAsset->type == AssetType::Model)
					needPreview = true;
			}
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
		renderAPI->ClearFrameBuffer();

		auto curAsset = EditorDataManager::GetInstance()->selectedAsset;
		auto curAssetInfo = EditorDataManager::GetInstance()->curAssetInfo;

		if (curAsset->type == AssetType::Material)
			RenderMaterialPreview(static_cast<AssetMaterialInfo*>(curAssetInfo));
		else if (curAsset->type == AssetType::Model)
			RenderModelPreview(static_cast<AssetModelInfo*>(curAssetInfo));

		renderAPI->GenerateDrawCommand(drawPreviewCommandID);

		RenderToQuad();
	}

	void EditorAssetPreviewer::RenderMaterialPreview(AssetMaterialInfo* info)
	{
		auto camera = cameraGO->GetComponent<Camera>();

		Matrix4 mat_M = GetModelMatrix();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		auto material = info->material;
		material->Use();

		material->SetMaterialProperties();

		auto engineProperties = RenderEngineProperties::GetInstance();
		engineProperties->matM = mat_M;
		engineProperties->matV = mat_V;
		engineProperties->matP = mat_P;
		engineProperties->matM_Inv = Math::Inverse(mat_M);

		// 固定参数模拟环境
		engineProperties->camPos = camera->GetTransform()->GetPosition();
		engineProperties->lightPos = Vector3(10.0f, 10.0f, -10.0f);
		engineProperties->lightDir = Vector3(1.0f, 1.0f, -1.0f).GetNormalized();
		engineProperties->lightColor = Vector3(1.0f, 1.0f, 1.0f);
		engineProperties->lightIntensity = 1.0f;
		engineProperties->shadowCubeMap = shadowCubeMap->GetID();
		engineProperties->isShadowCubeMapBuffer = false;

		material->SetEngineProperties();

		materialSphere->Draw();
	}

	void EditorAssetPreviewer::RenderModelPreview(AssetModelInfo* info)
	{
		auto camera = cameraGO->GetComponent<Camera>();

		Matrix4 mat_M = GetModelMatrix();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();
		Matrix4 mat_M_Inv = Math::Inverse(mat_M);

		previewModelMaterial->Use();
		previewModelMaterial->SetMatrix("ENGINE_Model", mat_M);
		previewModelMaterial->SetMatrix("ENGINE_View", mat_V);
		previewModelMaterial->SetMatrix("ENGINE_Projection", mat_P);
		previewModelMaterial->SetMatrix("ENGINE_Model_Inv", mat_M_Inv);
		previewModelMaterial->SetVector("_Direction", Vector3(1.0f, 1.0f, -1.0f).GetNormalized());

		info->meshRenderer->Draw();
	}

	void EditorAssetPreviewer::RenderToQuad()
	{
		auto renderAPI = RenderAPI::GetInstance();
		renderAPI->SwitchFrameBuffer(UINT32_MAX);
		renderAPI->SetViewPort(ProjectSetting::inspectorWidth, ProjectSetting::inspectorWidth, ProjectSetting::srcWidth - ProjectSetting::inspectorWidth, 0);
		renderAPI->SetRenderState(previewQuadRenderState);
		previewQuadMaterial->Use();
		previewQuadMaterial->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("AssetPreview")->ColorBuffer, 0, false, true);
		renderAPI->Draw(previewQuad->VAO);
		renderAPI->GenerateDrawCommand(drawQuadCommandID);
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
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
			Vector2(1, 1),
			Vector2(1, 0),
			Vector2(0, 1),
			Vector2(0, 0),
#else
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 0),
			Vector2(0, 1),
#endif
		};
		vector<Vertex> vertices;
		// 这里是直接手写的NDC坐标，所以需要考虑不同API的NDC坐标系差异，目前工程里都是以逆时针为图元正面
		// OpenGL的(-1,-1,-1)在左下近点，Vulkan的(-1,-1,-1)在左上近点，DirectX12的(-1,-1,-1)在左上远点
		// 因为OpenGL和DirectX12在Y轴和Z轴都是反的，所以负负得正，用同一个顶点索引顺序
		vector<unsigned int> indices =
		{
#if defined(ZX_API_OPENGL) || defined(ZX_API_D3D12)
			2, 3, 1,
			2, 1, 0,
#else
			3, 2, 0,
			3, 0, 1,
#endif
		};
		for (unsigned int i = 0; i < 4; i++)
		{
			Vertex vertex;
			vertex.Position = points[i];
			vertex.TexCoords = coords[i];
			vertices.push_back(vertex);
		}
		previewQuad = new StaticMesh(vertices, indices);
	}
}