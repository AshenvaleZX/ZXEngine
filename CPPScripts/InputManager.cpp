#include "InputManager.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Editor/EditorInputManager.h"

// 因为GLFW的函数接口问题，没办法传递成员函数，所以这里用普通函数包了一层
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

namespace ZXEngine
{
	InputManager* InputManager::mInstance = nullptr;

	void InputManager::Create()
	{
		mInstance = new InputManager();
	}

	InputManager* InputManager::GetInstance()
	{
		return mInstance;
	}

	InputManager::InputManager()
	{
		RegisterMouse();
		InitButtonRecord();
	}

	void InputManager::RegisterMouse()
	{
		glfwSetCursorPosCallback(RenderEngine::GetInstance()->window, CursorPosCallback);
		glfwSetScrollCallback(RenderEngine::GetInstance()->window, ScrollCallback);
		// tell GLFW to capture our mouse
		glfwSetInputMode(RenderEngine::GetInstance()->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void InputManager::Update()
	{
#ifdef ZX_EDITOR
		// 在编辑器模式下，如果鼠标未被游戏捕获，并且当前位置不在游戏画面区域，则不会处理游戏输入
		if (isCursorShow && EditorInputManager::GetInstance()->CheckCurMousePos() != EditorAreaType::EAT_Game)
			return;
#endif
		UpdateKeyInput();
	}

	void InputManager::UpdateMousePos(double xpos, double ypos)
	{
		EventManager::GetInstance()->FireEvent(EventType::UPDATE_MOUSE_POS, to_string(xpos) + "|" + to_string(ypos));
	}

	void InputManager::UpdateMouseScroll(double xoffset, double yoffset)
	{
		Debug::Log("xoffset " + to_string(xoffset) + " yoffset " + to_string(yoffset));
	}

	void InputManager::ShowCursor(bool show)
	{
		if (show)
			glfwSetInputMode(RenderEngine::GetInstance()->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(RenderEngine::GetInstance()->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		isCursorShow = show;
	}

	void InputManager::UpdateKeyInput()
	{
		// 鼠标按键
		CheckMouse(GLFW_MOUSE_BUTTON_1, InputButton::MOUSE_BUTTON_1, EventType::MOUSE_BUTTON_1_PRESS);
		CheckMouse(GLFW_MOUSE_BUTTON_2, InputButton::MOUSE_BUTTON_2, EventType::MOUSE_BUTTON_2_PRESS);

		// 从0到9
		for (int i = 0; i < 10; i++)
		{
			CheckKey(GLFW_KEY_0 + i, InputButton(InputButton::KEY_0 + i), EventType(EventType::KEY_0_PRESS + i));
		}

		// 从A到Z
		for (int i = 0; i < 26; i++)
		{
			CheckKey(GLFW_KEY_A + i, InputButton(InputButton::KEY_A + i), EventType(EventType::KEY_A_PRESS + i * 3));
		}

		CheckKey(GLFW_KEY_SPACE, InputButton::KEY_SPACE, EventType::KEY_SPACE_PRESS);
		CheckKey(GLFW_KEY_ESCAPE, InputButton::KEY_ESCAPE, EventType::KEY_ESCAPE_PRESS);
		CheckKey(GLFW_KEY_RIGHT, InputButton::KEY_RIGHT, EventType::KEY_RIGHT_PRESS);
		CheckKey(GLFW_KEY_LEFT, InputButton::KEY_LEFT, EventType::KEY_LEFT_PRESS);
		CheckKey(GLFW_KEY_DOWN, InputButton::KEY_DOWN, EventType::KEY_DOWN_PRESS);
		CheckKey(GLFW_KEY_UP, InputButton::KEY_UP, EventType::KEY_UP_PRESS);
	}

	void InputManager::CheckKey(int id, InputButton button, EventType e)
	{
		GLFWwindow* window = RenderEngine::GetInstance()->window;
		int state = glfwGetKey(window, id);
		
		if (state == GLFW_PRESS && buttonState[button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(e, ""); // Press
		else if (state == GLFW_PRESS && buttonState[button] == GLFW_RELEASE)
			EventManager::GetInstance()->FireEvent(e + 1, ""); // Down
		else if (state == GLFW_RELEASE && buttonState[button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(e + 2, ""); // Up

		buttonState[button] = state;
	}

	void InputManager::CheckMouse(int id, InputButton button, EventType e)
	{
		GLFWwindow* window = RenderEngine::GetInstance()->window;
		int state = glfwGetMouseButton(window, id);

		if (state == GLFW_PRESS && buttonState[button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(e, ""); // Press
		else if (state == GLFW_PRESS && buttonState[button] == GLFW_RELEASE)
			EventManager::GetInstance()->FireEvent(e + 1, ""); // Down
		else if (state == GLFW_RELEASE && buttonState[button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(e + 2, ""); // Up

		buttonState[button] = state;
	}

	void InputManager::InitButtonRecord()
	{
		for (int i = 0; i < InputButton::END; i++)
		{
			buttonState[i] = GLFW_RELEASE;
		}
	}
}


void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	ZXEngine::InputManager::GetInstance()->UpdateMousePos(xpos, ypos);
}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ZXEngine::InputManager::GetInstance()->UpdateMouseScroll(xoffset, yoffset);
}