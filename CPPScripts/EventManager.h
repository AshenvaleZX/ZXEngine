#pragma once
#include "pubh.h"

namespace ZXEngine
{
    class EventManager
    {
    public:
        map<int, std::function<void(string)>> callBackMap;

        static void Create();
        static EventManager* GetInstance();

        void AddEventHandler(int id, std::function<void(string)> callBack);
        void RemoveEventHandler();
        void ClearEventHandler();
        void FireEvent(int id, string args);

    private:
        static EventManager* mInstance;
    };
}