#include "Utils.h"

namespace ZXEngine
{
	std::vector<std::string> Utils::StringSplit(std::string str, char p)
	{
        std::size_t previous = 0;
        std::size_t current = str.find(p);
        std::vector<std::string> elems;
        while (current != std::string::npos) 
        {
            if (current > previous) 
            {
                elems.push_back(str.substr(previous, current - previous));
            }
            previous = current + 1;
            current = str.find(p, previous);
        }
        if (previous != str.size()) 
        {
            elems.push_back(str.substr(previous));
        }
        return elems;
	}

    std::string Utils::Vec2ToString(glm::vec2 v)
    {
        return "x " + std::to_string(v.x) + " y " + std::to_string(v.y);
    }

    std::string Utils::Vec3ToString(glm::vec3 v)
    {
        return "x " + std::to_string(v.x) + " y " + std::to_string(v.y) + " z " + std::to_string(v.z);
    }

    std::string Utils::Vec4ToString(glm::vec4 v)
    {
        return "x " + std::to_string(v.x) + " y " + std::to_string(v.y) + " z " + std::to_string(v.z) + " w " + std::to_string(v.w);
    }

    // 这个函数和GLM的LookAt函数效果是一样的
    glm::mat4 Utils::GetLookToMatrix(glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
    {
        // 学Unity用的左手坐标系，up叉乘forward得到right，右手坐标系得反过来
        glm::vec3 right = glm::cross(up, forward);

        // 由于GLM里是以列为主序的，平时书面上的矩阵都是以行为主序的，所以这里看起来像是转置过了一样，其实这个viewMat看起来应该是这样的：
        //  Right.x,  Right.y,  Right.z, 0,
        //  Up.x,     Up.y,     Up.z,    0,
        //  Front.x,  Front.y,  Front.z, 0,
        //  0,        0,        0,       1
        // 后面的posMat同理
        // 基于左手坐标系构建View矩阵这里的forward应该是正的，右手坐标系是负的
        glm::mat4 viewMat = glm::mat4(
            right.x, up.x, forward.x, 0,
            right.y, up.y, forward.y, 0,
            right.z, up.z, forward.z, 0,
            0, 0, 0, 1);
        glm::mat4 posMat = glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -pos.x, -pos.y, -pos.z, 1);

        return viewMat * posMat;
    }
}