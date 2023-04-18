#pragma once
#include "../pubh.h"

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
		virtual void Update() = 0;
		virtual void UpdateMousePos(double xpos, double ypos) = 0;
		virtual void UpdateMouseScroll(double xoffset, double yoffset) = 0;
		virtual void UpdateKeyInput() = 0;
		virtual bool IsShowCursor() = 0;

		// To lua
		virtual void ShowCursor(bool show) = 0;
	};
}