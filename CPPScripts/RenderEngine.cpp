#include "RenderEngine.h"
#include "EventManager.h"
#include "MeshRenderer.h"
#include "PublicEnum.h"
#include "SceneManager.h"
#include "CubeMap.h"
#include "ZCamera.h"
#include "RenderPassManager.h"
#include "RenderPass.h"

namespace ZXEngine
{
	RenderEngine* RenderEngine::mInstance = nullptr;

	void RenderEngine::Create()
	{
		mInstance = new RenderEngine();
	}

	RenderEngine* RenderEngine::GetInstance()
	{
		return mInstance;
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	void RenderEngine::InitWindow(unsigned int width, unsigned int height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		scrWidth = width;
		scrHeight = height;
		window = glfwCreateWindow(scrWidth, scrHeight, "ZXEngine", NULL, NULL);
		if (window == NULL)
		{
			Debug::LogError("Failed to create GLFW window");
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
		
		EventManager::GetInstance()->AddEventHandler(EventType::KEY_ESCAPE_PRESS, std::bind(&RenderEngine::CloseWindow, this, std::placeholders::_1));
	}

	void RenderEngine::CloseWindow(string args)
	{
		glfwSetWindowShouldClose(window, true);
	}

	void RenderEngine::SwapBufferAndPollPollEvents()
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	int RenderEngine::WindowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void RenderEngine::BeginRender()
	{
		// 渲染下一帧之前，先清理一下上一帧的Buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderSkyBox();
	}

	void RenderEngine::Render(Camera* camera)
	{
		auto renderPassMgr = RenderPassManager::GetInstance();
		for (unsigned int i = 0; i < renderPassMgr->passes.size(); i++)
		{
			renderPassMgr->passes[i]->Render(camera);
		}
	}

	void RenderEngine::EndRender()
	{
		SwapBufferAndPollPollEvents();
	}

	void RenderEngine::InitSkyBox()
	{
		// 这里自己在代码里写一个Box模型，就不从硬盘加载了
		vec3 points[8] = 
		{
			vec3(1, 1, 1),
			vec3(1, 1, -1),
			vec3(1, -1, 1),
			vec3(1, -1, -1),
			vec3(-1, 1, 1),
			vec3(-1, 1, -1),
			vec3(-1, -1, 1),
			vec3(-1, -1, -1)
		};
		vector<Vertex> vertices;
		vector<unsigned int> indices = 
		{
			// 前
			1,5,7,
			1,7,3,
			// 右
			0,1,3,
			0,3,2,
			// 后
			6,4,0,
			6,0,2,
			// 左
			4,7,5,
			4,6,7,
			// 上
			0,5,1,
			0,4,5,
			// 下
			2,3,7,
			7,6,2
		};
		for (unsigned int i = 0; i < 8; i++)
		{
			Vertex vertex;
			vertex.Position = points[i];
			vertex.Normal = vec3(1);
			vertex.Tangent = vec3(1);
			vertex.Bitangent = vec3(1);
			vertex.TexCoords = vec2(1);
			vertices.push_back(vertex);
		}
		skyBoxShader = new Shader(Resources::GetAssetFullPath("Shaders/SkyBox.zxshader").c_str());
		skyBox = new Mesh(vertices, indices);
	}

	void RenderEngine::RenderSkyBox()
	{
		Camera* camera = Camera::GetAllCameras()[0];

		// 先转3x3再回4x4，把相机位移信息去除
		mat4 mat_V = mat4(mat3(camera->GetViewMatrix()));
		mat4 mat_P = camera->GetProjectionMatrix();

		skyBoxShader->Use();
		skyBoxShader->SetMat4("view", mat_V);
		skyBoxShader->SetMat4("projection", mat_P);
		skyBoxShader->SetCubeMap("skybox", SceneManager::GetInstance()->GetCurScene()->skyBox->GetID(), 0);

		skyBox->Use();

		RenderAPI::GetInstance()->EnableDepthWrite(false);
		RenderAPI::GetInstance()->Draw();
		RenderAPI::GetInstance()->EnableDepthWrite(true);
	}
}