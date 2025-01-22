#pragma once
#include <sstream>
#include <android/log.h>

extern std::ostream aOut;
extern std::ostream aOutWarn;
extern std::ostream aOutError;

class AndroidOut: public std::stringbuf
{
public:
    inline AndroidOut(const char* kLogTag) : logTag_(kLogTag) {}

protected:
    virtual int sync() override
    {
        __android_log_print(ANDROID_LOG_DEBUG, logTag_, "%s", str().c_str());
        str("");
        return 0;
    }

private:
    const char* logTag_;
};

class AndroidOutWarn: public std::stringbuf
{
public:
    inline AndroidOutWarn(const char* kLogTag) : logTag_(kLogTag) {}

protected:
    virtual int sync() override
    {
        __android_log_print(ANDROID_LOG_WARN, logTag_, "%s", str().c_str());
        str("");
        return 0;
    }

private:
    const char* logTag_;
};

class AndroidOutError: public std::stringbuf
{
public:
    inline AndroidOutError(const char* kLogTag) : logTag_(kLogTag) {}

protected:
    virtual int sync() override
    {
        __android_log_print(ANDROID_LOG_ERROR, logTag_, "%s", str().c_str());
        str("");
        return 0;
    }

private:
    const char* logTag_;
};