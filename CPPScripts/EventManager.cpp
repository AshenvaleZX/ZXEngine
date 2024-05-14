#include "EventManager.h"
#include "LuaManager.h"

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

    void EventManager::FireEvent(int id, const string& args)
    {
        auto iter = mCallBackMap.find(id);
        if (iter != mCallBackMap.end())
        {
            for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
            {
				iter2->second(args);
			}
        }
        LuaManager::GetInstance()->CallFunction("EngineEvent", "FireEvent", (to_string(id) + "," + args).c_str());
    }

    uint32_t EventManager::AddEventHandler(int id, std::function<void(const string&)> callBack)
    {
        uint32_t key;
        if (mFreeKeys.empty())
        {
			key = mNextKey++;
		}
        else
        {
			key = mFreeKeys.front();
			mFreeKeys.pop();
		}

        mCallBackMap[id][key] = callBack;

        return key;
    }

    void EventManager::RemoveEventHandler(int id, uint32_t key)
    {
        auto iter = mCallBackMap.find(id);
        if (iter != mCallBackMap.end())
        {
			auto iter2 = iter->second.find(key);
            if (iter2 != iter->second.end())
            {
				iter->second.erase(iter2);
				mFreeKeys.push(key);
			}
		}
    }

    void EventManager::ClearEventHandler()
    {
        mNextKey = 0;
		mCallBackMap.clear();
		mFreeKeys = std::queue<uint32_t>();
    }

    void EventManager::ClearEventHandler(int id)
    {
        auto iter = mCallBackMap.find(id);
        if (iter != mCallBackMap.end())
        {
            for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
            {
				mFreeKeys.push(iter2->first);
			}
            mCallBackMap.erase(iter);
        }
    }
}