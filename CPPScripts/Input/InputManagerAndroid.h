#pragma once
#include "InputManager.h"

namespace ZXEngine
{
	class InputManagerAndroid : public InputManager
	{
	public:
		InputManagerAndroid() = default;
		~InputManagerAndroid() = default;

		virtual void Update();
		virtual void UpdateMousePos(float xPos, float yPos) {};
		virtual void UpdateMouseScroll(float xOffset, float yOffset) {};
		virtual bool IsShowCursor() { return false; };
		virtual uint32_t GetTouchCount();
		virtual Touch GetTouch(uint32_t index);

		// To lua
		virtual void ShowCursor(bool show) {};

	private:
		list<Touch> mTouches;
	};
}