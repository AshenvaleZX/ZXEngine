#include "InputManagerAndroid.h"
#include "../GlobalData.h"
#include "../EventManager.h"

namespace ZXEngine
{
    void InputManagerAndroid::Update()
    {
        // 移除所有已经结束的触摸事件
        mTouches.remove_if([](const Touch& touch)
        {
            return touch.phase == TouchPhase::Ended;
        });

        // handle all queued inputs
        auto inputBuffer = android_app_swap_input_buffers(GlobalData::app);
        if (!inputBuffer)
        {
            // no inputs yet.
            return;
        }

        // 当前帧的Down操作，不要被Move操作覆盖了
        vector<int32_t> downTouches;

        // handle motion events (motionEventsCounts can be 0).
        for (auto i = 0; i < inputBuffer->motionEventsCount; i++)
        {
            auto &motionEvent = inputBuffer->motionEvents[i];
            auto action = motionEvent.action;

            // Find the pointer index, mask and bitshift to turn it into a readable value.
            auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

            // get the x and y position of this event if it is not ACTION_MOVE.
            auto &pointer = motionEvent.pointers[pointerIndex];
            auto x = GameActivityPointerAxes_getX(&pointer);
            auto y = GameActivityPointerAxes_getY(&pointer);

            // determine the action type and process the event accordingly.
            switch (action & AMOTION_EVENT_ACTION_MASK)
            {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                {
                    mTouches.push_back(Touch{pointer.id, x, y, TouchPhase::Began, x, y});
                    break;
                }
                case AMOTION_EVENT_ACTION_CANCEL:
                    // treat the CANCEL as an UP event: doing nothing in the app, except
                    // removing the pointer from the cache if pointers are locally saved.
                    // code pass through on purpose.
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                {
                    auto it = std::find_if(mTouches.begin(), mTouches.end(), [pointer](const Touch& touch)
                    {
                        return touch.id == pointer.id;
                    });

                    if (it != mTouches.end())
                    {
                        it->phase = TouchPhase::Ended;
                        if (abs(it->startX - x) < 5.0f && abs(it->startY - y) < 5.0f)
                        {
                            EventManager::GetInstance()->FireEvent(EventType::TOUCH_CLICK, to_string(x) + "|" + to_string(y));
                        }
                    }
                    break;
                }
                case AMOTION_EVENT_ACTION_MOVE:
                {
                    // There is no pointer index for ACTION_MOVE, only a snapshot of
                    // all active pointers; app needs to cache previous active pointers
                    // to figure out which ones are actually moved.
                    for (auto index = 0; index < motionEvent.pointerCount; index++)
                    {
                        pointer = motionEvent.pointers[index];

                        if (std::find(downTouches.begin(), downTouches.end(), pointer.id) == downTouches.end())
                        {
                            x = GameActivityPointerAxes_getX(&pointer);
                            y = GameActivityPointerAxes_getY(&pointer);

                            auto it = std::find_if(mTouches.begin(), mTouches.end(), [pointer](const Touch& touch)
                            {
                                return touch.id == pointer.id;
                            });

                            if (it != mTouches.end())
                            {
                                it->x = x;
                                it->y = y;
                                it->phase = TouchPhase::Moved;
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // clear the motion input count in this buffer for main thread to re-use.
        android_app_clear_motion_events(inputBuffer);

        // handle input key events.
        for (auto i = 0; i < inputBuffer->keyEventsCount; i++)
        {
            auto &keyEvent = inputBuffer->keyEvents[i];
            string log = "Key: " + to_string(keyEvent.keyCode) + " ";
            switch (keyEvent.action)
            {
                case AKEY_EVENT_ACTION_DOWN:
                    log += "Key Down";
                    break;
                case AKEY_EVENT_ACTION_UP:
                    log += "Key Up";
                    break;
                case AKEY_EVENT_ACTION_MULTIPLE:
                    // Deprecated since Android API level 29.
                    log += "Multiple Key Actions";
                    break;
                default:
                    log += "Unknown KeyEvent Action: " + to_string(keyEvent.action);
            }
            Debug::Log(log);
        }

        // clear the key input count too.
        android_app_clear_key_events(inputBuffer);
    }

	uint32_t InputManagerAndroid::GetTouchCount()
    {
        return static_cast<uint32_t>(mTouches.size());
    }
	
    Touch InputManagerAndroid::GetTouch(uint32_t index)
    {
        return *std::next(mTouches.begin(), index);
    }
}