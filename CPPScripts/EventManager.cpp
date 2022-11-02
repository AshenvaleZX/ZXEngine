#include "EventManager.h"

namespace ZXEngine
{
    EventManager* EventManager::mInstance = nullptr;

    void EventManager::Create()
    {
        mInstance = new EventManager();
    }

    EventManager* EventManager::GetInstance()
    {
        return mInstance;
    }

    void EventManager::AddEventHandler(int id, std::function<void(string)> callBack)
    {
        callBackMap.insert(pair<int, std::function<void(string)>>(id, callBack));
    }

    void EventManager::RemoveEventHandler()
    {
    }

    void EventManager::ClearEventHandler()
    {
    }

    void EventManager::FireEvent(int id, string args)
    {
        map<int, std::function<void(string)>>::iterator iter = callBackMap.find(id);
        if (iter != callBackMap.end()) 
        {
            iter->second(args);
        }
    }
}