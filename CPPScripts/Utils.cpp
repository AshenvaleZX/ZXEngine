#include "Utils.h"
#include <filesystem>

#ifdef _WIN64
#include <Windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif

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

    std::string Utils::SecondsToString(float seconds)
    {
        uint32_t sec_uint32 = static_cast<uint32_t>(seconds * 1000);

        if (sec_uint32 < 60'000)
        {
            uint32_t frac = sec_uint32 % 1000;
            uint32_t secs = sec_uint32 / 1000;

            std::string fracStr;
            if (frac < 10)
                fracStr = "00" + std::to_string(frac);
            else if (frac < 100)
                fracStr = "0" + std::to_string(frac);
            else
                fracStr = std::to_string(frac);

            std::string secsStr = (secs < 10 ? "0" : "") + std::to_string(secs);

            return "00:00:" + secsStr + "." + fracStr;
        }
        else
        {
            return SecondsToString(sec_uint32 / 100);
        }
    }

    std::string Utils::SecondsToString(uint32_t seconds)
    {
        uint32_t hours = seconds / 3600;
        uint32_t minutes = (seconds % 3600) / 60;
        uint32_t secs = seconds % 60;

        std::string hoursStr = (hours < 10 ? "0" : "") + std::to_string(hours);
        std::string minutesStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
        std::string secsStr = (secs < 10 ? "0" : "") + std::to_string(secs);

        return hoursStr + ":" + minutesStr + ":" + secsStr;
    }

    std::string Utils::MillisecondsToString(uint32_t milliseconds)
    {
		uint32_t seconds = milliseconds / 1000;
        uint32_t frac = milliseconds % 1000;

        std::string fracStr;
        if (frac < 10)
            fracStr = "00" + std::to_string(frac);
        else if (frac < 100)
            fracStr = "0" + std::to_string(frac);
        else
            fracStr = std::to_string(frac);

        return SecondsToString(seconds) + "." + fracStr;
    }

    std::string Utils::DataSizeToString(uint64_t dataSize)
    {
        if (dataSize < 1024)
        {
            return std::to_string(dataSize) + "B";
        }

        std::string unit;
        uint64_t stepSize;
        if (dataSize < 1024ULL * 1024ULL)
        {
            unit = "KB";
            stepSize = 1024ULL;
        }
        else if (dataSize < 1024ULL * 1024ULL * 1024ULL)
        {
			unit = "MB";
			stepSize = 1024ULL * 1024ULL;
        }
        else if (dataSize < 1024ULL * 1024ULL * 1024ULL * 1024ULL)
        {
            unit = "GB";
            stepSize = 1024ULL * 1024ULL * 1024ULL;
        }
        else
        {
			unit = "TB";
			stepSize = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
		}

        double percentage = static_cast<double>(dataSize % stepSize) / stepSize * 100;
        uint32_t percentage_u32 = static_cast<uint32_t>(percentage);

        if (percentage_u32 == 0)
            return std::to_string(dataSize / stepSize) + unit;
        else if (percentage_u32 < 10)
            return std::to_string(dataSize / stepSize) + ".0" + std::to_string(percentage_u32) + unit;
        else
            return std::to_string(dataSize / stepSize) + "." + std::to_string(percentage_u32) + unit;
	}

    std::string Utils::GetCurrentExecutableFilePath()
    {
#ifdef _WIN64
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
#elif __APPLE__
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0)
            return "";
#else
            return "";
#endif

        std::filesystem::path path(buffer);
        if (std::filesystem::is_directory(path))
			return path.string();
		else
			return path.parent_path().string();
	}
}