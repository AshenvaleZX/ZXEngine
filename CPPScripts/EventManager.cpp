#include "EventManager.h"
#include "LuaManager.h"

#ifdef ZX_EDITOR
#include "Editor/EditorDataManager.h"
#endif

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

    void EventManager::FireEvent(uint32_t id, const string& args)
    {
#ifdef ZX_EDITOR
        if (EditorDataManager::GetInstance()->isGameView)
#endif
        {
#ifdef ZX_EDITOR
            if (EditorDataManager::GetInstance()->isGamePause)
                return;
#endif
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
#ifdef ZX_EDITOR
        else
        {
            auto iter = mEditorCallBackMap.find(id);
			if (iter != mEditorCallBackMap.end())
			{
				for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
				{
					iter2->second(args);
				}
			}
        }
#endif
    }

    void EventManager::FireEvent(EventType event, const string& args)
    {
        FireEvent(static_cast<uint32_t>(event), args);
    }

    uint32_t EventManager::AddEventHandler(EventType event, std::function<void(const string&)> callBack)
    {
        uint32_t id = static_cast<uint32_t>(event);

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

    void EventManager::RemoveEventHandler(EventType event, uint32_t key)
    {
        uint32_t id = static_cast<uint32_t>(event);

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

    void EventManager::ClearEventHandler(EventType event)
    {
        uint32_t id = static_cast<uint32_t>(event);

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

    uint32_t EventManager::AddEditorEventHandler(EventType event, std::function<void(const string&)> callBack)
    {
        uint32_t id = static_cast<uint32_t>(event);

        uint32_t key;
        if (mFreeEditorKeys.empty())
        {
            key = mNextEditorKey++;
        }
        else
        {
            key = mFreeEditorKeys.front();
            mFreeEditorKeys.pop();
        }

        mEditorCallBackMap[id][key] = callBack;

        return key;
    }

    void EventManager::RemoveEditorEventHandler(EventType event, uint32_t key)
    {
        uint32_t id = static_cast<uint32_t>(event);

        auto iter = mEditorCallBackMap.find(id);
        if (iter != mEditorCallBackMap.end())
        {
            auto iter2 = iter->second.find(key);
            if (iter2 != iter->second.end())
            {
                iter->second.erase(iter2);
                mFreeEditorKeys.push(key);
            }
        }
    }

    void EventManager::ClearEditorEventHandler()
    {
        mNextEditorKey = 0;
        mEditorCallBackMap.clear();
        mFreeEditorKeys = std::queue<uint32_t>();
    }

    void EventManager::ClearEditorEventHandler(EventType event)
    {
        uint32_t id = static_cast<uint32_t>(event);

        auto iter = mEditorCallBackMap.find(id);
        if (iter != mEditorCallBackMap.end())
        {
            for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
            {
                mFreeEditorKeys.push(iter2->first);
            }
            mEditorCallBackMap.erase(iter);
        }
    }
}