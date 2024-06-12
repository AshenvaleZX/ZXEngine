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
		// �жϵ�ǰ�����������
		EditorAreaType CheckCurMousePos();
		// ��ǰ�Ƿ�����Ϸ����
		bool IsProcessGameInput();
		bool IsProcessGameKeyInput();
		bool IsProcessGameMouseInput();
		void UpdateMouseScroll(float xOffset, float yOffset);

	private:
		// �Ƿ�����ק״̬
		bool isDragging = false;
	};
}