#include "Utils.h"

#if defined(ZX_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(ZX_PLATFORM_MACOS)
#include <mach-o/dyld.h>
#include <limits.h>
#include <CoreGraphics/CoreGraphics.h>
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

        for (auto& word : words)
            RemoveCRLF(word);

        return words;
    }

    void Utils::RemoveCRLF(string& str)
    {
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    }

    void Utils::RemoveSpace(string& str)
    {
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
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

    string Utils::GetNextWord(const string& str, size_t offset)
    {
        for (size_t i = offset + 1; i < str.size(); i++)
        {
            if (IsValidWordChar(str[i]))
            {
                size_t s = i;
                while (i < str.size() && IsValidWordChar(str[i]))
                    i++;
                return str.substr(s, i - s);
            }
        }
        return "";
    }

    string Utils::GetPreviousWord(const string& str, size_t offset)
    {
        for (int i = static_cast<int>(offset - 1); i >= 0; i--)
        {
            if (IsValidWordChar(str[i]))
            {
                int e = i;
                while (i >= 0 && IsValidWordChar(str[i]))
                    i--;
                return str.substr(static_cast<size_t>(i + 1), static_cast<size_t>(e - i));
            }
        }
        return "";
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

    string Utils::GetNextStringBlock(const string& oriStr, size_t offset, char sChar, char eChar, bool exclude)
    {
        size_t sPos = 0, ePos = 0;
        GetNextStringBlockPos(oriStr, offset, sChar, eChar, sPos, ePos);

        if (exclude)
        {
            sPos++;
            ePos--;
        }

		return oriStr.substr(sPos, ePos - sPos + 1);
    }

    string Utils::GetPreviousStringBlock(const string& oriStr, size_t offset, char sChar, char eChar, bool exclude)
    {
		size_t sPos = 0, ePos = 0;
		GetPreviousStringBlockPos(oriStr, offset, sChar, eChar, sPos, ePos);

        if (exclude)
        {
            sPos++;
            ePos--;
        }

		return oriStr.substr(sPos, ePos - sPos + 1);
    }

    void Utils::GetNextStringBlockPos(const string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos)
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

    void Utils::GetPreviousStringBlockPos(const string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos)
    {
        int level = 0;
        for (size_t i = offset; i > 0; i--)
        {
            if (oriStr[i] == eChar)
            {
                level++;
                if (level == 1)
                {
                    ePos = i;
                }
            }
            else if (oriStr[i] == sChar)
            {
                level--;
                if (level == 0)
                {
                    sPos = i;
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

    string Utils::RelativePathToAbsolutePath(const string& rPath)
    {
        filesystem::path path(rPath);
        if (path.is_absolute())
            return path.string();
        else
            return filesystem::absolute(path).string();
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
#if defined(ZX_PLATFORM_DESKTOP)
#   if defined(ZX_PLATFORM_WINDOWS)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
#   elif defined(ZX_PLATFORM_MACOS)
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0)
            return "";
#   elif defined(ZX_PLATFORM_LINUX)
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (count == -1)
            return "";
        buffer[count] = '\0';
#   endif
        filesystem::path path(buffer);
        if (filesystem::is_directory(path))
			return path.string();
		else
			return path.parent_path().string();
#else
        return "";
#endif
	}

    void Utils::OpenFileWithDefaultApplication(const string& path)
    {
        int ret = 0;
#if defined(ZX_PLATFORM_WINDOWS)
        ret = std::system(("start " + path).c_str());
#elif defined(ZX_PLATFORM_MACOS)
        ret = std::system(("open " + path).c_str());
#elif defined(ZX_PLATFORM_LINUX)
        ret = std::system(("xdg-open " + path).c_str());
#endif
        if (ret != 0)
            Debug::LogError("Failed to open file with default application: " + path);
    }

    void Utils::ShowSystemMessageBox(const string& title, const string& message)
    {
#if defined(ZX_PLATFORM_WINDOWS)
        MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
#elif defined(ZX_PLATFORM_MACOS)
        string command = "osascript -e 'display dialog \"" + message + "\" with title \"" + title + "\" buttons {\"OK\"} default button \"OK\"'";
        int ret = std::system(command.c_str());
#elif defined(ZX_PLATFORM_LINUX)
        string command = "zenity --info --title=\"" + title + "\" --text=\"" + message + "\"";
        int ret = std::system(command.c_str());
#endif
    }

    void Utils::GetScreenResolution(uint32_t& width, uint32_t& height)
    {
#if defined(ZX_PLATFORM_WINDOWS)
        /* 这种方式获取的是经过系统缩放后的分辨率，暂时不用
        int srcWidth = GetSystemMetrics(SM_CXSCREEN);
        int srcHeight = GetSystemMetrics(SM_CYSCREEN);
        */

        // 这种方式获取的是屏幕的实际物理分辨率
        HDC hdc = GetDC(NULL); // NULL表示获取整个屏幕而不是某个窗口的信息
        int srcWidth = GetDeviceCaps(hdc, DESKTOPHORZRES);
        int srcHeight = GetDeviceCaps(hdc, DESKTOPVERTRES);
        ReleaseDC(NULL, hdc);

        if (srcWidth > 0)
            width = static_cast<uint32_t>(srcWidth);
        if (srcHeight > 0)
            height = static_cast<uint32_t>(srcHeight);

#elif defined(ZX_PLATFORM_MACOS)
        CGDirectDisplayID displayID = CGMainDisplayID();
        width = static_cast<uint32_t>(CGDisplayPixelsWide(displayID));
        height = static_cast<uint32_t>(CGDisplayPixelsHigh(displayID));
#else
        width = 0;
        height = 0;
#endif
    }
}