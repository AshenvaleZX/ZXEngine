#include "RenderAPIOpenGL.h"
#include "GlobalData.h"

namespace ZXEngine
{
	void RenderAPIOpenGL::InitRenderSetting()
	{
		// 获取当前OpenGL版本并输出
		int majorVersion;
		int minorVersion;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		Debug::Log("Graphic API: OpenGL");
		Debug::Log("Version: " + to_string(majorVersion) + "." + to_string(minorVersion));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RenderAPIOpenGL::EnableDepthTest(bool enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void RenderAPIOpenGL::EnableDepthWrite(bool enable)
	{
		if (enable)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}

	void RenderAPIOpenGL::SwitchFrameBuffer(unsigned int id)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id);
	}

	void RenderAPIOpenGL::ClearFrameBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void RenderAPIOpenGL::ClearColorBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderAPIOpenGL::ClearDepthBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void RenderAPIOpenGL::ClearStencilBuffer()
	{
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

	unsigned int RenderAPIOpenGL::LoadTexture(const char* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
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
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // 纹理的第三个轴，类似普通点坐标的z，因为cubemap是3D的纹理

		return textureID;
	}

	ShaderInfo RenderAPIOpenGL::LoadAndCompileShader(const char* path)
	{
		ShaderInfo info{};
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

			int hasDirLight = shaderCode.find("DirLight");
			int hasPointLight = shaderCode.find("PointLight");
			if (hasDirLight > 0)
				info.lightType = LightType::Directional;
			else if (hasPointLight > 0)
				info.lightType = LightType::Point;
			else
				info.lightType = LightType::None;

			int vs_begin = shaderCode.find("#vs_begin") + 9;
			int vs_end = shaderCode.find("#vs_end");
			vertexCode = shaderCode.substr(vs_begin, vs_end - vs_begin);

			int gs_begin = shaderCode.find("#gs_begin") + 9;
			int gs_end = shaderCode.find("#gs_end");
			geometryCode = shaderCode.substr(gs_begin, gs_end - gs_begin);

			int fs_begin = shaderCode.find("#fs_begin") + 9;
			int fs_end = shaderCode.find("#fs_end");
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

		info.ID = ID;

		return info;
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
		width = width == 0 ? GlobalData::srcWidth : 1;
		height = height == 0 ? GlobalData::srcHeight : 1;
		FrameBufferObject* FBO = new FrameBufferObject();
		if (type == FrameBufferType::Normal)
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
				Debug::LogError("Framebuffer not complete!");
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
				Debug::LogError("Framebuffer not complete!");
			// 恢复默认状态
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// 对FBO对象赋值
			FBO->ID = FBO_ID;
			FBO->ColorBuffer = colorBuffer;
			FBO->DepthBuffer = depthBuffer;
		}

		return FBO;
	}

	void RenderAPIOpenGL::Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, primitiveSize, GL_UNSIGNED_INT, 0);
		// 绘制完重置一下(不重置也行，不过及时重置避免出问题)
		glBindVertexArray(0);
	}

	// Mesh设置
	void RenderAPIOpenGL::SetMesh(unsigned int VAO, unsigned int size)
	{
		this->VAO = VAO;
		this->primitiveSize = size;
	}

	void RenderAPIOpenGL::SetUpMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices)
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
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

		// 设置完记得还原一下
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
	void RenderAPIOpenGL::SetShaderVec2(unsigned int ID, string name, vec2 value)
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void RenderAPIOpenGL::SetShaderVec2(unsigned int ID, string name, float x, float y)
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void RenderAPIOpenGL::SetShaderVec3(unsigned int ID, string name, vec3 value)
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void RenderAPIOpenGL::SetShaderVec3(unsigned int ID, string name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void RenderAPIOpenGL::SetShaderVec4(unsigned int ID, string name, vec4 value)
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void RenderAPIOpenGL::SetShaderVec4(unsigned int ID, string name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void RenderAPIOpenGL::SetShaderMat2(unsigned int ID, string name, mat2 mat)
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void RenderAPIOpenGL::SetShaderMat3(unsigned int ID, string name, mat3 mat)
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void RenderAPIOpenGL::SetShaderMat4(unsigned int ID, string name, mat4 mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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
}