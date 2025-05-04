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

	struct OpenGLFBO
	{
		uint32_t colorBuffer = UINT32_MAX;
		uint32_t depthBuffer = UINT32_MAX;
		uint32_t positionBuffer = UINT32_MAX;
		uint32_t normalBuffer = UINT32_MAX;
		ClearInfo clearInfo = {};
	};

	struct OpenGLMaterialData
	{
		unordered_map<string, bool> boolList;
		unordered_map<string, float> floatList;
		unordered_map<string, int32_t> intList;
		unordered_map<string, uint32_t> uintList;
		unordered_map<string, Vector2> vec2List;
		unordered_map<string, Vector3> vec3List;
		unordered_map<string, Vector4> vec4List;
		unordered_map<string, pair<const Vector4*, uint32_t>> vec4ArrayList;
		unordered_map<string, Matrix3> mat3List;
		unordered_map<string, Matrix4> mat4List;
		unordered_map<string, pair<const Matrix4*, uint32_t>> mat4ArrayList;
		unordered_map<string, array<uint32_t, 2>> textures;
		unordered_map<string, array<uint32_t, 2>> cubeMaps;
		bool inUse = false;
	};
}