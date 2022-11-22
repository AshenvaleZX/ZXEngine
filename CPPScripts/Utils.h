#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace ZXEngine
{
	class Utils
	{
	public:
		static std::vector<std::string> StringSplit(std::string oriStr, char p);
		static std::string Vec2ToString(glm::vec2 v);
		static std::string Vec3ToString(glm::vec3 v);
		static std::string Vec4ToString(glm::vec4 v);
		// ���������GLM��LookAt����Ч����һ����
		static glm::mat4 GetLookToMatrix(glm::vec3 pos, glm::vec3 forward, glm::vec3 up);
	};
}