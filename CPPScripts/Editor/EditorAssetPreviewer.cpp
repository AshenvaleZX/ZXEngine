#include "EditorAssetPreviewer.h"
#include "EditorDataManager.h"
#include "../MeshRenderer.h"
#include "../GameObject.h"
#include "../ZCamera.h"
#include "../GlobalData.h"
#include "../ProjectSetting.h"
#include "../FBOManager.h"
#include "../CubeMap.h"

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
		previewQuadShader = new Shader(Resources::GetAssetFullPath("Shaders/RenderTexture.zxshader", true).c_str());
		previewModelShader = new Shader(Resources::GetAssetFullPath("Shaders/ModelPreview.zxshader", true).c_str());

		FBOManager::GetInstance()->CreateFBO("AssetPreview", FrameBufferType::Normal, previewSize, previewSize);
	}

	EditorAssetPreviewer::~EditorAssetPreviewer()
	{
		delete cameraGO;
		delete shadowCubeMap;
		delete materialSphere;
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
		// ViewPort��Size��ƫ�����ǻ��ڵ�ǰ��FBO�������ǻ��ڵ�ǰ�Ľ��̴��ڣ���һ�����Ҫ
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
		auto camera = cameraGO->GetComponent<Camera>();

		Matrix4 mat_M = GetModelMatrix();
		Matrix4 mat_V = camera->GetViewMatrix();
		Matrix4 mat_P = camera->GetProjectionMatrix();

		auto material = info->material;
		auto shader = material->shader;

		shader->Use();
		shader->SetMat4("model", mat_M);
		shader->SetMat4("view", mat_V);
		shader->SetMat4("projection", mat_P);

		unsigned int textureNum = material->textures.size();
		for (unsigned int i = 0; i < textureNum; i++)
		{
			shader->SetTexture(material->textures[i].first, material->textures[i].second->GetID(), i);
		}

		// �̶���Դ
		if (shader->GetLightType() == LightType::Directional)
		{
			shader->SetVec3("viewPos", camera->GetTransform()->position);
			shader->SetVec3("dirLight.direction", Vector3(1.0f, 1.0f, -1.0f).Normalize());
			shader->SetVec3("dirLight.color", Vector3(1.0f, 1.0f, 1.0f));
			shader->SetFloat("dirLight.intensity", 1.0f);
		}
		else if (shader->GetLightType() == LightType::Point)
		{
			shader->SetVec3("viewPos", camera->GetTransform()->position);
			shader->SetVec3("pointLight.position", Vector3(10.0f, 10.0f, -10.0f));
			shader->SetVec3("pointLight.color", Vector3(1.0f, 1.0f, 1.0f));
			shader->SetFloat("pointLight.intensity", 1.0f);
		}

		// ����Ӱ������Ӱ��ͼ���ף��������ڵ�
		if (shader->GetShadowType() == ShadowType::Directional)
		{

		}
		else if (shader->GetShadowType() == ShadowType::Point)
		{
			shader->SetCubeMap("_DepthCubeMap", shadowCubeMap->GetID(), textureNum);
			shader->SetFloat("_FarPlane", 100.0f);
		}

		materialSphere->Draw();
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

		info->meshRenderer->Draw();
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

	void EditorAssetPreviewer::Reset(float size)
	{
		yaw = 0.0f;
		pitch = 0.0f;
		scale = standardSize / size;
		minScale = scale * 0.1f;
		maxScale = scale * 10.0f;
		// �����������жȣ��ò�ͬ��С��ģ�������ٶȵ��ָ�һֱ
		scaleSensitivity = scale * standardScaleSensitivity;
	}

	void EditorAssetPreviewer::UpdateModelScale(float delta)
	{
		scale += delta * scaleSensitivity;
		scale = Math::Clamp(scale, minScale, maxScale);
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
		previewQuad = new Mesh(vertices, indices);
	}
}