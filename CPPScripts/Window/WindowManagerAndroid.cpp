#include "WindowManagerAndroid.h"

namespace ZXEngine
{
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
        return mApp->destroyRequested;
    }
}