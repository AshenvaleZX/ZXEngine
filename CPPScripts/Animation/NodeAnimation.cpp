#include "NodeAnimation.h"

namespace ZXEngine
{
	void NodeAnimation::Reset()
	{
		mCurScaleIdx = 0;
		mCurPositionIdx = 0;
		mCurRotationIdx = 0;
	}

	void NodeAnimation::UpdateCurFrame(float tick)
	{
		mCurFrame.mScale = GetScale(tick);
		mCurFrame.mPosition = GetPosition(tick);
		mCurFrame.mRotation = GetRotation(tick);
	}

	Vector3 NodeAnimation::GetScale(float tick)
	{
		if (mKeyScaleNum > 1)
		{
			if (tick >= mKeyScales[mCurScaleIdx + 1].mTime)
			{
				for (size_t i = mCurScaleIdx + 1; i < mKeyScaleNum - 1; i++)
				{
					if (tick < mKeyScales[i + 1].mTime)
					{
						mCurScaleIdx = i;
						break;
					}
				}
			}

			float start = mKeyScales[mCurScaleIdx].mTime;
			float end = mKeyScales[mCurScaleIdx + 1].mTime;
			float t = (tick - start) / (end - start);

			Vector3 from = mKeyScales[mCurScaleIdx].mValue;
			Vector3 to = mKeyScales[mCurScaleIdx + 1].mValue;

			return Math::Lerp(from, to, t);
		}
		else if (mKeyScaleNum == 1)
		{
			return mKeyScales[0].mValue;
		}
		else
		{
			return Vector3(1.0f, 1.0f, 1.0f);
		}
	}

	Vector3 NodeAnimation::GetPosition(float tick)
	{
		if (mKeyPositionNum > 1)
		{
			if (tick >= mKeyPositions[mCurPositionIdx + 1].mTime)
			{
				for (size_t i = mCurPositionIdx + 1; i < mKeyPositionNum - 1; i++)
				{
					if (tick < mKeyPositions[i + 1].mTime)
					{
						mCurPositionIdx = i;
						break;
					}
				}
			}

			float start = mKeyPositions[mCurPositionIdx].mTime;
			float end = mKeyPositions[mCurPositionIdx + 1].mTime;
			float t = (tick - start) / (end - start);

			Vector3 from = mKeyPositions[mCurPositionIdx].mValue;
			Vector3 to = mKeyPositions[mCurPositionIdx + 1].mValue;

			return Math::Lerp(from, to, t);
		}
		else if (mKeyPositionNum == 1)
		{
			return mKeyPositions[0].mValue;
		}
		else
		{
			return Vector3(0.0f, 0.0f, 0.0f);
		}
	}

	Quaternion NodeAnimation::GetRotation(float tick)
	{
		if (mKeyRotationNum > 1)
		{
			if (tick >= mKeyRotations[mCurRotationIdx + 1].mTime)
			{
				for (size_t i = mCurRotationIdx + 1; i < mKeyRotationNum - 1; i++)
				{
					if (tick < mKeyRotations[i + 1].mTime)
					{
						mCurRotationIdx = i;
						break;
					}
				}
			}

			float start = mKeyRotations[mCurRotationIdx].mTime;
			float end = mKeyRotations[mCurRotationIdx + 1].mTime;
			float t = (tick - start) / (end - start);

			Quaternion from = mKeyRotations[mCurRotationIdx].mValue;
			Quaternion to = mKeyRotations[mCurRotationIdx + 1].mValue;

			return Math::Slerp(from, to, t);
		}
		else if (mKeyRotationNum == 1)
		{
			return mKeyRotations[0].mValue;
		}
		else
		{
			return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}
}