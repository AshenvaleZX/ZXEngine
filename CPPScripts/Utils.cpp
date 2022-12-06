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
}