#include "WindowManagerAndroid.h"
#include "../GlobalData.h"

namespace ZXEngine
{
    static bool isAndroidWindowInitialized = false;

    /*!
    * Handles commands sent to this Android application
    * @param pApp the app the commands are coming from
    * @param cmd the command to handle
    */
    void handle_cmd(android_app* pApp, int32_t cmd)
    {
        switch (cmd)
        {
            case APP_CMD_INIT_WINDOW:
                // 安卓窗口初始化完成，必须等到这个CMD才能获取到窗口继续后面的步骤
                isAndroidWindowInitialized = true;
                break;
            case APP_CMD_TERM_WINDOW:
                // The window is being destroyed. Use this to clean up your userData to avoid leaking
                // resources.
                break;
            default:
                break;
        }
    }

    /*!
    * Enable the motion events you want to handle; not handled events are
    * passed back to OS for further processing. For this example case,
    * only pointer and joystick devices are enabled.
    *
    * @param motionEvent the newly arrived GameActivityMotionEvent.
    * @return true if the event is from a pointer or joystick device,
    *         false for all other input devices.
    */
    bool motion_event_filter_func(const GameActivityMotionEvent* motionEvent)
    {
        auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
        return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
                sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
    }

    void* WindowManagerAndroid::GetWindow()
    {
        return static_cast<void*>(mApp->window);
    }

    void WindowManagerAndroid::CloseWindow(string args)
    {
        // Android doesn't need this.
    }

    bool WindowManagerAndroid::WindowShouldClose()
    {
        CheckEvents();
        return mApp->destroyRequested;
    }

    void WindowManagerAndroid::Init()
    {
        mApp = GlobalData::app;

        // Set the callback to process system events
        mApp->onAppCmd = handle_cmd;
        
        // Set input event filters (set it to NULL if the app wants to process all inputs).
        // Note that for key inputs, this example uses the default default_key_filter()
        // implemented in android_native_app_glue.c.
        android_app_set_motion_event_filter(mApp, motion_event_filter_func);

        while (!isAndroidWindowInitialized) 
        {
            // Wait for the window to be initialized
            CheckEvents();
        }
        
		GlobalData::srcWidth = static_cast<uint32_t>(ANativeWindow_getWidth(GlobalData::app->window));
		GlobalData::srcHeight = static_cast<uint32_t>(ANativeWindow_getHeight(GlobalData::app->window));
    }

    void WindowManagerAndroid::CheckEvents()
    {
        // Process all pending events before running game logic.
        bool done = false;
        while (!done)
        {
            int events;
            android_poll_source* pSource;
            int result = ALooper_pollOnce(0, nullptr, &events, reinterpret_cast<void**>(&pSource));

            switch (result)
            {
                case ALOOPER_POLL_TIMEOUT:
                    [[clang::fallthrough]];
                case ALOOPER_POLL_WAKE:
                    // No events occurred before the timeout or explicit wake. Stop checking for events.
                    done = true;
                    break;
                case ALOOPER_EVENT_ERROR:
                    Debug::LogError("ALooper_pollOnce returned an error");
                    break;
                case ALOOPER_POLL_CALLBACK:
                    break;
                default:
                    if (pSource)
                    {
                        pSource->process(mApp, pSource);
                    }
            }
        }
    }
}