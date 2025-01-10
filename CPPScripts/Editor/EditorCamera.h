#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Camera;
	class Transform;
	class GameObject;
	class EditorCamera
	{
	public:
		static EditorCamera* GetInstance();

	private:
		static EditorCamera* mInstance;

	public:
		Camera* mCamera;
		Transform* mCameraTrans;

		void Update();
		void MoveTo(const GameObject* target);
		AxisType GetCurAxis() const { return mCurAxis; }
		float GetRotationRadian() const { return mRotationRadian; }

	private:
		EditorCamera();
		~EditorCamera();

		GameObject* mCameraGO;

		// -------------------------------- Camera Operation --------------------------------
		float mSpeed = 25.0f;
		float mRate = 1.0f;
		float mSensitivity = 0.1f;
		float mLastX = 0.0f;
		float mLastY = 0.0f;
		bool mFirstMove = true;

		uint32_t mRigisterHandle = 0;
		void RigisterHandle(const string& args);
		uint32_t mUnrigisterHandle = 0;
		void UnrigisterHandle(const string& args) const;

		uint32_t mMoveForwardHandle = 0;
		void MoveForwardCallBack(const string& args);
		uint32_t mMoveBackHandle = 0;
		void MoveBackCallBack(const string& args);
		uint32_t mMoveLeftHandle = 0;
		void MoveLeftCallBack(const string& args);
		uint32_t mMoveRightHandle = 0;
		void MoveRightCallBack(const string& args);
		uint32_t mMoveMouseHandle = 0;
		void MoveMouseCallBack(const string& args);
		uint32_t mAccelerateHandle = 0;
		void AccelerateCallBack(const string& args);
		uint32_t mDecelerateHandle = 0;
		void DecelerateCallBack(const string& args);

		void MoveCamera(const Vector3& dir);
		void RotateCamera(float hOffset, float vOffset);

		bool mAutoMove = false;
		Vector3 mAutoMoveOriginPos;
		Vector3 mAutoMoveTargetPos;
		float mCurAutoMoveTime = 0.0f;
		const float mAutoMoveTime = 0.3f;

		void AutoMoveToTarget();

		// -------------------------------- Widget Operation --------------------------------
		bool mPressed = false;
		AxisType mCurAxis = AxisType::None;
		Vector2 mLastWidgetScreenPos;
		float mRotationRadian = 0.0f;

		uint32_t mClickHandle = 0;
		void ClickCallBack(const string& args);
		uint32_t mReleaseHandle = 0;
		void ReleaseCallBack(const string& args);
		uint32_t mOperateWidgetHandle = 0;
		void OperateWidgetCallBack(const string& args);
	};
}