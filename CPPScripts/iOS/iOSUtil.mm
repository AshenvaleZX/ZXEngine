#include "iOSUtil.h"
#include <UIKit/UIKit.h>

namespace ZXEngine
{
    string iOSUtil::GetResourcePath(const string& path, const string& ext)
    {
        @autoreleasepool
        {
            NSString* res = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:path.c_str()]
                                                            ofType:[NSString stringWithUTF8String:ext.c_str()]];
            if (res)
            {
                return std::string([res UTF8String]);
            }
            return "";
        }
    }

    Vector2 iOSUtil::GetScreenSize()
    {
        @autoreleasepool
        {
            CGRect screenRect = [[UIScreen mainScreen] bounds];
            return Vector2(static_cast<float>(screenRect.size.width), static_cast<float>(screenRect.size.height));
        }
    }
}
