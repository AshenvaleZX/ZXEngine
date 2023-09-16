#pragma once
#include "../pubh.h"
#include "../PublicStruct.h"

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
		// 与骨骼的名字对应
		string mName;

		// 关键帧数据
		size_t mKeyScaleNum = 0;
		vector<KeyVector3> mKeyScales;
		size_t mKeyPositionNum = 0;
		vector<KeyVector3> mKeyPositions;
		size_t mKeyRotationNum = 0;
		vector<KeyQuaternion> mKeyRotations;

		KeyFrame mCurFrame;

		void Reset();
		void UpdateCurFrame(float tick);

	private:
		size_t mCurScaleIdx = 0;
		size_t mCurPositionIdx = 0;
		size_t mCurRotationIdx = 0;

		Vector3 GetScale(float tick);
		Vector3 GetPosition(float tick);
		Quaternion GetRotation(float tick);
	};
}