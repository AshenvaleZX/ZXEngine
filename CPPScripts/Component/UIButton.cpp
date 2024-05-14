#include "UIButton.h"
#include "Transform.h"
#include "../EventManager.h"
#include "../GlobalData.h"
#include "../LuaManager.h"

namespace ZXEngine
{
	ComponentType UIButton::GetType()
	{
		return ComponentType::UIButton;
	}

	ComponentType UIButton::GetInsType()
	{
		return ComponentType::UIButton;
	}

	UIButton::UIButton()
	{
		mUpCallBackKey = EventManager::GetInstance()->AddEventHandler((int)EventType::MOUSE_BUTTON_1_UP, std::bind(&UIButton::BtnUpCallBack, this, std::placeholders::_1));
		mDownCallBackKey = EventManager::GetInstance()->AddEventHandler((int)EventType::MOUSE_BUTTON_1_DOWN, std::bind(&UIButton::BtnDownCallBack, this, std::placeholders::_1));
	}

	UIButton::~UIButton()
	{
		UnregisterCallBack();
		EventManager::GetInstance()->RemoveEventHandler((int)EventType::MOUSE_BUTTON_1_UP, mUpCallBackKey);
		EventManager::GetInstance()->RemoveEventHandler((int)EventType::MOUSE_BUTTON_1_DOWN, mDownCallBackKey);
	}

	void UIButton::UnregisterCallBack()
	{
		if (mClickCallBack.func != INT_MAX)
		{
			LuaManager::GetInstance()->Unref(mClickCallBack.func);
			mClickCallBack.func = INT_MAX;
		}
		if (mClickCallBack.table != INT_MAX)
		{
			LuaManager::GetInstance()->Unref(mClickCallBack.table);
			mClickCallBack.table = INT_MAX;
		}
	}

	void UIButton::CheckScale()
	{
		if (!mScaleDirty)
			return;

		auto scale = GetTransform()->GetScale();
		mScale.x = scale.Get(0, 0);
		mScale.y = scale.Get(1, 1);

		mScaleDirty = false;
	}

	void UIButton::CheckClick() const
	{
		auto pos = GetTransform()->GetPosition();

		float hWidth = mScale.x * mWidth * 0.5f;
		float hHeight = mScale.y * mHeight * 0.5f;
		float clickSrcX = mClickX - GlobalData::srcWidth * 0.5f;
		float clickSrcY = GlobalData::srcHeight * 0.5f - mClickY;

		if (clickSrcX >= pos.x - hWidth && clickSrcX <= pos.x + hWidth &&
			clickSrcY >= pos.y - hHeight && clickSrcY <= pos.y + hHeight)
		{
			LuaManager::GetInstance()->CallFunction(mClickCallBack.table, mClickCallBack.func);
		}
	}

	void UIButton::BtnUpCallBack(const string& args)
	{
		if (IsActive() == false)
			return;

		auto argsVec = Utils::StringSplit(args, '|');

		float upX = std::stof(argsVec[0]);
		float upY = std::stof(argsVec[1]);

		if (abs(upX - mClickX) < 5.0f && abs(upY - mClickY) < 5.0f)
		{
			CheckScale();
			CheckClick();
		}
	}

	void UIButton::BtnDownCallBack(const string& args)
	{
		if (IsActive() == false)
			return;

		auto argsVec = Utils::StringSplit(args, '|');

		mClickX = std::stof(argsVec[0]);
		mClickY = std::stof(argsVec[1]);
	}
}