#include "../pubh.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#ifdef ZX_PLATFORM_MACOS
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#else
#include <QuartzCore/QuartzCore.hpp>
#endif