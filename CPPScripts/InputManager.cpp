#include "InputManager.h"
#include "RenderEngine.h"

// ��ΪGLFW�ĺ����ӿ����⣬û�취���ݳ�Ա������������������ͨ��������һ��
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);

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
		glfwSetCursorPosCallback(RenderEngine::window, CursorPosCallback);
		glfwSetScrollCallback(RenderEngine::window, ScrollCallback);
		// tell GLFW to capture our mouse
		glfwSetInputMode(RenderEngine::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void InputManager::Update()
	{
		UpdateKeyInput();
	}

	void InputManager::UpdateMousePos(double xpos, double ypos)
	{
		Debug::Log("xpos " + to_string(xpos) + " ypos " + to_string(ypos));
	}

	void InputManager::UpdateMouseScroll(double xoffset, double yoffset)
	{
		Debug::Log("xoffset " + to_string(xoffset) + " yoffset " + to_string(yoffset));
	}

	void InputManager::UpdateKeyInput()
	{
		GLFWwindow* window = RenderEngine::window;
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