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

    bool Utils::IsValidWordChar(char c)
    {
        return isalnum(c) or c == '_';
    }

    size_t Utils::FindWord(const std::string& str, const std::string& word, size_t offset)
    {
        size_t pos = str.find(word, offset);
        size_t strSize = str.size();
        size_t wordSize = word.size();
        while (pos != std::string::npos)
        {
            // 同时满足以下两个条件，说明这个单词是一个独立的单词，而不是另一个单词的一部分:
            // 1, 单词前一个位置无字符，或者字符不是合法单词字符
            // 2, 单词后一个位置无字符，或者字符不是合法单词字符
            if ((pos == 0 || (pos > 0 && !IsValidWordChar(str[pos - 1]))) && ((pos + wordSize == strSize) || !IsValidWordChar(str[pos + wordSize])))
                return pos;

            offset = pos + wordSize;
            pos = str.find(word, offset);
        }
        return std::string::npos;
    }

    void Utils::ReplaceAllWord(std::string& oriStr, const std::string& srcWord, const std::string& dstWord)
    {
        size_t offset = 0;
        size_t pos = FindWord(oriStr, srcWord, 0);
        size_t srcL = srcWord.length();
        size_t dstL = dstWord.length();
        while (pos != std::string::npos)
        {
            oriStr.replace(pos, srcL, dstWord);
            offset = pos + dstL;
            pos = FindWord(oriStr, srcWord, offset);
        }
    }

    void Utils::ReplaceAllString(std::string& oriStr, const std::string& srcStr, const std::string& dstStr)
    {
        size_t offset = 0;
        size_t pos = oriStr.find(srcStr, 0);
        size_t srcL = srcStr.length();
        size_t dstL = dstStr.length();
        while (pos != std::string::npos)
        {
            oriStr.replace(pos, srcL, dstStr);
            offset = pos + dstL;
            pos = oriStr.find(srcStr, offset);
        }
    }

    void Utils::GetNextStringBlockPos(std::string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos)
    {
        int level = 0;
        for (size_t i = offset; i < oriStr.size(); i++)
        {
            if (oriStr[i] == sChar)
            {
                level++;
                if (level == 1)
                {
                    sPos = i;
                }
            }
            else if (oriStr[i] == eChar)
            {
                level--;
                if (level == 0)
                {
                    ePos = i;
                    break;
                }
            }
        }
    }

    std::string Utils::ConcatenateStrings(const std::vector<std::string>& strings)
    {
        std::string res = "";
        for (auto& str : strings)
            res += str;
        return res;
    }

    std::string Utils::ConvertPathToWindowsFormat(std::string path)
    {
        std::replace(path.begin(), path.end(), '/', '\\');
        return path;
    }
}