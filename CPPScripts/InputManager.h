#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class InputManager
	{
	public:
		static void Create();
		static InputManager* GetInstance();

	private:
		static InputManager* mInstance;

	public:
		InputManager();
		~InputManager() {};

		void Update();
		void UpdateMousePos(double xpos, double ypos);
		void UpdateMouseScroll(double xoffset, double yoffset);
		void UpdateKeyInput();

		// To lua
		void ShowCursor(bool show);

	private:
		bool isCursorShow = true;
		int buttonState[InputButton::END];

		void CheckKey(int id, InputButton button, EventType e);
		void CheckMouse(int id, InputButton button, EventType e);
		void RegisterMouse();
		void InitButtonRecord();
	};
}