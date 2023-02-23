#include "Utils.h"
#include <algorithm>

namespace ZXEngine
{
	std::vector<std::string> Utils::StringSplit(const std::string& str, char p)
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

    std::vector<std::string> Utils::ExtractWords(const std::string& str)
    {
        std::vector<std::string> words;

        size_t s = 0, e = 0;
        bool record = false;
        for (size_t i = 0; i < str.size(); i++)
        {
            if (record && (str[i] == ' '))
            {
                words.push_back(str.substr(s, i - s));
                record = false;
            }
            else if (record && (i == str.size() - 1))
            {
                words.push_back(str.substr(s, i - s + 1));
                record = false;
            }
            else if (!record && str[i] != ' ')
            {
                s = i;
                record = true;
            }
        }

        return words;
    }

    std::string Utils::ConvertPathToWindowsFormat(std::string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        return path;
    }
}