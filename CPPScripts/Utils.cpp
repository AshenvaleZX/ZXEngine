#include "Utils.h"

#if defined(ZX_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(ZX_PLATFORM_MACOS)
#include <mach-o/dyld.h>
#include <limits.h>
#elif defined(ZX_PLATFORM_LINUX)
#include <unistd.h>
#include <limits.h>
#endif

namespace ZXEngine
{
	vector<string> Utils::StringSplit(const string& str, char p)
	{
        size_t previous = 0;
        size_t current = str.find(p);
        vector<string> elems;
        while (current != string::npos) 
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

    vector<string> Utils::ExtractWords(const string& str)
    {
        vector<string> words;

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

    size_t Utils::FindWord(const string& str, const string& word, size_t offset)
    {
        size_t pos = str.find(word, offset);
        size_t strSize = str.size();
        size_t wordSize = word.size();
        while (pos != string::npos)
        {
            // 同时满足以下两个条件，说明这个单词是一个独立的单词，而不是另一个单词的一部分:
            // 1, 单词前一个位置无字符，或者字符不是合法单词字符
            // 2, 单词后一个位置无字符，或者字符不是合法单词字符
            if ((pos == 0 || (pos > 0 && !IsValidWordChar(str[pos - 1]))) && ((pos + wordSize == strSize) || !IsValidWordChar(str[pos + wordSize])))
                return pos;

            offset = pos + wordSize;
            pos = str.find(word, offset);
        }
        return string::npos;
    }

    void Utils::ReplaceAllWord(string& oriStr, const string& srcWord, const string& dstWord)
    {
        size_t offset = 0;
        size_t pos = FindWord(oriStr, srcWord, 0);
        size_t srcL = srcWord.length();
        size_t dstL = dstWord.length();
        while (pos != string::npos)
        {
            oriStr.replace(pos, srcL, dstWord);
            offset = pos + dstL;
            pos = FindWord(oriStr, srcWord, offset);
        }
    }

    void Utils::ReplaceAllString(string& oriStr, const string& srcStr, const string& dstStr)
    {
        size_t offset = 0;
        size_t pos = oriStr.find(srcStr, 0);
        size_t srcL = srcStr.length();
        size_t dstL = dstStr.length();
        while (pos != string::npos)
        {
            oriStr.replace(pos, srcL, dstStr);
            offset = pos + dstL;
            pos = oriStr.find(srcStr, offset);
        }
    }

    void Utils::GetNextStringBlockPos(string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos)
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

    string Utils::ConcatenateStrings(const vector<string>& strings)
    {
        string res = "";
        for (auto& str : strings)
            res += str;
        return res;
    }

    string Utils::SecondsToString(float seconds)
    {
        uint32_t sec_uint32 = static_cast<uint32_t>(seconds * 1000);

        if (sec_uint32 < 60'000)
        {
            uint32_t frac = sec_uint32 % 1000;
            uint32_t secs = sec_uint32 / 1000;

            string fracStr;
            if (frac < 10)
                fracStr = "00" + to_string(frac);
            else if (frac < 100)
                fracStr = "0" + to_string(frac);
            else
                fracStr = to_string(frac);

            string secsStr = (secs < 10 ? "0" : "") + to_string(secs);

            return "00:00:" + secsStr + "." + fracStr;
        }
        else
        {
            return SecondsToString(sec_uint32 / 100);
        }
    }

    string Utils::SecondsToString(uint32_t seconds)
    {
        uint32_t hours = seconds / 3600;
        uint32_t minutes = (seconds % 3600) / 60;
        uint32_t secs = seconds % 60;

        string hoursStr = (hours < 10 ? "0" : "") + to_string(hours);
        string minutesStr = (minutes < 10 ? "0" : "") + to_string(minutes);
        string secsStr = (secs < 10 ? "0" : "") + to_string(secs);

        return hoursStr + ":" + minutesStr + ":" + secsStr;
    }

    string Utils::MillisecondsToString(uint32_t milliseconds)
    {
		uint32_t seconds = milliseconds / 1000;
        uint32_t frac = milliseconds % 1000;

        string fracStr;
        if (frac < 10)
            fracStr = "00" + to_string(frac);
        else if (frac < 100)
            fracStr = "0" + to_string(frac);
        else
            fracStr = to_string(frac);

        return SecondsToString(seconds) + "." + fracStr;
    }

    string Utils::DataSizeToString(uint64_t dataSize)
    {
        if (dataSize < 1024)
        {
            return to_string(dataSize) + "B";
        }

        string unit;
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
            return to_string(dataSize / stepSize) + unit;
        else if (percentage_u32 < 10)
            return to_string(dataSize / stepSize) + ".0" + to_string(percentage_u32) + unit;
        else
            return to_string(dataSize / stepSize) + "." + to_string(percentage_u32) + unit;
	}

    string Utils::GetCurrentExecutableFilePath()
    {
#if defined(ZX_PLATFORM_WINDOWS)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
#elif defined(ZX_PLATFORM_MACOS)
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0)
            return "";
#elif defined(ZX_PLATFORM_LINUX)
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (count == -1)
            return "";
        buffer[count] = '\0';
#else
        return "";
#endif

        filesystem::path path(buffer);
        if (filesystem::is_directory(path))
			return path.string();
		else
			return path.parent_path().string();
	}
}