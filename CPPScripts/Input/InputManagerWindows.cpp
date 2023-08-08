#include "InputManagerWindows.h"
#include "../EventManager.h"
#include "../Editor/EditorInputManager.h"

// ��ֹwindows.h��ĺ궨��max��minӰ�쵽�����������ͬ�ֶ�
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace ZXEngine
{
	InputManagerWindows::InputManagerWindows()
	{
		InitButtonRecord();
	}

	void InputManagerWindows::Update()
	{
#ifdef ZX_EDITOR
		if (!EditorInputManager::GetInstance()->IsProcessGameInput())
			return;
#endif
		CheckMouse();
		UpdateKeyInput();
	}

	void InputManagerWindows::UpdateMousePos(double xpos, double ypos)
	{
		EventManager::GetInstance()->FireEvent((int)EventType::UPDATE_MOUSE_POS, to_string(xpos) + "|" + to_string(ypos));
	}

	void InputManagerWindows::UpdateMouseScroll(double xoffset, double yoffset)
	{

	}

	void InputManagerWindows::UpdateKeyInput()
	{
		// ������Ҽ�
		CheckKey(VK_LBUTTON, InputButton::MOUSE_BUTTON_1, EventType::MOUSE_BUTTON_1_PRESS);
		CheckKey(VK_RBUTTON, InputButton::MOUSE_BUTTON_2, EventType::MOUSE_BUTTON_2_PRESS);

		// ��0��9
		for (int i = 0; i < 10; i++)
		{
			CheckKey(0x30 + i, InputButton((int)InputButton::KEY_0 + i), EventType((int)EventType::KEY_0_PRESS + i * 3));
		}

		// ��A��Z
		for (int i = 0; i < 26; i++)
		{
			CheckKey(0x41 + i, InputButton((int)InputButton::KEY_A + i), EventType((int)EventType::KEY_A_PRESS + i * 3));
		}

		CheckKey(VK_SPACE,  InputButton::KEY_SPACE,  EventType::KEY_SPACE_PRESS );
		CheckKey(VK_ESCAPE, InputButton::KEY_ESCAPE, EventType::KEY_ESCAPE_PRESS);
		CheckKey(VK_RIGHT,  InputButton::KEY_RIGHT,  EventType::KEY_RIGHT_PRESS );
		CheckKey(VK_LEFT,   InputButton::KEY_LEFT,   EventType::KEY_LEFT_PRESS  );
		CheckKey(VK_DOWN,   InputButton::KEY_DOWN,   EventType::KEY_DOWN_PRESS  );
		CheckKey(VK_UP,     InputButton::KEY_UP,     EventType::KEY_UP_PRESS    );
	}

	void InputManagerWindows::CheckKey(int id, InputButton button, EventType e)
	{
		SHORT state = GetAsyncKeyState(id);
		if ((state & 0x8000) && mButtonState[(int)button] == 1)
			EventManager::GetInstance()->FireEvent((int)e, ""); // Press
		else if ((state & 0x8000) && mButtonState[(int)button] == 0)
			EventManager::GetInstance()->FireEvent((int)e + 1, ""); // Down
		else if (!(state & 0x8000) && mButtonState[(int)button] == 1)
			EventManager::GetInstance()->FireEvent((int)e + 2, ""); // Up
		mButtonState[(int)button] = (state & 0x8000) ? 1 : 0;
	}

	void InputManagerWindows::CheckMouse()
	{
		POINT point;
		GetCursorPos(&point);
		UpdateMousePos(static_cast<double>(point.x), static_cast<double>(point.y));
	}

	bool InputManagerWindows::IsShowCursor()
	{
		return isCursorShow;
	}

	void InputManagerWindows::ShowCursor(bool show)
	{
		if (show == isCursorShow)
			return;

		// ShowCursor�����������ֱ�Ӽ򵥵�ͨ��bool������ʾ��΢������������ڲ�ά����һ��������
		// ��true��һ�ξ�+1��false��һ�ξ�-1���ڼ���ֵ���ڵ���0��ʱ�����ʾ
		if (show)
		{
			while (::ShowCursor(true) < 0) {};
		}
		else
		{
			while (::ShowCursor(false) >= 0) {};
		}

		isCursorShow = show;
	}

	void InputManagerWindows::InitButtonRecord()
	{
		for (int i = 0; i < (int)InputButton::END; i++)
		{
			// 0�����ɿ���1������
			mButtonState[i] = 0;
		}
	}
}