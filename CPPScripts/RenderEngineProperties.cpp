#include "RenderEngineProperties.h"
#include "CubeMap.h"
#include "Texture.h"
#include "Resources.h"
#include "Component/Light.h"
#include "Component/ZCamera.h"
#include "Component/Transform.h"
#include "Component/MeshRenderer.h"
#include "ProjectSetting.h"

namespace ZXEngine
{
	RenderEngineProperties* RenderEngineProperties::mInstance = nullptr;

	void RenderEngineProperties::Create()
	{
		mInstance = new RenderEngineProperties();
	}

	RenderEngineProperties* RenderEngineProperties::GetInstance()
	{
		return mInstance;
	}

	RenderEngineProperties::RenderEngineProperties()
	{
		emptyShadowMap = new Texture(Resources::GetAssetFullPath("Textures/white.png", true));
		SetEmptyShadowMap();

		vector<string> cubeMapPath;
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		cubeMapPath.push_back(Resources::GetAssetFullPath("Textures/white.png", true));
		emptyShadowCubeMap = new CubeMap(cubeMapPath);
		SetEmptyShadowCubeMap();
	}

	RenderEngineProperties::~RenderEngineProperties()
	{
		delete emptyShadowMap;
		delete emptyShadowCubeMap;
	}

	void RenderEngineProperties::SetLightMatrix(const Matrix4& mat)
	{
		lightMat = mat;
	}

	void RenderEngineProperties::SetLightProperties(const vector<Light*>& lights)
	{
		if (lights.empty())
			return;

		lightPos = lights[0]->GetTransform()->GetPosition();
		// 光源方向是指向光源的，所以要取反
		lightDir = -lights[0]->GetTransform()->GetForward();
		lightColor = lights[0]->color;
		lightIntensity = lights[0]->intensity;
	}

	void RenderEngineProperties::SetCameraProperties(Camera* camera)
	{
		matV = camera->GetViewMatrix();
		matV_Inv = Math::Inverse(matV);
		matP = camera->GetProjectionMatrix();
		matP_Inv = Math::Inverse(matP);
		camPos = camera->GetTransform()->GetPosition();
	}

	void RenderEngineProperties::SetRendererProperties(MeshRenderer* renderer)
	{
		matM = renderer->GetTransform()->GetModelMatrix();
		matM_Inv = Math::Inverse(matM);
	}

	void RenderEngineProperties::SetDepthMap(uint32_t id)
	{
		depthMap = id;
	}

	void RenderEngineProperties::SetEmptyShadowMap()
	{
		shadowMap = emptyShadowMap->GetID();
		isShadowMapBuffer = false;
	}

	void RenderEngineProperties::SetShadowMap(uint32_t id, bool isBuffer)
	{
		shadowMap = id;
		isShadowMapBuffer = isBuffer;
	}

	void RenderEngineProperties::SetEmptyShadowCubeMap()
	{
		shadowCubeMap = emptyShadowCubeMap->GetID();
		isShadowCubeMapBuffer = false;
	}

	void RenderEngineProperties::SetShadowCubeMap(uint32_t id, bool isBuffer)
	{
		// 支持Geometry Shader才会实际渲染ShadowCubeMap
		// Metal支持非GS的绘制
#ifndef ZX_API_METAL
		if (ProjectSetting::isSupportGeometryShader)
#endif
		{
			shadowCubeMap = id;
			isShadowCubeMapBuffer = isBuffer;
		}
	}
}