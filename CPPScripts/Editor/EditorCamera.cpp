#include "EditorCamera.h"
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

		mCamera->mFarClipDis = 1000.0f;
		mCamera->mCameraType = CameraType::EditorCamera;

		auto eventMgr = EventManager::GetInstance();
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

	void EditorCamera::MoveCamera(const Vector3& dir)
	{
		Vector3 offset = dir * mSpeed * Time::deltaTime;
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