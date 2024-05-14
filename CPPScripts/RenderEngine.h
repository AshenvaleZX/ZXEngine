#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
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

		// ����
		void BeginRender();
		void Render(Camera* camera);
		void EndRender();
	};
}