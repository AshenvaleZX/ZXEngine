#include "InputManager.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Editor/EditorInputManager.h"

// ��ΪGLFW�ĺ����ӿ����⣬û�취���ݳ�Ա������������������ͨ��������һ��
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
		if (!EditorInputManager::GetInstance()->IsProcessGameInput())
			return;
#endif
		UpdateKeyInput();
	}

	void InputManager::UpdateMousePos(double xpos, double ypos)
	{
		EventManager::GetInstance()->FireEvent((int)EventType::UPDATE_MOUSE_POS, to_string(xpos) + "|" + to_string(ypos));
	}

	void InputManager::UpdateMouseScroll(double xoffset, double yoffset)
	{
		// Debug::Log("xoffset " + to_string(xoffset) + " yoffset " + to_string(yoffset));
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
		// ��갴��
		CheckMouse(GLFW_MOUSE_BUTTON_1, InputButton::MOUSE_BUTTON_1, EventType::MOUSE_BUTTON_1_PRESS);
		CheckMouse(GLFW_MOUSE_BUTTON_2, InputButton::MOUSE_BUTTON_2, EventType::MOUSE_BUTTON_2_PRESS);

		// ��0��9
		for (int i = 0; i < 10; i++)
		{
			CheckKey(GLFW_KEY_0 + i, InputButton((int)InputButton::KEY_0 + i), EventType((int)EventType::KEY_0_PRESS + i));
		}

		// ��A��Z
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

	void InputManager::CheckKey(int id, InputButton button, EventType e)
	{
		GLFWwindow* window = RenderEngine::GetInstance()->window;
		int state = glfwGetKey(window, id);
		
		if (state == GLFW_PRESS && buttonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e, ""); // Press
		else if (state == GLFW_PRESS && buttonState[(int)button] == GLFW_RELEASE)
			EventManager::GetInstance()->FireEvent((int)e + 1, ""); // Down
		else if (state == GLFW_RELEASE && buttonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e + 2, ""); // Up

		buttonState[(int)button] = state;
	}

	void InputManager::CheckMouse(int id, InputButton button, EventType e)
	{
		GLFWwindow* window = RenderEngine::GetInstance()->window;
		int state = glfwGetMouseButton(window, id);

		if (state == GLFW_PRESS && buttonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e, ""); // Press
		else if (state == GLFW_PRESS && buttonState[(int)button] == GLFW_RELEASE)
			EventManager::GetInstance()->FireEvent((int)e + 1, ""); // Down
		else if (state == GLFW_RELEASE && buttonState[(int)button] == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent((int)e + 2, ""); // Up

		buttonState[(int)button] = state;
	}

	void InputManager::InitButtonRecord()
	{
		for (int i = 0; i < (int)InputButton::END; i++)
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
#ifdef ZX_EDITOR
	ZXEngine::EditorInputManager::GetInstance()->UpdateMouseScroll((float)xoffset, (float)yoffset);
#endif
}