#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	struct KeyVector3
	{
		float mTime;
		Vector3 mValue;

		KeyVector3() : mTime(0.0f), mValue() {}
		KeyVector3(float time, const Vector3& value) : mTime(time), mValue(value) {}
	};

	struct KeyQuaternion
	{
		float mTime;
		Quaternion mValue;

		KeyQuaternion() : mTime(0.0f), mValue() {}
		KeyQuaternion(float time, const Quaternion& value) : mTime(time), mValue(value) {}
	};

	class NodeAnimation
	{
	public:
		// ����������ֶ�Ӧ
		string mName;

		// �ؼ�֡����
		vector<KeyVector3> mKeyScales;
		vector<KeyVector3> mKeyPositions;
		vector<KeyQuaternion> mKeyRotations;
	};
}