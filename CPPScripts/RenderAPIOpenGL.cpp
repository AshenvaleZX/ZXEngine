#include "RenderAPIOpenGL.h"

namespace ZXEngine
{
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
}