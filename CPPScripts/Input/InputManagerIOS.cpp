#include "InputManagerIOS.h"
#include "../EventManager.h"

namespace ZXEngine
{
    void InputManagerIOS::Update()
    {
        // 移除所有已经结束的触摸事件
        mTouches.remove_if([](const Touch& touch)
        {
            return touch.phase == TouchPhase::Ended;
        });

        set<int32_t> curTouchIds;
        for (auto& touch : mTouches)
        {
            curTouchIds.insert(touch.id);

            auto it = mTouchMap.find(touch.id);
            if (it == mTouchMap.end())
            {
                touch.phase = TouchPhase::Ended;
                if (abs(touch.startX - touch.x) < 5.0f && abs(touch.startY - touch.y) < 5.0f)
                {
                    EventManager::GetInstance()->FireEvent(EventType::TOUCH_CLICK, to_string(touch.x) + "|" + to_string(touch.y));
                }
            }
            else
            {
                touch.x = it->second.x;
                touch.y = it->second.y;
                touch.phase = TouchPhase::Moved;
            }
        }

        for (auto& iter : mTouchMap)
        {
            if (curTouchIds.find(iter.first) == curTouchIds.end())
            {
                mTouches.push_back(Touch{
                    iter.first,
                    iter.second.x,
                    iter.second.y,
                    TouchPhase::Began,
                    iter.second.x,
                    iter.second.y
                });
            }
        }
    }

	uint32_t InputManagerIOS::GetTouchCount()
    {
        return static_cast<uint32_t>(mTouches.size());
    }

    Touch InputManagerIOS::GetTouch(uint32_t index)
    {
        return *std::next(mTouches.begin(), index);
    }

    void InputManagerIOS::UpdateTouch(int32_t id, float x, float y)
    {
        mTouchMap[id] = Vector2(x, y);
    }

    void InputManagerIOS::RemoveTouch(int32_t id)
    {
        mTouchMap.erase(id);
    }
}