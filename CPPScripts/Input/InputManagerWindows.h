#pragma once
#include "InputManager.h"

namespace ZXEngine
{
	class InputManagerWindows : public InputManager
	{
	public:
		InputManagerWindows();
		~InputManagerWindows() {};

		virtual void Update();
		virtual void UpdateMousePos(double xpos, double ypos);
		virtual void UpdateMouseScroll(double xoffset, double yoffset);
		virtual bool IsShowCursor();

		// To lua
		virtual void ShowCursor(bool show);

	private:
		bool isCursorShow = true;
		int mButtonState[(int)InputButton::END];

		void UpdateKeyInput();
		void CheckKey(int id, InputButton button, EventType e);
		void InitButtonRecord();
	};
}