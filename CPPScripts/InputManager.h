#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class InputManager
	{
	public:
		InputManager() {};
		~InputManager() {};

		static void Create();
		static InputManager* GetInstance();

		void Update();
		void UpdateMousePos(double xpos, double ypos);
		void UpdateMouseScroll(double xoffset, double yoffset);
		void UpdateKeyInput();

	private:
		static InputManager* mInstance;

		void RegisterMouse();
	};
}