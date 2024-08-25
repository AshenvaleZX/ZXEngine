#include "InputManagerWindows.h"
#include "../EventManager.h"
#include "../Editor/EditorInputManager.h"
#include "../Window/WindowManager.h"
#include "../ProjectSetting.h"

#ifdef ZX_EDITOR
#include "../Editor/EditorGUIManager.h"
#endif

// 防止windows.h里的宏定义max和min影响到其它库里的相同字段
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
		if (EditorInputManager::GetInstance()->IsProcessGameMouseInput())
			CheckMouse();

		UpdateKeyInput();
#else
		CheckMouse();
		UpdateKeyInput();
#endif
	}

	void InputManagerWindows::UpdateMousePos(float xPos, float yPos)
	{
#ifdef ZX_EDITOR
		float headerSize = EditorGUIManager::GetInstance()->mHeaderSize;
		const Vector2& viewBorderSize = EditorGUIManager::GetInstance()->mViewBorderSize;

		mMouseX = xPos - ProjectSetting::hierarchyWidth - viewBorderSize.x;
		mMouseY = yPos - ProjectSetting::mainBarHeight - viewBorderSize.y - headerSize;
#else
		mMouseX = xPos;
		mMouseY = yPos;
#endif
		EventManager::GetInstance()->FireEvent(EventType::UPDATE_MOUSE_POS, to_string(mMouseX) + "|" + to_string(mMouseY));
	}

	void InputManagerWindows::UpdateMouseScroll(float xOffset, float yOffset)
	{
#ifdef ZX_EDITOR
		if (EditorInputManager::GetInstance()->CheckCurMousePos() != EditorAreaType::Game)
			return;
#endif
		EventManager::GetInstance()->FireEvent(EventType::UPDATE_MOUSE_SCROLL, to_string(yOffset));
	}

	void InputManagerWindows::UpdateKeyInput()
	{
		// 鼠标左右键
#ifdef ZX_EDITOR
		if (EditorInputManager::GetInstance()->IsProcessGameMouseInput())
		{
			CheckMouseKey(VK_LBUTTON, InputButton::MOUSE_BUTTON_1, EventType::MOUSE_BUTTON_1_PRESS);
			CheckMouseKey(VK_RBUTTON, InputButton::MOUSE_BUTTON_2, EventType::MOUSE_BUTTON_2_PRESS);
		}
#else
		CheckMouseKey(VK_LBUTTON, InputButton::MOUSE_BUTTON_1, EventType::MOUSE_BUTTON_1_PRESS);
		CheckMouseKey(VK_RBUTTON, InputButton::MOUSE_BUTTON_2, EventType::MOUSE_BUTTON_2_PRESS);
#endif

		// 从0到9
		for (int i = 0; i < 10; i++)
		{
			CheckKey(0x30 + i, InputButton((int)InputButton::KEY_0 + i), EventType((int)EventType::KEY_0_PRESS + i * 3));
		}

		// 从A到Z
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
			EventManager::GetInstance()->FireEvent((uint32_t)e, ""); // Press
		else if ((state & 0x8000) && mButtonState[(int)button] == 0)
			EventManager::GetInstance()->FireEvent((uint32_t)e + 1, ""); // Down
		else if (!(state & 0x8000) && mButtonState[(int)button] == 1)
			EventManager::GetInstance()->FireEvent((uint32_t)e + 2, ""); // Up

		mButtonState[(int)button] = (state & 0x8000) ? 1 : 0;
	}

	void InputManagerWindows::CheckMouseKey(int id, InputButton button, EventType e)
	{
		SHORT state = GetAsyncKeyState(id);

		string pos = to_string(mMouseX) + "|" + to_string(mMouseY);

		if ((state & 0x8000) && mButtonState[(int)button] == 1)
			EventManager::GetInstance()->FireEvent((uint32_t)e, pos); // Press
		else if ((state & 0x8000) && mButtonState[(int)button] == 0)
			EventManager::GetInstance()->FireEvent((uint32_t)e + 1, pos); // Down
		else if (!(state & 0x8000) && mButtonState[(int)button] == 1)
			EventManager::GetInstance()->FireEvent((uint32_t)e + 2, pos); // Up

		mButtonState[(int)button] = (state & 0x8000) ? 1 : 0;
	}

	void InputManagerWindows::CheckMouse()
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(static_cast<HWND>(WindowManager::GetInstance()->GetWindow()), &point);
		UpdateMousePos(static_cast<float>(point.x), static_cast<float>(point.y));
	}

	bool InputManagerWindows::IsShowCursor()
	{
		return isCursorShow;
	}

	void InputManagerWindows::ShowCursor(bool show)
	{
		if (show == isCursorShow)
			return;

		// ShowCursor这个函数不是直接简单的通过bool控制显示，微软在这个函数内部维护了一个计数器
		// 用true调一次就+1，false调一次就-1，在计数值大于等于0的时候才显示
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
			// 0代表松开，1代表按下
			mButtonState[i] = 0;
		}
	}
}