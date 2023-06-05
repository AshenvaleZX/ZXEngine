#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct OpenGLVAO
	{
		// Vertex Array Object
		uint32_t VAO = 0;
		// Vertex Buffer Objects
		uint32_t VBO = 0;
		// Element Buffer Objects
		uint32_t EBO = 0;

		uint32_t size = 0;
		bool indexed = true;
		bool inUse = false;
	};

	struct OpenGLMaterialData
	{
		map<string, int> intList;
		map<string, bool> boolList;
		map<string, float> floatList;
		map<string, uint32_t> uintList;
		map<string, Vector2> vec2List;
		map<string, Vector3> vec3List;
		map<string, Vector4> vec4List;
		map<string, Matrix3> mat3List;
		map<string, Matrix4> mat4List;
		map<string, array<uint32_t, 2>> textures;
		map<string, array<uint32_t, 2>> cubeMaps;
		bool inUse = false;
	};
}