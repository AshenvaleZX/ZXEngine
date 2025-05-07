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
}
