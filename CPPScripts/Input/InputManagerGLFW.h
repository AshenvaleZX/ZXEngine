#pragma once
#include "InputManager.h"

namespace ZXEngine
{
	class InputManagerGLFW : public InputManager
	{
	public:
		InputManagerGLFW();
		~InputManagerGLFW() {};

		virtual void Update();
		virtual void UpdateMousePos(double xpos, double ypos);
		virtual void UpdateMouseScroll(double xoffset, double yoffset);
		virtual void UpdateKeyInput();
		virtual bool IsShowCursor();

		// To lua
		virtual void ShowCursor(bool show);

	private:
		bool isCursorShow = true;
		int mButtonState[(int)InputButton::END];

		void PollEvents();
		void CheckKey(int id, InputButton button, EventType e);
		void CheckMouse(int id, InputButton button, EventType e);
		void RegisterMouse();
		void InitButtonRecord();
	};
}