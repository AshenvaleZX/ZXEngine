#include "Light.h"

namespace ZXEngine
{
	vector<Light*> Light::allLights;

	Light::Light()
	{
		allLights.push_back(this);
	}

	Light::~Light()
	{
		auto l = std::find(allLights.begin(), allLights.end(), this);
		allLights.erase(l);
	}

	vector<Light*> Light::GetAllLights()
	{
		return allLights;
	}

	ComponentType Light::GetType()
	{
		return ComponentType::T_Light;
	}
}