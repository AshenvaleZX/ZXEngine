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
		virtual void UpdateMousePos(float xPos, float yPos) = 0;
		virtual void UpdateMouseScroll(float xOffset, float yOffset) = 0;
		virtual bool IsShowCursor() = 0;

		// To lua
		virtual void ShowCursor(bool show) = 0;

	protected:
		float mMouseX = 0.0f;
		float mMouseY = 0.0f;
	};
}