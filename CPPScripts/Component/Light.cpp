#include "Light.h"
#include "Transform.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	vector<Light*> Light::allLights;
	vector<Light*> Light::GetAllLights()
	{
		return allLights;
	}

	ComponentType Light::GetType()
	{
		return ComponentType::Light;
	}

	Light::Light()
	{
		allLights.push_back(this);
	}

	Light::~Light()
	{
		auto l = std::find(allLights.begin(), allLights.end(), this);
		allLights.erase(l);
	}

	ComponentType Light::GetInsType()
	{
		return ComponentType::Light;
	}

	Matrix4 Light::GetLightMatrix()
	{
		// Óë Camera::GetViewMatrix Í¬Àí
		Matrix4 model = GetTransform()->GetModelMatrix();
		return Math::Inverse(model);
	}

	Matrix4 Light::GetProjectionMatrix()
	{
		return Math::Orthographic(-mDirectionalLightSpaceSize, mDirectionalLightSpaceSize, -mDirectionalLightSpaceSize, mDirectionalLightSpaceSize,
			GlobalData::shadowMapNearPlane, GlobalData::shadowMapFarPlane);
	}
}