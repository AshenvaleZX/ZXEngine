#pragma once
#include "../pubh.h"
#include "EditorEnumStruct.h"

namespace ZXEngine
{
	class EditorInputManager
	{
	public:
		static void Create();
		static EditorInputManager* GetInstance();

	private:
		static EditorInputManager* mInstance;

	public:
		EditorInputManager() {};
		~EditorInputManager() {};

		void Update();
		// 判断当前鼠标所在区域
		EditorAreaType CheckCurMousePos();
		// 当前是否处理游戏输入
		bool IsProcessGameInput();
		bool IsProcessGameKeyInput();
		bool IsProcessGameMouseInput();
		void UpdateMouseScroll(float xOffset, float yOffset);

	private:
		// 是否处于拖拽状态
		bool isDragging = false;
	};
}