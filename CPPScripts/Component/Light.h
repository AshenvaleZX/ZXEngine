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
		Vector3 mColor;
		float mIntensity = 0.0f;
		LightType mType = LightType::None;
		float mDirectionalLightSpaceSize = 20.0f;

		Light();
		~Light();

		virtual ComponentType GetInsType();

		// 获取将顶点从世界空间转换到光源空间的矩阵
		Matrix4 GetLightMatrix();
		Matrix4 GetProjectionMatrix();
	};

	ZXRef_StaticReflection
	(
		Light,
		ZXRef_BaseType(Component)
		ZXRef_Fields
		(
			ZXRef_Field(&Light::mColor),
			ZXRef_Field(&Light::mIntensity),
			ZXRef_Field(&Light::mType),
			ZXRef_Field(&Light::mDirectionalLightSpaceSize)
		)
	)
}