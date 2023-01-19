#include "RenderAPIOpenGL.h"
#include "GlobalData.h"
#include "RenderStateSetting.h"

namespace ZXEngine
{
	void RenderAPIOpenGL::InitRenderSetting()
	{
		// ��ȡ��ǰOpenGL�汾�����
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

		InitGLConstMap();
		targetState = new RenderStateSetting();
		curRealState = new RenderStateSetting();
	}
	
	void RenderAPIOpenGL::SetRenderState(RenderStateSetting* state)
	{
		*targetState = *state;
		stateDirty = true;
	}

	void RenderAPIOpenGL::EnableDepthTest(bool enable)
	{
		targetState->depthTest = enable;
		stateDirty = true;
	}

	void RenderAPIOpenGL::EnableDepthWrite(bool enable)
	{
		targetState->depthWrite = enable;
		stateDirty = true;
	}

	void RenderAPIOpenGL::SetBlendMode(BlendOption sfactor, BlendOption dfactor)
	{
		targetState->srcFactor = sfactor;
		targetState->dstFactor = dfactor;
		stateDirty = true;
	}

	void RenderAPIOpenGL::SetClearColor(const Vector4& color)
	{
		targetState->clearColor = color;
		stateDirty = true;
	}

	void RenderAPIOpenGL::EnableFaceCull(bool enable)
	{
		targetState->faceCull = enable;
		stateDirty = true;
	}

	void RenderAPIOpenGL::SetFaceCullMode(FaceCullOption mode)
	{
		targetState->faceCullOption = mode;
		stateDirty = true;
	}

	void RenderAPIOpenGL::SwitchFrameBuffer(unsigned int id)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id);
	}

	void RenderAPIOpenGL::SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset)
	{
		glViewport(xOffset, yOffset, width, height);
	}

	void RenderAPIOpenGL::ClearFrameBuffer()
	{
		ClearColorBuffer();
		ClearDepthBuffer();
		ClearStencilBuffer();
	}

	void RenderAPIOpenGL::ClearColorBuffer()
	{
		ClearColorBuffer(targetState->clearColor);
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
	}

	void RenderAPIOpenGL::ClearDepthBuffer()
	{
		ClearDepthBuffer(targetState->clearDepth);
	}

	void RenderAPIOpenGL::ClearDepthBuffer(float depth)
	{
		if (curRealState->clearDepth != depth)
		{
			glClearDepth(depth);
			curRealState->clearDepth = depth;
			stateDirty = true;
		}
		// Clear Depth֮ǰ���뿪����Ȳ��Ժ�д��
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
	}

	void RenderAPIOpenGL::ClearStencilBuffer()
	{
		ClearStencilBuffer(targetState->clearStencil);
	}

	void RenderAPIOpenGL::ClearStencilBuffer(int stencil)
	{
		if (curRealState->clearStencil != stencil)
		{
			glClearStencil(stencil);
			curRealState->clearStencil = stencil;
			stateDirty = true;
		}
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void RenderAPIOpenGL::CheckError()
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

		return textureID;
	}

	void RenderAPIOpenGL::DeleteTexture(unsigned int id)
	{
		glDeleteTextures(1, &id);
	}

	unsigned int RenderAPIOpenGL::LoadCubeMap(vector<string> faces)
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
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // ����ĵ������ᣬ������ͨ�������z����Ϊcubemap��3D������

		return textureID;
	}

	unsigned int RenderAPIOpenGL::GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data)
	{
		// ͨ���������ɵ�λͼ��һ��8λ�Ҷ�ͼ������ÿһ����ɫ����һ���ֽ�����ʾ�����������Ҫ��λͼ�����ÿһ�ֽڶ���Ϊ�������ɫֵ��
		// ��������ÿһ�ֽڶ���Ӧ��������ɫ�ĺ�ɫ����(��ɫ�����ĵ�һ���ֽ�)��������ʹ��һ���ֽ�����ʾ�������ɫ��
		// ����OpenGLĬ�����е�������4�ֽڶ���ģ�������Ĵ�С��Զ��4�ֽڵı�������Ϊ�󲿷�����Ŀ�ȶ�Ϊ4�ı�����/��ÿ����ʹ��4���ֽڡ�
		// ������������ÿ������ֻ����һ���ֽڣ�������������Ŀ�ȣ�������Ҫ�������ѹ���������Ϊ1����������ȷ�������ж������⡣
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// ��������
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// ��ԭĬ������
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return textureID;
	}

	ShaderInfo* RenderAPIOpenGL::LoadAndCompileShader(const char* path)
	{
		ShaderInfo* info = new ShaderInfo();
		string shaderCode;
		string vertexCode;
		string fragmentCode;
		string geometryCode;
		ifstream shaderFile;
		// ensure ifstream objects can throw exceptions:
		shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			// open files
			shaderFile.open(path);
			stringstream shaderStream;
			// read file's buffer contents into streams
			shaderStream << shaderFile.rdbuf();
			// close file handlers
			shaderFile.close();
			// convert stream into string
			shaderCode = shaderStream.str();

			// �������������õ���string�����ר�����ͣ���Ϊstring���find��substr�Ȳ������ص���Щ�������ͺ;�����뻷���й�
			// �ر���find�����Ϻܶ�ط�˵û�ҵ��ͻ᷵��-1����ʵ���˵����׼ȷ����Ϊfind�ĺ������巵�ص�������size_t
			// ��size_t��һ���޷�������(�������λȡ���ڱ��뻷��)��һ���޷����������-1������Ϊ����ˣ�ʵ����û�ҵ���ʱ���������ص���npos
			// ��ʵֱ����int������Ҳ�У����Զ���ʽת����Ҳ������-1���ж��Ƿ��ҵ��������������б����Warning
			// Ϊ���ڸ��ֱ��뻷���²���������ֱ�Ӳ���ԭ�����е�string::size_type��string::npos����յģ����Ҳ�����Warning
			string::size_type hasDirLight = shaderCode.find("DirLight");
			string::size_type hasPointLight = shaderCode.find("PointLight");
			if (hasDirLight != string::npos)
				info->lightType = LightType::Directional;
			else if (hasPointLight != string::npos)
				info->lightType = LightType::Point;
			else
				info->lightType = LightType::None;

			string::size_type hasDirShadow = shaderCode.find("_DepthMap");
			string::size_type hasPointShadow = shaderCode.find("_DepthCubeMap");
			if (hasDirShadow != string::npos)
				info->shadowType = ShadowType::Directional;
			else if (hasPointShadow != string::npos)
				info->shadowType = ShadowType::Point;
			else
				info->shadowType = ShadowType::None;

			string::size_type vs_begin = shaderCode.find("#vs_begin") + 9;
			string::size_type vs_end = shaderCode.find("#vs_end");
			vertexCode = shaderCode.substr(vs_begin, vs_end - vs_begin);

			string::size_type gs_begin = shaderCode.find("#gs_begin") + 9;
			string::size_type gs_end = shaderCode.find("#gs_end");
			geometryCode = shaderCode.substr(gs_begin, gs_end - gs_begin);

			string::size_type fs_begin = shaderCode.find("#fs_begin") + 9;
			string::size_type fs_end = shaderCode.find("#fs_end");
			fragmentCode = shaderCode.substr(fs_begin, fs_end - fs_begin);
		}
		catch (ifstream::failure e)
		{
			Debug::LogError("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = geometryCode.c_str();

		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry = 0;
		if (geometryCode.length() > 1)
		{
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		unsigned int ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryCode.length() > 1)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryCode.length() > 1)
			glDeleteShader(geometry);

		info->ID = ID;

		return info;
	}

	void RenderAPIOpenGL::DeleteShaderProgram(unsigned int id)
	{
		glDeleteProgram(id);
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
	}

	FrameBufferObject* RenderAPIOpenGL::CreateFrameBufferObject(FrameBufferType type, unsigned int width, unsigned int height)
	{
		width = width == 0 ? GlobalData::srcWidth : width;
		height = height == 0 ? GlobalData::srcHeight : height;
		FrameBufferObject* FBO = new FrameBufferObject(type);
		if (type == FrameBufferType::Normal)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);
			// ����ColorBuffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// ����DepthBuffer
			// (�õ�Renderbuffer������Texture��Renderbufferһ�����ڲ���ȡ��ֻд����߸��Ƶ�buffer��������Ȼ��������ʺ���Renderbuffer)
			unsigned int depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			// ��ColorBuffer��DepthBuffer�󶨵�FBO��
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer Normal not complete!");
			// �ָ�Ĭ��״̬
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// ��FBO����ֵ
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = depthBuffer;
		}
		else if (type == FrameBufferType::HigthPrecision)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);
			// �����߾���(����)ColorBuffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// ����DepthBuffer
			// (�õ�Renderbuffer������Texture��Renderbufferһ�����ڲ���ȡ��ֻд����߸��Ƶ�buffer��������Ȼ��������ʺ���Renderbuffer)
			unsigned int depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			// ��ColorBuffer��DepthBuffer�󶨵�FBO��
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer HigthPrecision not complete!");
			// �ָ�Ĭ��״̬
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// ��FBO����ֵ
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = depthBuffer;
		}
		else if (type == FrameBufferType::Color)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);
			// ����ColorBuffer
			unsigned int colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// ��ColorBuffer�󶨵�FBO��
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer Color not complete!");
			// �ָ�Ĭ��״̬
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// ��FBO����ֵ
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = NULL;
		}
		else if (type == FrameBufferType::ShadowMap)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);
			// �������Map
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
			// �����Map�󶨵�FBO
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
			// ��ȷ����OpenGL���FBO������Ⱦ��Color Buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer ShadowMap not complete!");
			// �ָ�Ĭ��״̬
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// ��FBO����ֵ
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = NULL;
			FBO->DepthBuffer = depthMap;
		}
		else if (type == FrameBufferType::ShadowCubeMap)
		{
			unsigned int FBO_ID;
			glGenFramebuffers(1, &FBO_ID);
			// �������CubeMap
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
			// �����CubeMap�󶨵�FBO
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
			// ��ȷ����OpenGL���FBO������Ⱦ��Color Buffer
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::LogError("Framebuffer ShadowCubeMap not complete!");
			// �ָ�Ĭ��״̬
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// ��FBO����ֵ
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = NULL;
			FBO->DepthBuffer = depthCubeMap;
		}
		else
		{
			Debug::LogError("Invalide frame buffer type.");
		}

		return FBO;
	}

	unsigned int RenderAPIOpenGL::GenerateParticleMesh()
	{
		unsigned int VAO;
		unsigned int VBO;
		float particleQuad[] = {
			// pos        // tex coord
			-0.5f,  0.5f, 0.0f, 1.0f,
			 0.5f, -0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f,  0.5f, 0.0f, 1.0f,
			 0.5f,  0.5f, 1.0f, 1.0f,
			 0.5f, -0.5f, 1.0f, 0.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		// Fill mesh buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);
		// Set mesh attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);

		return VAO;
	}

	void RenderAPIOpenGL::DeleteBuffer(unsigned int id)
	{
		glDeleteBuffers(1, &id);
	}

	void RenderAPIOpenGL::Draw()
	{
		UpdateRenderState();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, primitiveSize, GL_UNSIGNED_INT, 0);
		// ����������һ��(������Ҳ�У�������ʱ���ñ��������)
		glBindVertexArray(0);

#ifdef ZX_DEBUG
		Debug::drawCallCount++;
#endif
	}

	void RenderAPIOpenGL::Draw(unsigned int VAO, unsigned int size, DrawType type)
	{
		UpdateRenderState();
		
		glBindVertexArray(VAO);

		if (type == DrawType::OpenGLDrawArrays)
			glDrawArrays(GL_TRIANGLES, 0, size);
		else if (type == DrawType::OpenGLDrawElements)
			glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
		else
			Debug::LogError("Wrong draw type: " + to_string((int)type));

		glBindVertexArray(0);

#ifdef ZX_DEBUG
		Debug::drawCallCount++;
#endif
	}

	// Mesh����
	void RenderAPIOpenGL::SetMesh(unsigned int VAO, unsigned int size)
	{
		this->VAO = VAO;
		this->primitiveSize = size;
	}

	void RenderAPIOpenGL::DeleteMesh(unsigned int VAO)
	{
		glDeleteVertexArrays(1, &VAO);
	}

	void RenderAPIOpenGL::SetUpStaticMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices)
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a Vector3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		// ������ǵû�ԭһ��
		glBindVertexArray(0);
	}

	void RenderAPIOpenGL::SetUpDynamicMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, unsigned int vertexSize, unsigned int indexSize)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// ����ΪGL_DYNAMIC_DRAW��������ʱ����ʼ�����ݣ�����ͨ��UpdateDynamicMesh��������
		glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}

	void RenderAPIOpenGL::UpdateDynamicMesh(unsigned int VAO, unsigned int VBO, unsigned int EBO, vector<Vertex> vertices, vector<unsigned int> indices)
	{
		// �л���ָ��VAO
		glBindVertexArray(VAO);

		// ����VBO����
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);

		// ����EBO����
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);

		// ��ԭ
		glBindVertexArray(0);
	}

	void RenderAPIOpenGL::UseShader(unsigned int ID)
	{
		glUseProgram(ID);
	}
	void RenderAPIOpenGL::SetShaderBool(unsigned int ID, string name, bool value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void RenderAPIOpenGL::SetShaderInt(unsigned int ID, string name, int value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void RenderAPIOpenGL::SetShaderFloat(unsigned int ID, string name, float value)
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void RenderAPIOpenGL::SetShaderVec2(unsigned int ID, string name, const Vector2& value)
	{
		float* array = new float[2];
		value.ToArray(array);
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, array);
		delete[] array;
	}
	void RenderAPIOpenGL::SetShaderVec2(unsigned int ID, string name, float x, float y)
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void RenderAPIOpenGL::SetShaderVec3(unsigned int ID, string name, const Vector3& value)
	{
		float* array = new float[3];
		value.ToArray(array);
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, array);
		delete[] array;
	}
	void RenderAPIOpenGL::SetShaderVec3(unsigned int ID, string name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void RenderAPIOpenGL::SetShaderVec4(unsigned int ID, string name, const Vector4& value)
	{
		float* array = new float[4];
		value.ToArray(array);
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, array);
		delete[] array;
	}
	void RenderAPIOpenGL::SetShaderVec4(unsigned int ID, string name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void RenderAPIOpenGL::SetShaderMat3(unsigned int ID, string name, const Matrix3& mat)
	{
		float* array = new float[9];
		mat.ToColumnMajorArray(array);
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, array);
		delete[] array;
	}
	void RenderAPIOpenGL::SetShaderMat4(unsigned int ID, string name, const Matrix4& mat)
	{
		float* array = new float[16];
		mat.ToColumnMajorArray(array);
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, array);
		delete[] array;
	}
	void RenderAPIOpenGL::SetShaderTexture(unsigned int ID, string name, unsigned int textureID, unsigned int idx)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), idx);
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	void RenderAPIOpenGL::SetShaderCubeMap(unsigned int ID, string name, unsigned int textureID, unsigned int idx)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), idx);
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
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
			glBlendFunc(BlendMap[targetState->srcFactor], BlendMap[targetState->dstFactor]);

		if (curRealState->faceCull != targetState->faceCull)
		{
			if (targetState->faceCull)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		if (curRealState->faceCullOption != targetState->faceCullOption)
			glCullFace(FaceCullMap[targetState->faceCullOption]);

		if (curRealState->clearColor != targetState->clearColor)
			glClearColor(targetState->clearColor.r, targetState->clearColor.g, targetState->clearColor.b, targetState->clearColor.a);

		if (curRealState->clearDepth != targetState->clearDepth)
			glClearDepth(targetState->clearDepth);

		if (curRealState->clearStencil != targetState->clearStencil)
			glClearStencil(targetState->clearStencil);

		*curRealState = *targetState;
	}

	void RenderAPIOpenGL::InitGLConstMap()
	{
		BlendMap =
		{
			{ BlendOption::ZERO,					GL_ZERO						},
			{ BlendOption::ONE,						GL_ONE						},
			{ BlendOption::SRC_COLOR,				GL_SRC_COLOR				},
			{ BlendOption::ONE_MINUS_SRC_COLOR,		GL_ONE_MINUS_SRC_COLOR		},
			{ BlendOption::DST_COLOR,				GL_DST_COLOR				},
			{ BlendOption::ONE_MINUS_DST_COLOR,		GL_ONE_MINUS_DST_COLOR		},
			{ BlendOption::SRC_ALPHA,				GL_SRC_ALPHA				},
			{ BlendOption::ONE_MINUS_SRC_ALPHA,		GL_ONE_MINUS_SRC_ALPHA		},
			{ BlendOption::DST_ALPHA,				GL_DST_ALPHA				},
			{ BlendOption::ONE_MINUS_DST_ALPHA,		GL_ONE_MINUS_DST_ALPHA		},
			{ BlendOption::CONSTANT_COLOR,			GL_CONSTANT_COLOR			},
			{ BlendOption::ONE_MINUS_CONSTANT_COLOR,GL_ONE_MINUS_CONSTANT_COLOR },
			{ BlendOption::CONSTANT_ALPHA,			GL_CONSTANT_ALPHA			},
			{ BlendOption::ONE_MINUS_CONSTANT_ALPHA,GL_ONE_MINUS_CONSTANT_ALPHA	},
		};

		FaceCullMap =
		{
			{ FaceCullOption::Back,			GL_BACK			  },
			{ FaceCullOption::Front,		GL_FRONT		  },
			{ FaceCullOption::FrontAndBack,	GL_FRONT_AND_BACK },
		};
	}
}