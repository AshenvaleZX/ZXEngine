#include "InputManagerIOS.h"

namespace ZXEngine
{
	uint32_t InputManagerIOS::GetTouchCount()
    {
        return static_cast<uint32_t>(mTouches.size());
    }
	
    Touch InputManagerIOS::GetTouch(uint32_t index)
    {
        return *std::next(mTouches.begin(), index);
    }
}