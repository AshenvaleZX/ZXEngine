#pragma once
#include "Component.h"

namespace ZXEngine
{
	struct LuaCallBackRef
	{
		int func = INT_MAX;
		int table = INT_MAX;
	};

	class UIButton : public Component
	{
	public:
		static ComponentType GetType();

	public:
		float mWidth = 0.0f;
		float mHeight = 0.0f;
		LuaCallBackRef mClickCallBack;

		UIButton();
		~UIButton();

		virtual ComponentType GetInsType();

		void UnregisterCallBack();

	private:
		bool mScaleDirty = true;
		Vector2 mScale = Vector2(1.0f, 1.0f);

		float mClickX = 0.0f;
		float mClickY = 0.0f;
		uint32_t mUpCallBackKey = 0;
		uint32_t mDownCallBackKey = 0;
		uint32_t mTouchClickCallBackKey = 0;

		void CheckScale();
		void CheckClick() const;
		void BtnUpCallBack(const string& args);
		void BtnDownCallBack(const string& args);
		void TouchClickCallBack(const string& args);
	};
}