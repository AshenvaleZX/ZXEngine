#include "InputManagerGLFW.h"
#include "../EventManager.h"
#include "../Editor/EditorInputManager.h"
#include "../Window/WindowManager.h"
#include <GLFW/glfw3.h>

// 因为GLFW的函数接口问题，没办法传递成员函数，所以这里用普通函数包了一层
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

namespace ZXEngine
{
	InputManagerGLFW::InputManagerGLFW()
	{
		RegisterMouse();
		InitButtonRecord();
	}

	void InputManagerGLFW::RegisterMouse()
	{
		glfwSetCursorPosCallback(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), CursorPosCallback);
		glfwSetScrollCallback(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), ScrollCallback);
		// tell GLFW to capture our mouse
		glfwSetInputMode(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void InputManagerGLFW::Update()
	{
		PollEvents();
#ifdef ZX_EDITOR
		if (!EditorInputManager::GetInstance()->IsProcessGameInput())
			return;
#endif
		UpdateKeyInput();
	}

	void InputManagerGLFW::UpdateMousePos(double xpos, double ypos)
	{
		EventManager::GetInstance()->FireEvent((int)EventType::UPDATE_MOUSE_POS, to_string(xpos) + "|" + to_string(ypos));
	}

	void InputManagerGLFW::UpdateMouseScroll(double xoffset, double yoffset)
	{
		// Debug::Log("xoffset " + to_string(xoffset) + " yoffset " + to_string(yoffset));
	}

	bool InputManagerGLFW::IsShowCursor()
	{
		return isCursorShow;
	}

	void InputManagerGLFW::ShowCursor(bool show)
	{
		if (show)
			glfwSetInputMode(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		isCursorShow = show;
	}

	void InputManagerGLFW::UpdateKeyInput()
	{
		// 鼠标按键
		CheckMouse(GLFW_MOUSE_BUTTON_1, InputButton::MOUSE_BUTTON_1, EventType::MOUSE_BUTTON_1_PRESS);
		CheckMouse(GLFW_MOUSE_BUTTON_2, InputButton::MOUSE_BUTTON_2, EventType::MOUSE_BUTTON_2_PRESS);

		// 从0到9
		for (int i = 0; i < 10; i++)
		{
			CheckKey(GLFW_KEY_0 + i, InputButton((int)InputButton::KEY_0 + i), EventType((int)EventType::KEY_0_PRESS + i * 3));
		}

		// 从A到Z
		for (int i = 0; i < 26; i++)
		{
			CheckKey(GLFW_KEY_A + i, InputButton((int)InputButton::KEY_A + i), EventType((int)EventType::KEY_A_PRESS + i * 3));
		}

		CheckKey(GLFW_KEY_SPACE, InputButton::KEY_SPACE, EventType::KEY_SPACE_PRESS);
		CheckKey(GLFW_KEY_ESCAPE, InputButton::KEY_ESCAPE, EventType::KEY_ESCAPE_PRESS);
		CheckKey(GLFW_KEY_RIGHT, InputButton::KEY_RIGHT, EventType::KEY_RIGHT_PRESS);
		CheckKey(GLFW_KEY_LEFT, InputButton::KEY_LEFT, EventType::KEY_LEFT_PRESS);
		CheckKey(GLFW_KEY_DOWN, InputButton::KEY_DOWN, EventType::KEY_DOWN_PRESS);
		CheckKey(GLFW_KEY_UP, InputButton::KEY_UP, EventType::KEY_UP_PRESS);
	}

	void InputManagerGLFW::PollEvents()
	{
		glfwPollEvents();
	}

	void InputManagerGLFW::CheckKey(int id, InputButton button, EventType e)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow());
		int state = glfwGetKey(window, id);

		if (state == GLFW_PRESS && mButtonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e, ""); // Press
		else if (state == GLFW_PRESS && mButtonState[(int)button] == GLFW_RELEASE)
			EventManager::GetInstance()->FireEvent((int)e + 1, ""); // Down
		else if (state == GLFW_RELEASE && mButtonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e + 2, ""); // Up

		mButtonState[(int)button] = state;
	}

	void InputManagerGLFW::CheckMouse(int id, InputButton button, EventType e)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow());
		int state = glfwGetMouseButton(window, id);

		if (state == GLFW_PRESS && mButtonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e, ""); // Press
		else if (state == GLFW_PRESS && mButtonState[(int)button] == GLFW_RELEASE)
			EventManager::GetInstance()->FireEvent((int)e + 1, ""); // Down
		else if (state == GLFW_RELEASE && mButtonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e + 2, ""); // Up

		mButtonState[(int)button] = state;
	}

	void InputManagerGLFW::InitButtonRecord()
	{
		for (int i = 0; i < (int)InputButton::END; i++)
		{
			mButtonState[i] = GLFW_RELEASE;
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
#ifdef ZX_EDITOR
	ZXEngine::EditorInputManager::GetInstance()->UpdateMouseScroll((float)xoffset, (float)yoffset);
#endif
}