#pragma once
#include "pubh.h"

namespace ZXEngine
{
    enum EventType
    {
        GAME_START = 0,
        UPDATE_MOUSE_POS,
    };

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