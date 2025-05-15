#pragma once
#include "InputManager.h"

namespace ZXEngine
{
	class InputManagerIOS : public InputManager
	{
	public:
		InputManagerIOS() = default;
		~InputManagerIOS() = default;

		virtual void Update();
		virtual void UpdateMousePos(float xPos, float yPos) {};
		virtual void UpdateMouseScroll(float xOffset, float yOffset) {};
		virtual bool IsShowCursor() { return false; };
		virtual uint32_t GetTouchCount();
		virtual Touch GetTouch(uint32_t index);

		// To lua
		virtual void ShowCursor(bool show) {};

		void UpdateTouch(int32_t id, float x, float y);
		void RemoveTouch(int32_t id);

	private:
		list<Touch> mTouches;
		unordered_map<int32_t, Vector2> mTouchMap;
	};
}