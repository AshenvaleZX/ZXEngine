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
        void FireEvent(uint32_t event, const string& args);
        void FireEvent(EventType event, const string& args);
        uint32_t AddEventHandler(EventType event, std::function<void(const string&)> callBack);
        void RemoveEventHandler(EventType event, uint32_t key);
        void ClearEventHandler();
        void ClearEventHandler(EventType event);

    private:
        uint32_t mNextKey = 0;
        std::queue<uint32_t> mFreeKeys;
        unordered_map<uint32_t, unordered_map<uint32_t, std::function<void(const string&)>>> mCallBackMap;

        // -------------------------------- For Editor --------------------------------
    public:
        uint32_t AddEditorEventHandler(EventType event, std::function<void(const string&)> callBack);
        void RemoveEditorEventHandler(EventType event, uint32_t key);
        void ClearEditorEventHandler();
        void ClearEditorEventHandler(EventType event);

    private:
        uint32_t mNextEditorKey = 0;
        std::queue<uint32_t> mFreeEditorKeys;
        unordered_map<uint32_t, unordered_map<uint32_t, std::function<void(const string&)>>> mEditorCallBackMap;
    };
}