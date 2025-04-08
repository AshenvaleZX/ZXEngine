#include "RenderAPIOpenGL.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "GlobalData.h"
#include "RenderStateSetting.h"
#include "ShaderParser.h"
#include "Resources.h"
#include "ZShader.h"
#include "Material.h"
#include "MaterialData.h"
#include "ProjectSetting.h"
#include "FBOManager.h"
#include "Window/WindowManager.h"
#include "EventManager.h"
#ifdef ZX_EDITOR
#include "Editor/ImGuiTextureManager.h"
#endif

namespace ZXEngine
{
	map<BlendFactor, int> glBlendFactorMap =
	{
        { BlendFactor::ZERO,           GL_ZERO           }, { BlendFactor::ONE,						 GL_ONE                      },
		{ BlendFactor::SRC_COLOR,      GL_SRC_COLOR      }, { BlendFactor::ONE_MINUS_SRC_COLOR,		 GL_ONE_MINUS_SRC_COLOR      },
		{ BlendFactor::DST_COLOR,      GL_DST_COLOR      }, { BlendFactor::ONE_MINUS_DST_COLOR,		 GL_ONE_MINUS_DST_COLOR      },
		{ BlendFactor::SRC_ALPHA,      GL_SRC_ALPHA      }, { BlendFactor::ONE_MINUS_SRC_ALPHA,		 GL_ONE_MINUS_SRC_ALPHA		 },
		{ BlendFactor::DST_ALPHA,      GL_DST_ALPHA      }, { BlendFactor::ONE_MINUS_DST_ALPHA,		 GL_ONE_MINUS_DST_ALPHA		 },
		{ BlendFactor::CONSTANT_COLOR, GL_CONSTANT_COLOR }, { BlendFactor::ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR },
		{ BlendFactor::CONSTANT_ALPHA, GL_CONSTANT_ALPHA }, { BlendFactor::ONE_MINUS_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA },
	};

	map<FaceCullOption, int> glFaceCullOptionMap =
	{
		{ FaceCullOption::Back,			GL_BACK			  }, { FaceCullOption::Front, GL_FRONT },
		{ FaceCullOption::FrontAndBack,	GL_FRONT_AND_BACK },
	};

	RenderAPIOpenGL::RenderAPIOpenGL()
	{
		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Debug::LogError("Failed to initialize GLAD");
			return;
		}

		// 获取当前OpenGL版本并输出
		int majorVersion;
		int minorVersion;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		Debug::Log("Graphic API: OpenGL");
		Debug::Log("Version: " + to_string(majorVersion) + "." + to_string(minorVersion));

		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		targetState = new RenderStateSetting();
		curRealState = new RenderStateSetting();
		FBOClearInfoMap[0] = {};

		CheckError();
	}

	void RenderAPIOpenGL::BeginFrame()
	{
		
	}

	void RenderAPIOpenGL::EndFrame()
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()));
		RealCheckError();
	}

	void RenderAPIOpenGL::OnWindowSizeChange(uint32_t width, uint32_t height)
	{
#ifdef ZX_EDITOR
		uint32_t lastSrcWidth = GlobalData::srcWidth;
		uint32_t lastSrcHeight = GlobalData::srcHeight;

		ProjectSetting::SetWindowSize(width, height);

		if (lastSrcWidth != GlobalData::srcWidth || lastSrcHeight != GlobalData::srcHeight)
		{
			FBOManager::GetInstance()->RecreateAllFollowWindowFBO();
		}
#else
		GlobalData::srcWidth = width;
		GlobalData::srcHeight = height;
		FBOManager::GetInstance()->RecreateAllFollowWindowFBO();
#endif
		EventManager::GetInstance()->FireEvent(EventType::WINDOW_RESIZE, "");
	}

	void RenderAPIOpenGL::OnGameViewSizeChange()
	{
		FBOManager::GetInstance()->RecreateAllFollowWindowFBO();
		EventManager::GetInstance()->FireEvent(EventType::WINDOW_RESIZE, "");
	}

	void RenderAPIOpenGL::SetRenderState(RenderStateSetting* state)
	{
		*targetState = *state;
		stateDirty = true;
	}

	void RenderAPIOpenGL::WaitForRenderFinish()
	{
		// OpenGL的所有行为都是单线程的，同步的，所以不需要实现这个接口
	}

	void RenderAPIOpenGL::SwitchFrameBuffer(uint32_t id)
	{
		if (id == UINT32_MAX)
		{
			curFBOID = 0;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			curFBOID = id;
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}
		CheckError();
	}

	void RenderAPIOpenGL::SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset)
	{
		// 0点在左下角
		glViewport(xOffset, yOffset, width, height);
		CheckError();
	}

	void RenderAPIOpenGL::ClearFrameBuffer(FrameBufferClearFlags clearFlags)
	{
		auto& clearInfo = FBOClearInfoMap[curFBOID];

		if (clearFlags & ZX_CLEAR_FRAME_BUFFER_COLOR_BIT)
			ClearColorBuffer(clearInfo.color);

		if (clearFlags & ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT)
			ClearDepthBuffer(clearInfo.depth);

		if (clearFlags & ZX_CLEAR_FRAME_BUFFER_STENCIL_BIT)
			ClearStencilBuffer(clearInfo.stencil);
	}

	void RenderAPIOpenGL::ClearColorBuffer(const Vector4& color)
	{
		if (curRealState->clearColor != color)
		{
			glClearColor(color.r, color.g, color.b, color.a);
			curRealState->clearColor = color;
			stateDirty = true;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		CheckError();
	}

	void RenderAPIOpenGL::ClearDepthBuffer(float depth)
	{
		if (curRealState->clearDepth != depth)
		{
			glClearDepth(depth);
			curRealState->clearDepth = depth;
			stateDirty = true;
		}
		// Clear Depth之前必须开启深度测试和写入
		if (!curRealState->depthTest)
		{
			glEnable(GL_DEPTH_TEST);
			curRealState->depthTest = true;
			stateDirty = true;
		}
		if (!curRealState->depthWrite)
		{
			glDepthMask(GL_TRUE);
			curRealState->depthWrite = true;
			stateDirty = true;
		}
		glClear(GL_DEPTH_BUFFER_BIT);
		CheckError();
	}

	void RenderAPIOpenGL::ClearStencilBuffer(uint32_t stencil)
	{
		if (curRealState->clearStencil != stencil)
		{
			glClearStencil(static_cast<GLint>(stencil));
			curRealState->clearStencil = stencil;
			stateDirty = true;
		}
		glClear(GL_STENCIL_BUFFER_BIT);
		CheckError();
	}

	void RenderAPIOpenGL::BlitFrameBuffer(uint32_t cmd, const string& src, const string& dst, FrameBufferPieceFlags flags)
	{
		auto sFBO = FBOManager::GetInstance()->GetFBO(src);
		auto dFBO = FBOManager::GetInstance()->GetFBO(dst);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, sFBO->ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dFBO->ID);

		if (flags & ZX_FRAME_BUFFER_PIECE_COLOR)
			glBlitFramebuffer(0, 0, sFBO->width, sFBO->height, 0, 0, dFBO->width, dFBO->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		if (flags & ZX_FRAME_BUFFER_PIECE_DEPTH)
			glBlitFramebuffer(0, 0, sFBO->width, sFBO->height, 0, 0, dFBO->width, dFBO->height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// 切回原来的FBO
		glBindFramebuffer(GL_FRAMEBUFFER, curFBOID);
	}

	void RenderAPIOpenGL::CheckError()
	{
		if (ProjectSetting::enableGraphicsDebug)
			RealCheckError();
	}

	void RenderAPIOpenGL::RealCheckError()
	{
		GLenum error = glGetError();
		if (error == GL_NO_ERROR)
			return;
		else if (error == GL_INVALID_ENUM)
			Debug::LogError("GL_INVALID_ENUM");
		else if (error == GL_INVALID_VALUE)
			Debug::LogError("GL_INVALID_VALUE");
		else if (error == GL_INVALID_OPERATION)
			Debug::LogError("GL_INVALID_OPERATION");
		else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
			Debug::LogError("GL_INVALID_FRAMEBUFFER_OPERATION");
		else if (error == GL_OUT_OF_MEMORY)
			Debug::LogError("GL_OUT_OF_MEMORY");
		else if (error == GL_STACK_UNDERFLOW)
			Debug::LogError("GL_STACK_UNDERFLOW");
		else if (error == GL_STACK_OVERFLOW)
			Debug::LogError("GL_STACK_OVERFLOW");
		else
			Debug::LogError("OTHER_ERROR");
	}

	unsigned int RenderAPIOpenGL::LoadTexture(const char* path, int& width, int& height)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = GL_RGB;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			string p = path;
			Debug::LogError("Texture failed to load at path: " + p);
			stbi_image_free(data);
		}
		CheckError();

		return textureID;
	}

	void RenderAPIOpenGL::DeleteTexture(unsigned int id)
	{
		glDeleteTextures(1, &id);
#ifdef ZX_EDITOR
		ImGuiTextureManager::GetInstance()->DeleteByEngineID(id);
#endif
		CheckError();
	}

	unsigned int RenderAPIOpenGL::LoadCubeMap(const vector<string>& faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrComponents;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				GLenum format = GL_RGB;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				Debug::LogError("Cubemap texture failed to load at path: " + faces[i]);
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // 纹理的第三个轴，类似普通点坐标的z，因为cubemap是3D的纹理
		
		CheckError();

		return textureID;
	}

	unsigned int RenderAPIOpenGL::CreateTexture(TextureFullData* data)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		GLenum format = GL_RGB;
		if (data->numChannel == 1)
			format = GL_RED;
		else if (data->numChannel == 3)
			format = GL_RGB;
		else if (data->numChannel == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, data->width, data->height, 0, format, GL_UNSIGNED_BYTE, data->data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		CheckError();

		return textureID;
	}

	unsigned int RenderAPIOpenGL::CreateCubeMap(CubeMapFullData* data)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		for (unsigned int i = 0; i < 6; i++)
		{
			GLenum format = GL_RGB;
			if (data->numChannel == 1)
				format = GL_RED;
			else if (data->numChannel == 3)
				format = GL_RGB;
			else if (data->numChannel == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, data->width, data->height, 0, format, GL_UNSIGNED_BYTE, data->data[i]);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // 纹理的第三个轴，类似普通点坐标的z，因为cubemap是3D的纹理

		CheckError();

		return textureID;
	}

	unsigned int RenderAPIOpenGL::GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data)
	{
		// 通过字形生成的位图是一个8位灰度图，它的每一个颜色都由一个字节来表示。因此我们需要将位图缓冲的每一字节都作为纹理的颜色值。
		// 这个纹理的每一字节都对应着纹理颜色的红色分量(颜色向量的第一个字节)，即我们使用一个字节来表示纹理的颜色。
		// 但是OpenGL默认所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数，因为大部分纹理的宽度都为4的倍数并/或每像素使用4个字节。
		// 但是现在我们每个像素只用了一个字节，它可以是任意的宽度，所以需要将纹理解压对齐参数设为1，这样才能确保不会有对齐问题。
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// 配置纹理
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 还原默认设置
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		CheckError();

		return textureID;
	}

	ShaderReference* RenderAPIOpenGL::LoadAndSetUpShader(const string& path, FrameBufferType type)
	{
		string shaderCode = Resources::LoadTextFile(path);
		return SetUpShader(path, shaderCode, type);
	}

	ShaderReference* RenderAPIOpenGL::SetUpShader(const string& path, const string& shaderCode, FrameBufferType type)
	{
		string vertCode, geomCode, fragCode;
		ShaderParser::ParseShaderCode(shaderCode, vertCode, geomCode, fragCode);

		vertCode = ShaderParser::TranslateToOpenGL(vertCode);
		geomCode = ShaderParser::TranslateToOpenGL(geomCode);
		fragCode = ShaderParser::TranslateToOpenGL(fragCode);

		// vertex shader
		const char* vShaderCode = vertCode.c_str();
		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");
		// fragment Shader
		const char* fShaderCode = fragCode.c_str();
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry = 0;
		if (geomCode.length() > 1)
		{
			const char* gShaderCode = geomCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		unsigned int ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geomCode.length() > 1)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geomCode.length() > 1)
			glDeleteShader(geometry);

		materialDataInShaders.insert(pair(ID, new OpenGLMaterialData()));

		ShaderReference* reference = new ShaderReference();
		reference->ID = ID;
		reference->shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::OpenGL);
		reference->targetFrameBufferType = type;

		CheckError();

		return reference;
	}

	void RenderAPIOpenGL::DeleteShader(uint32_t id)
	{
		delete materialDataInShaders[id];
		materialDataInShaders.erase(id);
		glDeleteProgram(id);
		CheckError();
	}

	uint32_t RenderAPIOpenGL::CreateMaterialData()
	{
		uint32_t idx = GetNextMaterialDataIndex();
		auto materialData = GetMaterialDataByIndex(idx);

		materialData->inUse = true;

		return idx;
	}

	void RenderAPIOpenGL::UseMaterialData(uint32_t ID)
	{
		curMaterialDataID = ID;
	}

	void RenderAPIOpenGL::SetUpMaterial(Material* material)
	{
		// 设置材质数据
		for (auto& property : material->data->vec2Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->vec3Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->vec4Datas)
			SetShaderVector(material, property.first, property.second, true);
		for (auto& property : material->data->floatDatas)
			SetShaderScalar(material, property.first, property.second, true);
		for (auto& property : material->data->uintDatas)
			SetShaderScalar(material, property.first, property.second, true);
		for (auto& property : material->data->colorDatas)
			SetShaderVector(material, property.first, property.second, true);

		material->data->initialized = true;
	}
	
	void RenderAPIOpenGL::DeleteMaterialData(uint32_t id)
	{
		auto materialData = GetMaterialDataByIndex(id);

		materialData->intList.clear();
		materialData->uintList.clear();
		materialData->boolList.clear();
		materialData->floatList.clear();
		materialData->vec2List.clear();
		materialData->vec3List.clear();
		materialData->vec4List.clear();
		materialData->vec4ArrayList.clear();
		materialData->mat3List.clear();
		materialData->mat4List.clear();
		materialData->mat4ArrayList.clear();
		materialData->textures.clear();
		materialData->cubeMaps.clear();

		materialData->inUse = false;
	}

	void RenderAPIOpenGL::CheckCompileErrors(unsigned int shader, string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				Debug::LogError("ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				Debug::LogError("ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
			}
		}
		CheckError();
	}

	FrameBufferObject* RenderAPIOpenGL::CreateFrameBufferObject(FrameBufferType type, unsigned int width, unsigned int height)
	{
		ClearInfo clearInfo = {};
		return CreateFrameBufferObject(type, clearInfo, width, height);
	}

	FrameBufferObject* RenderAPIOpenGL::CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width, unsigned int height)
	{
		FrameBufferObject* FBO = new FrameBufferObject(type);
		FBO->clearInfo = clearInfo;
		FBO->isFollowWindow = width == 0 || height == 0;

		width = width == 0 ? GlobalData::srcWidth : width;
		height = height == 0 ? GlobalData::srcHeight : height;

		FBO->width = width;
		FBO->height = height;

		if (type == FrameBufferType::Normal || type == FrameBufferType::Deferred)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);

			// 创建ColorBuffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 创建DepthBuffer
			unsigned int depthBuffer;
#ifdef ZX_PLATFORM_MACOS
			if (type == FrameBufferType::Normal)
			{
				// 在Mac上使用OpenGL 4.1，不支持把Renderbuffer复制到Texture，而目前正向渲染管线里需要这个操作
				glGenTextures(1, &depthBuffer);
				glBindTexture(GL_TEXTURE_2D, depthBuffer);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			}
			else
			{
				glGenRenderbuffers(1, &depthBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			}
#else
			// 用的Renderbuffer而不是Texture，Renderbuffer一般用于不读取，只写入或者复制的buffer，所以深度缓冲区更适合用Renderbuffer
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
#endif

			// 把ColorBuffer和DepthBuffer绑定到FBO上
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
#ifdef ZX_PLATFORM_MACOS
			if (type == FrameBufferType::Normal)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
			else
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
#else
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
#endif

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer Normal not complete!");

			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = depthBuffer;
		}
		else if (type == FrameBufferType::HigthPrecision)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);

			// 创建高精度(浮点)ColorBuffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 创建DepthBuffer
			// (用的Renderbuffer而不是Texture，Renderbuffer一般用于不读取，只写入或者复制的buffer，所以深度缓冲区更适合用Renderbuffer)
			unsigned int depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

			// 把ColorBuffer和DepthBuffer绑定到FBO上
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer HigthPrecision not complete!");

			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = depthBuffer;
		}
		else if (type == FrameBufferType::Color)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);

			// 创建ColorBuffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 把ColorBuffer绑定到FBO上
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer Color not complete!");

			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = UINT32_MAX;
		}
		else if (type == FrameBufferType::ShadowMap)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);

			// 创建深度Map
			unsigned int depthMap;
			glGenTextures(1, &depthMap);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			// 把深度Map绑定到FBO
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

			// 明确告诉OpenGL这个FBO不会渲染到Color Buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer ShadowMap not complete!");

			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = UINT32_MAX;
			FBO->DepthBuffer = depthMap;
		}
		else if (type == FrameBufferType::ShadowCubeMap)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);

			// 创建深度CubeMap
			unsigned int depthCubeMap;
			glGenTextures(1, &depthCubeMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
			for (unsigned int i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			// 把深度CubeMap绑定到FBO
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);

			// 明确告诉OpenGL这个FBO不会渲染到Color Buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer ShadowCubeMap not complete!");

			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = UINT32_MAX;
			FBO->DepthBuffer = depthCubeMap;
		}
		else if (type == FrameBufferType::GBuffer)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);

			// 创建Position Buffer
			unsigned int posBuffer;
			glGenTextures(1, &posBuffer);
			glBindTexture(GL_TEXTURE_2D, posBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 创建Normal Buffer
			unsigned int normalBuffer;
			glGenTextures(1, &normalBuffer);
			glBindTexture(GL_TEXTURE_2D, normalBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 创建Color Buffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 创建Depth Buffer
			unsigned int depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

			// 绑定到FBO上
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posBuffer, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorBuffer, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

			// 3个Render Target
			unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, attachments);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer GBuffer not complete!");

			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = depthBuffer;
			FBO->PositionBuffer = posBuffer;
			FBO->NormalBuffer = normalBuffer;
		}
		else if (type == FrameBufferType::RayTracing)
		{
			// OpenGL不支持光线追踪
		}
		else
		{
			Debug::LogError("Invalide frame buffer type.");
		}

		FBOClearInfoMap[FBO->ID] = clearInfo;
		CheckError();

		return FBO;
	}

	void RenderAPIOpenGL::DeleteFrameBufferObject(FrameBufferObject* FBO)
	{
		FBOClearInfoMap.erase(FBO->ID);

		if (FBO->ColorBuffer != UINT32_MAX)
			glDeleteTextures(1, &FBO->ColorBuffer);

		if (FBO->DepthBuffer != UINT32_MAX)
		{
			if (FBO->type == FrameBufferType::Normal || FBO->type == FrameBufferType::HigthPrecision ||
				FBO->type == FrameBufferType::GBuffer || FBO->type == FrameBufferType::Deferred)
				glDeleteRenderbuffers(1, &FBO->DepthBuffer);
			else
				glDeleteTextures(1, &FBO->DepthBuffer);
		}

		if (FBO->PositionBuffer != UINT32_MAX)
			glDeleteTextures(1, &FBO->PositionBuffer);

		if (FBO->NormalBuffer != UINT32_MAX)
			glDeleteTextures(1, &FBO->NormalBuffer);

		glDeleteFramebuffers(1, &FBO->ID);
		CheckError();
	}

	uint32_t RenderAPIOpenGL::GetRenderBufferTexture(uint32_t id)
	{
		return id;
	}

	uint32_t RenderAPIOpenGL::CreateStaticInstanceBuffer(uint32_t size, uint32_t num, const void* data)
	{
		GLuint buffer;
		GLsizeiptr bufferSize = static_cast<GLsizeiptr>(num * size * static_cast<uint32_t>(sizeof(Vector4)));

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);

		return static_cast<uint32_t>(buffer);
	};

	uint32_t RenderAPIOpenGL::CreateDynamicInstanceBuffer(uint32_t size, uint32_t num)
	{
		GLuint buffer;
		GLsizeiptr bufferSize = static_cast<GLsizeiptr>(num * size * static_cast<uint32_t>(sizeof(Vector4)));

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);

		return static_cast<uint32_t>(buffer);
	};

	void RenderAPIOpenGL::UpdateDynamicInstanceBuffer(uint32_t id, uint32_t size, uint32_t num, const void* data)
	{
		GLsizeiptr bufferSize = static_cast<GLsizeiptr>(num * size * static_cast<uint32_t>(sizeof(Vector4)));

		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, data);
	};

	void RenderAPIOpenGL::SetUpInstanceBufferAttribute(uint32_t VAO, uint32_t instanceBuffer, uint32_t size, uint32_t offset)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		glBindVertexArray(meshBuffer->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

		for (uint32_t i = 0; i < size; i++)
		{
			glEnableVertexAttribArray(offset + i);
			glVertexAttribPointer(offset + i, 4, GL_FLOAT, GL_FALSE, size * sizeof(Vector4), (void*)(sizeof(Vector4) * i));
			glVertexAttribDivisor(offset + i, 1);
		}

		glBindVertexArray(0);
	};

	void RenderAPIOpenGL::DeleteInstanceBuffer(uint32_t id)
	{
		glDeleteBuffers(1, &id);
	};

	void RenderAPIOpenGL::GenerateParticleMesh(unsigned int& VAO)
	{
		vector<Vertex> vertices =
		{
			{ .Position = {  0.5f,  0.5f, 0.0f, 1.0f }, .TexCoords = { 1.0f, 0.0f, 0.0f, 0.0f } },
			{ .Position = {  0.5f, -0.5f, 0.0f, 1.0f }, .TexCoords = { 1.0f, 1.0f, 0.0f, 0.0f } },
			{ .Position = { -0.5f,  0.5f, 0.0f, 1.0f }, .TexCoords = { 0.0f, 0.0f, 0.0f, 0.0f } },
			{ .Position = { -0.5f, -0.5f, 0.0f, 1.0f }, .TexCoords = { 0.0f, 1.0f, 0.0f, 0.0f } },
		};

		vector<uint32_t> indices =
		{
			2, 1, 3,
			2, 0, 1,
		};

		SetUpStaticMesh(VAO, vertices, indices);
	}

	uint32_t RenderAPIOpenGL::AllocateDrawCommand(CommandType commandType, FrameBufferClearFlags clearFlags)
	{
		// OpenGL不需要这个接口
		return 0;
	}

	void RenderAPIOpenGL::FreeDrawCommand(uint32_t commandID)
	{
		// OpenGL不需要这个接口
	}

	void RenderAPIOpenGL::Draw(uint32_t VAO)
	{
		UpdateRenderState();
		UpdateMaterialData();

		auto meshBuffer = GetVAOByIndex(VAO);

		glBindVertexArray(meshBuffer->VAO);

		if (meshBuffer->indexed)
			glDrawElements(GL_TRIANGLES, meshBuffer->size, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, meshBuffer->size);

		// 绘制完重置一下(不重置也行，不过及时重置避免出问题)
		glBindVertexArray(0);

		CheckError();
#ifdef ZX_DEBUG
		Debug::drawCallCount++;
#endif
	}

	void RenderAPIOpenGL::DrawInstanced(uint32_t VAO, uint32_t instanceNum, uint32_t instanceBuffer)
	{
		UpdateRenderState();
		UpdateMaterialData();

		auto meshBuffer = GetVAOByIndex(VAO);

		glBindVertexArray(meshBuffer->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

		if (meshBuffer->indexed)
			glDrawElementsInstanced(GL_TRIANGLES, meshBuffer->size, GL_UNSIGNED_INT, 0, instanceNum);
		else
			glDrawArraysInstanced(GL_TRIANGLES, 0, meshBuffer->size, instanceNum);

		// 绘制完重置一下(不重置也行，不过及时重置避免出问题)
		glBindVertexArray(0);

		CheckError();
#ifdef ZX_DEBUG
		Debug::drawCallCount++;
#endif
	};

	void RenderAPIOpenGL::GenerateDrawCommand(uint32_t id)
	{
		// OpenGL不需要这个接口
	}

	void RenderAPIOpenGL::DeleteMesh(unsigned int VAO)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		glDeleteBuffers(1, &meshBuffer->VBO);
		glDeleteBuffers(1, &meshBuffer->EBO);
		glDeleteVertexArrays(1, &meshBuffer->VAO);

		meshBuffer->inUse = false;

		CheckError();
	}

	void RenderAPIOpenGL::SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool skinned)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->indexed = true;
		meshBuffer->size = static_cast<uint32_t>(indices.size());

		// create buffers/arrays
		glGenVertexArrays(1, &meshBuffer->VAO);
		glGenBuffers(1, &meshBuffer->VBO);
		glGenBuffers(1, &meshBuffer->EBO);

		glBindVertexArray(meshBuffer->VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer->VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a Vector3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
#ifdef ZX_COMPUTE_ANIMATION
		if (skinned)
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_COPY);
		else
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
#else
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
#endif

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bone weights
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));
		// vertex bone indices
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));

		// 设置完记得还原一下
		glBindVertexArray(0);

		meshBuffer->inUse = true;

		CheckError();
	}

	void RenderAPIOpenGL::SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize)
	{
		VAO = GetNextVAOIndex();
		auto meshBuffer = GetVAOByIndex(VAO);
		meshBuffer->indexed = true;
		meshBuffer->size = indexSize;

		glGenVertexArrays(1, &meshBuffer->VAO);
		glGenBuffers(1, &meshBuffer->VBO);
		glGenBuffers(1, &meshBuffer->EBO);

		glBindVertexArray(meshBuffer->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer->VBO);
		// 设置为GL_DYNAMIC_DRAW，这里暂时不初始化数据，后面通过UpdateDynamicMesh更新数据
		glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(uint32_t), NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));

		glBindVertexArray(0);

		meshBuffer->inUse = true;

		CheckError();
	}

	void RenderAPIOpenGL::UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
	{
		auto meshBuffer = GetVAOByIndex(VAO);

		// 切换到指定VAO
		glBindVertexArray(meshBuffer->VAO);

		// 更新VBO数据
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);

		// 更新EBO数据
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer->EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);

		// 还原
		glBindVertexArray(0);

		CheckError();
	}

	void RenderAPIOpenGL::UseShader(unsigned int ID)
	{
		curShaderID = ID;
		glUseProgram(ID);
		CheckError();
	}

	// Boolean
	void RenderAPIOpenGL::SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->boolList[name] = value;
	}
	void RenderAPIOpenGL::RealSetShaderScalar(const string& name, bool value)
	{
		glUniform1i(glGetUniformLocation(curShaderID, name.c_str()), (int)value);
		CheckError();
	}

	// Float
	void RenderAPIOpenGL::SetShaderScalar(Material* material, const string& name, float value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->floatList[name] = value;
	}
	void RenderAPIOpenGL::RealSetShaderScalar(const string& name, float value)
	{
		glUniform1f(glGetUniformLocation(curShaderID, name.c_str()), value);
		CheckError();
	}

	// Integer
	void RenderAPIOpenGL::SetShaderScalar(Material* material, const string& name, int32_t value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->intList[name] = value;
	}
	void RenderAPIOpenGL::RealSetShaderScalar(const string& name, int32_t value)
	{
		glUniform1i(glGetUniformLocation(curShaderID, name.c_str()), value);
		CheckError();
	}

	// Unsigned Integer
	void RenderAPIOpenGL::SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->uintList[name] = value;
	}
	void RenderAPIOpenGL::RealSetShaderScalar(const string& name, uint32_t value)
	{
		glUniform1ui(glGetUniformLocation(curShaderID, name.c_str()), value);
		CheckError();
	}

	// Vector2
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->vec2List[name] = value;
	}
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer)
	{
		string arrayName = name + "[" + to_string(idx) + "]";
		SetShaderVector(material, arrayName, value);
	}
	void RenderAPIOpenGL::RealSetShaderVector(const string& name, const Vector2& value)
	{
		glUniform2fv(glGetUniformLocation(curShaderID, name.c_str()), 1, value.v);
		CheckError();
	}

	// Vector3
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->vec3List[name] = value;
	}
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer)
	{
		string arrayName = name + "[" + to_string(idx) + "]";
		SetShaderVector(material, arrayName, value);
	}
	void RenderAPIOpenGL::RealSetShaderVector(const string& name, const Vector3& value)
	{
		glUniform3fv(glGetUniformLocation(curShaderID, name.c_str()), 1, value.v);
		CheckError();
	}

	// Vector4
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->vec4List[name] = value;
	}
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer)
	{
		string arrayName = name + "[" + to_string(idx) + "]";
		SetShaderVector(material, arrayName, value);
	}
	void RenderAPIOpenGL::SetShaderVector(Material* material, const string& name, const Vector4* value, uint32_t count, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->vec4ArrayList[name] = { value, count };
	}
	void RenderAPIOpenGL::RealSetShaderVector(const string& name, const Vector4& value)
	{
		glUniform4fv(glGetUniformLocation(curShaderID, name.c_str()), 1, value.v);
		CheckError();
	}
	void RenderAPIOpenGL::RealSetShaderVector(const string& name, const Vector4* value, uint32_t count)
	{
		const GLfloat* ptr = reinterpret_cast<const GLfloat*>(value);
		glUniform4fv(glGetUniformLocation(curShaderID, name.c_str()), count, ptr);
		CheckError();
	}

	// Matrix3
	void RenderAPIOpenGL::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->mat3List[name] = value;
	}
	void RenderAPIOpenGL::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer)
	{
		string arrayName = name + "[" + to_string(idx) + "]";
		SetShaderMatrix(material, arrayName, value);
	}
	void RenderAPIOpenGL::RealSetShaderMatrix(const string& name, const Matrix3& value)
	{
		float* array = new float[9];
		value.ToColumnMajorArray(array);
		glUniformMatrix3fv(glGetUniformLocation(curShaderID, name.c_str()), 1, GL_FALSE, array);
		delete[] array;
		CheckError();
	}

	// Matrix4
	void RenderAPIOpenGL::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->mat4List[name] = value;
	}
	void RenderAPIOpenGL::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer)
	{
		string arrayName = name + "[" + to_string(idx) + "]";
		SetShaderMatrix(material, arrayName, value);
	}
	void RenderAPIOpenGL::SetShaderMatrix(Material* material, const string& name, const Matrix4* value, uint32_t count, bool allBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->mat4ArrayList[name] = { value, count };
	}
	void RenderAPIOpenGL::RealSetShaderMatrix(const string& name, const Matrix4& value)
	{
		float* array = new float[16];
		value.ToColumnMajorArray(array);
		glUniformMatrix4fv(glGetUniformLocation(curShaderID, name.c_str()), 1, GL_FALSE, array);
		delete[] array;
		CheckError();
	}
	void RenderAPIOpenGL::RealSetShaderMatrix(const string& name, const Matrix4* value, uint32_t count)
	{
		const GLfloat* ptr = reinterpret_cast<const GLfloat*>(value);
		glUniformMatrix4fv(glGetUniformLocation(curShaderID, name.c_str()), count, GL_FALSE, ptr);
		CheckError();
	}

	// Texture
	void RenderAPIOpenGL::SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->textures[name] = { ID, idx };
	}
	void RenderAPIOpenGL::RealSetShaderTexture(const string& name, uint32_t ID, uint32_t idx)
	{
		glUniform1i(glGetUniformLocation(curShaderID, name.c_str()), idx);
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, ID);
		CheckError();
	}

	// Cube Map
	void RenderAPIOpenGL::SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		auto materialData = GetMaterialDataByIndex(material->data->GetID());
		materialData->cubeMaps[name] = { ID, idx };
	}
	void RenderAPIOpenGL::RealSetShaderCubeMap(const string& name, uint32_t ID, uint32_t idx)
	{
		glUniform1i(glGetUniformLocation(curShaderID, name.c_str()), idx);
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
		CheckError();
	}

	uint32_t RenderAPIOpenGL::CreateShaderStorageBuffer(const void* data, size_t size, GPUBufferType type)
	{
		GLuint buffer;

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

		if (type == GPUBufferType::Static)
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
		else if (type == GPUBufferType::DynamicCPUWriteGPURead)
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), data, GL_DYNAMIC_DRAW);
		else if (type == GPUBufferType::DynamicGPUWriteCPURead)
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), data, GL_DYNAMIC_READ);
		else if (type == GPUBufferType::DynamicGPUWriteGPURead)
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), data, GL_DYNAMIC_COPY);

		return static_cast<uint32_t>(buffer);
	}

	void RenderAPIOpenGL::BindShaderStorageBuffer(uint32_t id, uint32_t binding)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
	}

	void RenderAPIOpenGL::UpdateShaderStorageBuffer(uint32_t id, const void* data, size_t size)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
	}

	void RenderAPIOpenGL::DeleteShaderStorageBuffer(uint32_t id)
	{
		glDeleteBuffers(1, &id);
	}

	void RenderAPIOpenGL::BindVertexBuffer(uint32_t VAO, uint32_t binding)
	{
		auto meshBuffer = GetVAOByIndex(VAO);
		BindShaderStorageBuffer(meshBuffer->VBO, binding);
	}

	ComputeShaderReference* RenderAPIOpenGL::LoadAndSetUpComputeShader(const string& path)
	{
		string shaderCode = Resources::LoadTextFile(path + ".glc");

		unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
		const char* cShaderCode = shaderCode.c_str();
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		CheckCompileErrors(compute, "COMPUTE");

		unsigned int ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");

		glDeleteShader(compute);

		ComputeShaderReference* reference = new ComputeShaderReference();
		reference->ID = ID;

		CheckError();

		return reference;
	};

	void RenderAPIOpenGL::DeleteComputeShader(uint32_t id)
	{
		glDeleteProgram(id);
		CheckError();
	}

	void RenderAPIOpenGL::Dispatch(uint32_t commandID, uint32_t shaderID, uint32_t groupX, uint32_t groupY, uint32_t groupZ)
	{
		glUseProgram(shaderID);
		glDispatchCompute(groupX, groupY, groupZ);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		CheckError();
	}

	void RenderAPIOpenGL::SubmitAllComputeCommands()
	{
		// OpenGL不需要这个接口
	}

	void RenderAPIOpenGL::UpdateRenderState()
	{
		if (stateDirty)
			stateDirty = false;
		else
			return;

		if (curRealState->depthTest != targetState->depthTest)
		{
			if (targetState->depthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		if (curRealState->depthWrite != targetState->depthWrite)
		{
			if (targetState->depthWrite)
				glDepthMask(GL_TRUE);
			else
				glDepthMask(GL_FALSE);
		}

		if (curRealState->srcFactor != targetState->srcFactor || curRealState->dstFactor != targetState->dstFactor)
			glBlendFunc(glBlendFactorMap[targetState->srcFactor], glBlendFactorMap[targetState->dstFactor]);

		if (curRealState->faceCull != targetState->faceCull)
		{
			if (targetState->faceCull)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		if (curRealState->faceCullOption != targetState->faceCullOption)
			glCullFace(glFaceCullOptionMap[targetState->faceCullOption]);

		if (curRealState->clearColor != targetState->clearColor)
			glClearColor(targetState->clearColor.r, targetState->clearColor.g, targetState->clearColor.b, targetState->clearColor.a);

		if (curRealState->clearDepth != targetState->clearDepth)
			glClearDepth(targetState->clearDepth);

		if (curRealState->clearStencil != targetState->clearStencil)
			glClearStencil(targetState->clearStencil);

		*curRealState = *targetState;

		CheckError();
	}

	void RenderAPIOpenGL::UpdateMaterialData()
	{
		auto shaderData = materialDataInShaders[curShaderID];
		auto materialData = GetMaterialDataByIndex(curMaterialDataID);

		for (auto& iter : materialData->intList)
		{
			bool update = false;
			if (shaderData->intList.find(iter.first) == shaderData->intList.end())
				update = true;
			else if (shaderData->intList[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->intList[iter.first] = iter.second;
				RealSetShaderScalar(iter.first, iter.second);
			}
		}

		for (auto& iter : materialData->boolList)
		{
			bool update = false;
			if (shaderData->boolList.find(iter.first) == shaderData->boolList.end())
				update = true;
			else if (shaderData->boolList[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->boolList[iter.first] = iter.second;
				RealSetShaderScalar(iter.first, iter.second);
			}
		}

		for (auto& iter : materialData->floatList)
		{
			bool update = false;
			if (shaderData->floatList.find(iter.first) == shaderData->floatList.end())
				update = true;
			else if (shaderData->floatList[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->floatList[iter.first] = iter.second;
				RealSetShaderScalar(iter.first, iter.second);
			}
		}

		for (auto& iter : materialData->vec2List)
		{
			bool update = false;
			if (shaderData->vec2List.find(iter.first) == shaderData->vec2List.end())
				update = true;
			else if (shaderData->vec2List[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->vec2List[iter.first] = iter.second;
				RealSetShaderVector(iter.first, iter.second);
			}
		}

		for (auto& iter : materialData->vec3List)
		{
			bool update = false;
			if (shaderData->vec3List.find(iter.first) == shaderData->vec3List.end())
				update = true;
			else if (shaderData->vec3List[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->vec3List[iter.first] = iter.second;
				RealSetShaderVector(iter.first, iter.second);
			}
		}

		for (auto& iter : materialData->vec4List)
		{
			bool update = false;
			if (shaderData->vec4List.find(iter.first) == shaderData->vec4List.end())
				update = true;
			else if (shaderData->vec4List[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->vec4List[iter.first] = iter.second;
				RealSetShaderVector(iter.first, iter.second);
			}
		}
		for (auto& iter : materialData->vec4ArrayList)
		{
			RealSetShaderVector(iter.first, iter.second.first, iter.second.second);
		}

		for (auto& iter : materialData->mat3List)
		{
			bool update = false;
			if (shaderData->mat3List.find(iter.first) == shaderData->mat3List.end())
				update = true;
			else if (shaderData->mat3List[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->mat3List[iter.first] = iter.second;
				RealSetShaderMatrix(iter.first, iter.second);
			}
		}

		for (auto& iter : materialData->mat4List)
		{
			bool update = false;
			if (shaderData->mat4List.find(iter.first) == shaderData->mat4List.end())
				update = true;
			else if (shaderData->mat4List[iter.first] != iter.second)
				update = true;

			if (update)
			{
				shaderData->mat4List[iter.first] = iter.second;
				RealSetShaderMatrix(iter.first, iter.second);
			}
		}
		for (auto& iter : materialData->mat4ArrayList)
		{
			RealSetShaderMatrix(iter.first, iter.second.first, iter.second.second);
		}

		// OpenGL里glActiveTexture和glBindTexture是全局操作，会影响所有Shader，所以这里需要每次都做一下
		for (auto& iter : materialData->textures)
			RealSetShaderTexture(iter.first, iter.second[0], iter.second[1]);
		for (auto& iter : materialData->cubeMaps)
			RealSetShaderCubeMap(iter.first, iter.second[0], iter.second[1]);
	}

	uint32_t RenderAPIOpenGL::GetNextVAOIndex()
	{
		uint32_t length = (uint32_t)OpenGLVAOArray.size();

		for (uint32_t i = 0; i < length; i++)
		{
			if (!OpenGLVAOArray[i]->inUse)
				return i;
		}

		OpenGLVAOArray.push_back(new OpenGLVAO());

		return length;
	}

	OpenGLVAO* RenderAPIOpenGL::GetVAOByIndex(uint32_t idx)
	{
		return OpenGLVAOArray[idx];
	}

	uint32_t RenderAPIOpenGL::GetNextMaterialDataIndex()
	{
		uint32_t length = (uint32_t)OpenGLMaterialDataArray.size();

		for (uint32_t i = 0; i < length; i++)
		{
			if (!OpenGLMaterialDataArray[i]->inUse)
				return i;
		}

		OpenGLMaterialDataArray.push_back(new OpenGLMaterialData());

		return length;
	}

	OpenGLMaterialData* RenderAPIOpenGL::GetMaterialDataByIndex(uint32_t idx)
	{
		return OpenGLMaterialDataArray[idx];
	}
}