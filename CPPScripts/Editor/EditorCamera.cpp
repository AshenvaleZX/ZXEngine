#include "EditorCamera.h"
#include "EditorDataManager.h"
#include "../GameObject.h"
#include "../EventManager.h"
#include "../Time.h"
#include "../Utils.h"
#include "../Input/InputManager.h"

namespace ZXEngine
{
	EditorCamera* EditorCamera::mInstance = nullptr;

	EditorCamera* EditorCamera::GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new EditorCamera();
		}
		return mInstance;
	}

	EditorCamera::EditorCamera()
	{
		mCameraGO = new GameObject();
		mCamera = mCameraGO->AddComponent<Camera>();
		mCameraTrans = mCameraGO->AddComponent<Transform>();

		mCameraTrans->SetPosition(-20.0f, 20.0f, -20.0f);
		mCameraTrans->SetEulerAngles(45.0f, 45.0f, 0.0f);

		mCamera->mFarClipDis = 1000.0f;
		mCamera->mCameraType = CameraType::EditorCamera;

		auto eventMgr = EventManager::GetInstance();
		mClickHandle = eventMgr->AddEditorEventHandler(EventType::MOUSE_BUTTON_1_DOWN, std::bind(&EditorCamera::ClickCallBack, this, std::placeholders::_1));
		mReleaseHandle = eventMgr->AddEditorEventHandler(EventType::MOUSE_BUTTON_1_UP, std::bind(&EditorCamera::ReleaseCallBack, this, std::placeholders::_1));
		mRigisterHandle = eventMgr->AddEditorEventHandler(EventType::MOUSE_BUTTON_2_DOWN, std::bind(&EditorCamera::RigisterHandle, this, std::placeholders::_1));
		mUnrigisterHandle = eventMgr->AddEditorEventHandler(EventType::MOUSE_BUTTON_2_UP, std::bind(&EditorCamera::UnrigisterHandle, this, std::placeholders::_1));
	}

	EditorCamera::~EditorCamera()
	{
		delete mCameraGO;
	}

	void EditorCamera::RigisterHandle(const string& args)
	{
		mFirstMove = true;
		InputManager::GetInstance()->ShowCursor(false);

		auto eventMgr = EventManager::GetInstance();
		mMoveForwardHandle = eventMgr->AddEditorEventHandler(EventType::KEY_W_PRESS,      std::bind(&EditorCamera::MoveForwardCallBack, this, std::placeholders::_1));
		mMoveBackHandle    = eventMgr->AddEditorEventHandler(EventType::KEY_S_PRESS,      std::bind(&EditorCamera::MoveBackCallBack,    this, std::placeholders::_1));
		mMoveLeftHandle    = eventMgr->AddEditorEventHandler(EventType::KEY_A_PRESS,      std::bind(&EditorCamera::MoveLeftCallBack,    this, std::placeholders::_1));
		mMoveRightHandle   = eventMgr->AddEditorEventHandler(EventType::KEY_D_PRESS,      std::bind(&EditorCamera::MoveRightCallBack,   this, std::placeholders::_1));
		mMoveMouseHandle   = eventMgr->AddEditorEventHandler(EventType::UPDATE_MOUSE_POS, std::bind(&EditorCamera::MoveMouseCallBack,   this, std::placeholders::_1));
		mAccelerateHandle  = eventMgr->AddEditorEventHandler(EventType::KEY_LSHIFT_DOWN,  std::bind(&EditorCamera::AccelerateCallBack,  this, std::placeholders::_1));
		mDecelerateHandle  = eventMgr->AddEditorEventHandler(EventType::KEY_LSHIFT_UP,    std::bind(&EditorCamera::DecelerateCallBack,  this, std::placeholders::_1));
	}

	void EditorCamera::UnrigisterHandle(const string& args) const
	{
		InputManager::GetInstance()->ShowCursor(true);

		auto eventMgr = EventManager::GetInstance();
		eventMgr->RemoveEditorEventHandler(EventType::KEY_W_PRESS,      mMoveForwardHandle);
		eventMgr->RemoveEditorEventHandler(EventType::KEY_S_PRESS,      mMoveBackHandle);
		eventMgr->RemoveEditorEventHandler(EventType::KEY_A_PRESS,      mMoveLeftHandle);
		eventMgr->RemoveEditorEventHandler(EventType::KEY_D_PRESS,      mMoveRightHandle);
		eventMgr->RemoveEditorEventHandler(EventType::UPDATE_MOUSE_POS, mMoveMouseHandle);
		eventMgr->RemoveEditorEventHandler(EventType::KEY_LSHIFT_DOWN,  mAccelerateHandle);
		eventMgr->RemoveEditorEventHandler(EventType::KEY_LSHIFT_UP,    mDecelerateHandle);
	}

	void EditorCamera::ClickCallBack(const string& args)
	{
		auto dataMgr = EditorDataManager::GetInstance();
		if (dataMgr->isGameView || dataMgr->selectedGO == nullptr)
			return;

		auto argList = Utils::StringSplit(args, '|');

		mLastWidgetScreenPos.x = std::stof(argList[0]);
		mLastWidgetScreenPos.y = std::stof(argList[1]);

		auto ray = mCamera->ScreenPointToRay(mLastWidgetScreenPos);

		mCurAxis = AxisType::None;

		if (dataMgr->mCurTransType == TransformType::Rotation)
		{
			float minDis = FLT_MAX;
			PhysZ::RayHitInfo hitInfo;
			auto& widgetColliders = dataMgr->mTransRotWidgetColliders;
			for (auto& iter : widgetColliders)
			{
				if (iter.second->IntersectRay(ray, hitInfo))
				{
					if (hitInfo.distance < minDis)
					{
						minDis = hitInfo.distance;
						mCurAxis = iter.first;
					}
				}
			}
		}
		else
		{
			auto& widgetColliders = dataMgr->mCurTransType == TransformType::Position ? dataMgr->mTransPosWidgetColliders : dataMgr->mTransScaleWidgetColliders;
			for (auto& iter : widgetColliders)
			{
				if (iter.second->IntersectRay(ray))
				{
					mCurAxis = iter.first;
				}
			}
		}

		mOperateWidgetHandle = EventManager::GetInstance()->AddEditorEventHandler(EventType::UPDATE_MOUSE_POS, std::bind(&EditorCamera::OperateWidgetCallBack, this, std::placeholders::_1));
	}

	void EditorCamera::ReleaseCallBack(const string& args) const
	{
		EventManager::GetInstance()->RemoveEditorEventHandler(EventType::UPDATE_MOUSE_POS, mOperateWidgetHandle);
	}

	void EditorCamera::OperateWidgetCallBack(const string& args)
	{
		if (mCurAxis == AxisType::None)
			return;

		auto dataMgr = EditorDataManager::GetInstance();
		auto go = dataMgr->selectedGO;
		if (go == nullptr)
			return;

		auto argList = Utils::StringSplit(args, '|');

		Vector2 widgetScreenPos;
		widgetScreenPos.x = std::stof(argList[0]);
		widgetScreenPos.y = std::stof(argList[1]);
		// 当前帧拖拽Widget在屏幕上产生的位移
		Vector2 deltaScreenPos = widgetScreenPos - mLastWidgetScreenPos;

		auto goTrans = go->GetComponent<Transform>();

		if (dataMgr->mCurTransType == TransformType::Rotation)
		{
			Vector3 goPos = goTrans->GetPosition();
			Vector3 camPos = mCameraTrans->GetPosition();
			Vector3 goDir = goPos - camPos;

			Vector2 centerScreenPos = mCamera->WorldToScreenPoint(goPos);
			Vector2 centerDir = centerScreenPos - mLastWidgetScreenPos;
			centerDir.Normalize();

			Vector2 clockWiseDir(centerDir.y, -centerDir.x);
			float moveDis = Math::Dot(deltaScreenPos, clockWiseDir);

			Vector3 axis;
			switch (mCurAxis)
			{
			case ZXEngine::AxisType::X:
				axis = goTrans->GetRight();
				break;
			case ZXEngine::AxisType::Y:
				axis = goTrans->GetUp();
				break;
			case ZXEngine::AxisType::Z:
				axis = goTrans->GetForward();
				break;
			}

			float angle = moveDis * 0.01f;
			if (Math::Dot(axis, goDir) > 0.0f)
				angle = -angle;

			goTrans->Rotate(axis, angle);
		}
		else
		{
			auto& widgetOrientations = dataMgr->mCurTransType == TransformType::Position ? dataMgr->mTransPosWidgetOrientations : dataMgr->mTransScaleWidgetOrientations;
			Vector3 headPos = widgetOrientations[mCurAxis].first->GetComponent<Transform>()->GetPosition();
			Vector3 tailPos = widgetOrientations[mCurAxis].second->GetComponent<Transform>()->GetPosition();

			Vector2 headScreenPos = mCamera->WorldToScreenPoint(headPos);
			Vector2 tailScreenPos = mCamera->WorldToScreenPoint(tailPos);

			// 当前选中的轴的3D方向投影到屏幕上后的2D方向
			Vector2 axisDir = headScreenPos - tailScreenPos;
			axisDir.Normalize();

			// 将鼠标在屏幕上的位移距离投影到轴的方向上
			float dis = Math::Dot(deltaScreenPos, axisDir);

			// 移动速率，离镜头越远移动速率越快
			float transRate = (mCameraTrans->GetPosition() - goTrans->GetPosition()).GetMagnitude();

			if (dataMgr->mCurTransType == TransformType::Position)
			{
				transRate *= 0.001f;

				Vector3 offset;
				switch (mCurAxis)
				{
				case ZXEngine::AxisType::X:
					offset = goTrans->GetRight();
					break;
				case ZXEngine::AxisType::Y:
					offset = goTrans->GetUp();
					break;
				case ZXEngine::AxisType::Z:
					offset = goTrans->GetForward();
					break;
				}
				offset *= (dis * transRate);

				goTrans->Translate(offset);
			}
			else if (dataMgr->mCurTransType == TransformType::Scale)
			{
				transRate *= 0.0001f;

				Vector3 scale(1.0f);
				switch (mCurAxis)
				{
				case ZXEngine::AxisType::X:
					scale.x += (dis * transRate);
					break;
				case ZXEngine::AxisType::Y:
					scale.y += (dis * transRate);
					break;
				case ZXEngine::AxisType::Z:
					scale.z += (dis * transRate);
					break;
				}

				goTrans->Scale(scale);
			}
		}

		mLastWidgetScreenPos = widgetScreenPos;
	}

	void EditorCamera::MoveForwardCallBack(const string& args)
	{
		MoveCamera(mCameraTrans->GetForward());
	}

	void EditorCamera::MoveBackCallBack(const string& args)
	{
		MoveCamera(-mCameraTrans->GetForward());
	}

	void EditorCamera::MoveLeftCallBack(const string& args)
	{
		MoveCamera(-mCameraTrans->GetRight());
	}

	void EditorCamera::MoveRightCallBack(const string& args)
	{
		MoveCamera(mCameraTrans->GetRight());
	}

	void EditorCamera::MoveMouseCallBack(const string& args)
	{
		auto pos = Utils::StringSplit(args, '|');

		float xPos = std::stof(pos[0]);
		float yPos = std::stof(pos[1]);

		if (mFirstMove)
		{
			mLastX = xPos;
			mLastY = yPos;
			mFirstMove = false;
		}

		float xOffset = xPos - mLastX;
		float yOffset = mLastY - yPos;

		mLastX = xPos;
		mLastY = yPos;

		xOffset *= mSensitivity;
		yOffset *= mSensitivity;

		RotateCamera(xOffset, yOffset);
	}

	void EditorCamera::AccelerateCallBack(const string& args)
	{
		mRate = 3.0f;
	}

	void EditorCamera::DecelerateCallBack(const string& args)
	{
		mRate = 1.0f;
	}

	void EditorCamera::MoveCamera(const Vector3& dir)
	{
		Vector3 offset = dir * mSpeed * mRate * Time::deltaTimeEditor;
		mCameraTrans->Translate(offset);
	}

	void EditorCamera::RotateCamera(float hOffset, float vOffset)
	{
		Vector3 angle = mCameraTrans->GetEulerAngles();
		angle.x -= vOffset;
		angle.y += hOffset;
		angle.x = Math::Clamp(angle.x, -89.0f, 89.0f);
		mCameraTrans->SetEulerAngles(angle);
	}
}