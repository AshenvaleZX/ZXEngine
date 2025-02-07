#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderEngine
	{
	public:
		static void Create();
		static RenderEngine* GetInstance();

	private:
		static RenderEngine* mInstance;

	public:
		RenderEngine();
		~RenderEngine() {};

		void CloseWindow(const string& args);
		int WindowShouldClose();

		// 绘制
		void BeginRender();
		void Render();
		void EndRender();
	};
}