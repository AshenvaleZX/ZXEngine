#pragma once
#include "../pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class Light : public Component
	{
	public:
		static ComponentType GetType();
		static vector<Light*> GetAllLights();

	private:
		static vector<Light*> allLights;

	public:
		Vector3 color;
		float intensity = 0.0f;
		LightType type = LightType::None;
		float mDirectionalLightSpaceSize = 20.0f;

		Light();
		~Light();

		virtual ComponentType GetInsType();

		// 获取将顶点从世界空间转换到光源空间的矩阵
		Matrix4 GetLightMatrix();
		Matrix4 GetProjectionMatrix();
	};
}