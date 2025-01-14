#include "EditorCamera.h"
#include "EditorDataManager.h"
#include "EditorSceneWidgetsRenderer.h"
#include "../GameObject.h"
#include "../EventManager.h"
#include "../Time.h"
#include "../Utils.h"
#include "../Input/InputManager.h"
#include "../GlobalData.h"
#include "../SceneManager.h"

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

	void EditorCamera::Update()
	{
		if (mAutoMove)
		{
			AutoMoveToTarget();
		}
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

	void EditorCamera::MoveTo(const GameObject* target)
	{
		mAutoMove = true;
		mCurAutoMoveTime = 0.0f;

		mAutoMoveOriginPos = mCameraTrans->GetPosition();
		auto pos = target->GetComponent<Transform>()->GetPosition();
		mAutoMoveTargetPos = pos - mCameraTrans->GetForward() * 30.0f;
	}

	void EditorCamera::AutoMoveToTarget()
	{
		mCurAutoMoveTime += Time::deltaTimeEditor;

		if (mCurAutoMoveTime >= mAutoMoveTime)
		{
			mAutoMove = false;
			mCameraTrans->SetPosition(mAutoMoveTargetPos);
		}
		else
		{
			float t = mCurAutoMoveTime / mAutoMoveTime;
			Vector3 pos = Math::Lerp(mAutoMoveOriginPos, mAutoMoveTargetPos, t);
			mCameraTrans->SetPosition(pos);
		}
	}

	void EditorCamera::ClickCallBack(const string& args)
	{
		mPressed = true;
		mCurAxis = AxisType::None;

		auto dataMgr = EditorDataManager::GetInstance();
		if (dataMgr->isGameView || dataMgr->selectedGO == nullptr)
			return;

		auto argList = Utils::StringSplit(args, '|');

		mLastWidgetScreenPos.x = std::stof(argList[0]);
		mLastWidgetScreenPos.y = std::stof(argList[1]);

		auto ray = mCamera->ScreenPointToRay(mLastWidgetScreenPos);

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

		if (mCurAxis != AxisType::None)
		{
			EditorSceneWidgetsRenderer::GetInstance()->UpdateWidgetColor(true);
			mOperateWidgetHandle = EventManager::GetInstance()->AddEditorEventHandler(EventType::UPDATE_MOUSE_POS, std::bind(&EditorCamera::OperateWidgetCallBack, this, std::placeholders::_1));
		}
	}

	void EditorCamera::ReleaseCallBack(const string& args)
	{
		if (mCurAxis == AxisType::None)
		{
			if (!mPressed)
				return;

			auto argList = Utils::StringSplit(args, '|');

			Vector2 releaseScreenPos;
			releaseScreenPos.x = std::stof(argList[0]);
			releaseScreenPos.y = std::stof(argList[1]);

			if (releaseScreenPos.x > 0.0f && releaseScreenPos.x < GlobalData::srcWidth &&
				releaseScreenPos.y > 0.0f && releaseScreenPos.y < GlobalData::srcHeight)
			{
				auto ray = mCamera->ScreenPointToRay(releaseScreenPos);
				auto go = SceneManager::GetInstance()->GetCurScene()->Pick(ray);

				if (go)
				{
					EditorDataManager::GetInstance()->SetSelectedGO(go);
				}
				else
				{
					EditorDataManager::GetInstance()->SetSelectedGO(nullptr);
				}
			}
			else
			{
				EditorDataManager::GetInstance()->SetSelectedGO(nullptr);
			}
		}
		else
		{
			mRotationRadian = 0.0f;
			EditorSceneWidgetsRenderer::GetInstance()->UpdateWidgetColor(false);
			EventManager::GetInstance()->RemoveEditorEventHandler(EventType::UPDATE_MOUSE_POS, mOperateWidgetHandle);
		}

		mPressed = false;
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

			// 这里轴的正负方向不影响旋转操作，因为下一步会根据相机视角与所选取的轴方向来调整旋转的方向
			// 所以这里z轴取反后旋转操作依然是正确的，因为z轴取反后相当于Roate参数的axis和angle同时取反了，这对于旋转操作是等效的
			// 这个取反的意义是让angle的正负发生变化，进而影响mRotationRadian数值的正负
			// 而mRotationRadian会作为Shader参数，影响绘制旋转操作组件的已旋转角度扇形区域
			// 如果这里不取反，在操作z轴旋转时的渲染效果和操作xy轴时的就会不一致
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
				axis = -goTrans->GetForward();
				break;
			default:
				break;
			}

			// 根据视角调整旋转方向，保证操作逻辑的一致性
			float angle = moveDis * 0.01f;
			if (Math::Dot(axis, goDir) > 0.0f)
				angle = -angle;

			mRotationRadian += angle;

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
				default:
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
				default:
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