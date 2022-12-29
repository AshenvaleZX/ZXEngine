#include "Light.h"

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
}