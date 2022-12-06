#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class Light : public Component
	{
	public:
		Vector3 color;
		float intensity;
		LightType type;

		Light();
		~Light();

		static ComponentType GetType();
		static vector<Light*> GetAllLights();

	private:
		static vector<Light*> allLights;
	};
}