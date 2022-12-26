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

		// 判断当前鼠标所在区域
		EditorAreaType CheckCurMousePos();
	};
}