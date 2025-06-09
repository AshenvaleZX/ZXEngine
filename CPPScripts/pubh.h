#pragma once
/*  ______   ___  ___   ______   ___   __   ________   ______   ___   __   ______
|  |___   |  \  \/  /  |  ____| |   \ |  | |   _____| |__  __| |   \ |  | |  ____|
|     /  /    \    /   |  |___  | \  \|  | |  | ____    |  |   | \  \|  | |  |___
|    /  /     /    \   |   ___| |  \  \  | |  ||__  |   |  |   |  \  \  | |   ___|
|   /  /__   /  /\  \  |  |___  |  |\  \ | |  |__|  |  _|  |_  |  |\  \ | |  |___
|  |______| |__/  \__| |______| |__| \___| |________| |______| |__| \___| |______|
|
|  C++ game engine project, for learning, practice and experiment.
|  Repository: https://github.com/AshenvaleZX/ZXEngine
|  License: GPL-3.0
*/

#if defined(__ANDROID__)
#   define ZX_PLATFORM_ANDROID
#elif defined(__linux__)
#   define ZX_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#   define ZX_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define ZX_PLATFORM_IOS
#   else
#       define ZX_PLATFORM_MACOS
#   endif
#else
#   error "Unsupported Platform"
#endif

#if defined(ZX_PLATFORM_WINDOWS) || defined(ZX_PLATFORM_MACOS) || defined(ZX_PLATFORM_LINUX)
#   define ZX_PLATFORM_DESKTOP
#elif defined(ZX_PLATFORM_ANDROID) || defined(ZX_PLATFORM_IOS)
#   define ZX_PLATFORM_MOBILE
#endif

#if defined(ZX_PLATFORM_ANDROID)
#   define ZX_API_DEFAULT 1
#elif defined(ZX_PLATFORM_MACOS) || defined(ZX_PLATFORM_IOS)
#   define ZX_API_DEFAULT 3
#else
#   define ZX_API_DEFAULT 0
#endif

/*
|  Graphics API Switch
|  Manually set a ZX_API_SWITCH value instead of ZX_API_DEFAULT to switch the graphics API.
|
|  0: OpenGL
|  1: Vulkan
|  2: D3D12
|  3: Metal
*/
#define ZX_API_SWITCH ZX_API_DEFAULT

#if ZX_API_SWITCH == 0
#define ZX_API_OPENGL
#elif ZX_API_SWITCH == 1
#define ZX_API_VULKAN
#elif ZX_API_SWITCH == 2
#define ZX_API_D3D12
#elif ZX_API_SWITCH == 3
#define ZX_API_METAL
#else
#error "Error Graphics API Definition"
#endif

/*
|  Graphics API Compatibility Check
|
|  Windows : D3D12,  Vulkan, OpenGL
|  macOS   : Metal,  Vulkan, OpenGL
|  Linux   : OpenGL, Vulkan(Unverified)
|  Android : Vulkan
|  iOS     : Metal
*/
#if defined(ZX_PLATFORM_WINDOWS) && defined(ZX_API_METAL)
#error "Unsupported Graphics API"
#elif defined(ZX_PLATFORM_MACOS) && defined(ZX_API_D3D12)
#error "Unsupported Graphics API"
#elif defined(ZX_PLATFORM_LINUX) && (defined(ZX_API_D3D12) || defined(ZX_API_METAL))
#error "Unsupported Graphics API"
#elif defined(ZX_PLATFORM_ANDROID) && (defined(ZX_API_OPENGL) || defined(ZX_API_D3D12) || defined(ZX_API_METAL))
#error "Unsupported Graphics API"
#elif defined(ZX_PLATFORM_IOS) && (defined(ZX_API_OPENGL) || defined(ZX_API_D3D12) || defined(ZX_API_VULKAN))
#error "Unsupported Graphics API"
#endif

/*
|  Editor Mode Switch
|  Comment the definition of ZX_EDITOR to disable the editor mode.
*/
#if defined(ZX_PLATFORM_DESKTOP)
#define ZX_EDITOR
#endif

/*
|  Compute Shader Support Check
|  macOS only supports OpenGL 4.1, but OpenGL 4.3 is required for compute shader.
*/
#if !(defined(ZX_PLATFORM_MACOS) && defined(ZX_API_OPENGL))
#define ZX_COMPUTE_SHADER_SUPPORT
#endif

/*
|  Compute Pipeline Animation Switch
|
|  This is disabled for OpenGL and Vulkan on macOS for following reasons:
|  1. macOS only supports OpenGL 4.1, but OpenGL 4.3 is required for compute shader.
|  2. Although Vulkan on macOS supports compute pipeline, and it actually runs without
|  any code errors or crashes, but the output of the compute shader could be abnormal.
|  The output is not always abnormal, if I run it directly, I will probably get abnormal
|  output, but if I pause and run it frame by frame, or restart it after pausing, the
|  result is more likely to be normal. I feel like there's something wrong with the Vulkan
|  driver on macOS. You can enable ZX_COMPUTE_ANIMATION if you want to try it on your own
|  macOS device, just delete the condition of ZX_PLATFORM_MACOS in the following code.
|  But remember to recompile the SPIR-V shader files after enabling this, beacuse there
|  are some macros in zxshader, and SPIR-V files won't be recompiled automatically.
|  You can lick "Assets/Compile All Shader for Vulkan" in the editor to recompile them.
|
|  This is disabled on OpenGL for the following reasons:
|  1. OpenGL compute shader is not efficient, the animation system's performance will
|  be reduced by using OpenGL compute shader. You can enable it if you want to test it.
*/
#if defined(ZX_COMPUTE_SHADER_SUPPORT) &&                                                  \
    !(defined(ZX_PLATFORM_MACOS) && (defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN))) && \
    !defined(ZX_API_OPENGL)
#define ZX_COMPUTE_ANIMATION
#endif

#include <string>
#include <list>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <atomic>
#include <future>
#include <nlohmann/json.hpp>

#ifdef ZX_PLATFORM_ANDROID
#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#endif

using std::string; 
using std::wstring;
using std::to_string;
using std::to_wstring;
using std::list;
using std::array;
using std::vector;
using std::queue;
using std::deque;
using std::map;
using std::set;
using std::multimap;
using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::make_pair;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using json = nlohmann::json;
namespace filesystem = std::filesystem;

#include "Debug.h"
#include "PublicEnum.h"
#include "Math.h"