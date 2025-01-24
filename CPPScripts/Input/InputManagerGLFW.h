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
		virtual void UpdateMousePos(float xPos, float yPos);
		virtual void UpdateMouseScroll(float xOffset, float yOffset);
		virtual bool IsShowCursor();
		virtual uint32_t GetTouchCount() { return 0; };
		virtual Touch GetTouch(uint32_t index) { return Touch{}; };

		// To lua
		virtual void ShowCursor(bool show);

	private:
		bool isCursorShow = true;
		int mButtonState[(int)InputButton::END];

		void PollEvents();
		void UpdateKeyInput();
		void CheckKey(int id, InputButton button, EventType e);
		void CheckMouseKey(int id, InputButton button, EventType e);
		void RegisterMouse();
		void InitButtonRecord();
	};
}