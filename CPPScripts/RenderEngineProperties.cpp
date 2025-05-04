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
		// 如果当前环境无法渲染ShadowCubeMap可以禁用此函数，或者判断一下条件直接return，这样会用一个空的ShadowCubeMap，相当于点光源阴影失效
		// 以前由于只能通过几何着色器渲染ShadowCubeMap，而有些环境(比如苹果)可能不支持几何着色器，所以可能有无法渲染ShadowCubeMap的情况出现
		// 但是现在无论是否支持几何着色器都可以渲染ShadowCubeMap了，所以应该不存在无法渲染ShadowCubeMap的情况了
		shadowCubeMap = id;
		isShadowCubeMapBuffer = isBuffer;
	}
}