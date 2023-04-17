#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class WindowManager
	{
	public:
		static void Creat();
		static WindowManager* GetInstance();
	private:
		static WindowManager* mInstance;

	public:
		virtual void* GetWindow() = 0;
		virtual void CloseWindow(string args) = 0;
		virtual bool WindowShouldClose() = 0;
	};
}