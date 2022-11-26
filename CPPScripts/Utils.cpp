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

    // ���������GLM��LookAt����Ч����һ����
    glm::mat4 Utils::GetLookToMatrix(glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
    {
        // ѧUnity�õ���������ϵ��up���forward�õ�right����������ϵ�÷�����
        glm::vec3 right = glm::cross(up, forward);

        // ����GLM��������Ϊ����ģ�ƽʱ�����ϵľ���������Ϊ����ģ��������￴��������ת�ù���һ������ʵ���viewMat������Ӧ���������ģ�
        //  Right.x,  Right.y,  Right.z, 0,
        //  Up.x,     Up.y,     Up.z,    0,
        //  Front.x,  Front.y,  Front.z, 0,
        //  0,        0,        0,       1
        // �����posMatͬ��
        // ������������ϵ����View���������forwardӦ�������ģ���������ϵ�Ǹ���
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