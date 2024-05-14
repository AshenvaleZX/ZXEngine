#pragma once
#include "pubh.h"

namespace ZXEngine
{
    class EventManager
    {
    public:
        static void Create();
        static EventManager* GetInstance();

    private:
        static EventManager* mInstance;

    public:
        void FireEvent(int id, const string& args);
        uint32_t AddEventHandler(int id, std::function<void(const string&)> callBack);
        void RemoveEventHandler(int id, uint32_t key);
        void ClearEventHandler();
        void ClearEventHandler(int id);

    private:
        uint32_t mNextKey = 0;
        std::queue<uint32_t> mFreeKeys;
        unordered_map<int, unordered_map<uint32_t, std::function<void(const string&)>>> mCallBackMap;
    };
}