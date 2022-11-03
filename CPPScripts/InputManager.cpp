#include "InputManager.h"
#include "RenderEngine.h"
#include "EventManager.h"

// 因为GLFW的函数接口问题，没办法传递成员函数，所以这里用普通函数包了一层
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

namespace ZXEngine
{
	InputManager* InputManager::mInstance = nullptr;

	void InputManager::Create()
	{
		mInstance = new InputManager();
		mInstance->RegisterMouse();
	}

	InputManager* InputManager::GetInstance()
	{
		return mInstance;
	}

	void InputManager::RegisterMouse()
	{
		glfwSetCursorPosCallback(RenderEngine::GetInstance()->window, CursorPosCallback);
		glfwSetScrollCallback(RenderEngine::GetInstance()->window, ScrollCallback);
		// tell GLFW to capture our mouse
		glfwSetInputMode(RenderEngine::GetInstance()->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void InputManager::Update()
	{
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

	void InputManager::UpdateKeyInput()
	{
		GLFWwindow* window = RenderEngine::GetInstance()->window;

		// 从0到9
		for (int i = 0; i < 10; i++)
		{
			if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			{
				EventManager::GetInstance()->FireEvent(EventType::KEY_0_PRESS + i, "");
			}
		}

		// 从A到Z
		for (int i = 0; i < 26; i++)
		{
			if (glfwGetKey(window, GLFW_KEY_A + i) == GLFW_PRESS)
			{
				EventManager::GetInstance()->FireEvent(EventType::KEY_A_PRESS + i, "");
			}
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(EventType::KEY_SPACE_PRESS, "");
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(EventType::KEY_ESCAPE_PRESS, "");
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(EventType::KEY_RIGHT_PRESS, "");
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(EventType::KEY_LEFT_PRESS, "");
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(EventType::KEY_DOWN_PRESS, "");
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			EventManager::GetInstance()->FireEvent(EventType::KEY_UP_PRESS, "");
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