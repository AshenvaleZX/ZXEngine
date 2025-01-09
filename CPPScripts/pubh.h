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

#if defined(__linux__)
#define ZX_PLATFORM_LINUX
#elif defined(__APPLE__)
#define ZX_PLATFORM_MACOS
#elif defined(_WIN32) || defined(_WIN64)
#define ZX_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
#define ZX_PLATFORM_ANDROID
#endif

#if defined(ZX_PLATFORM_WINDOWS) || defined(ZX_PLATFORM_MACOS) || defined(ZX_PLATFORM_LINUX)
#define ZX_PLATFORM_DESKTOP
#elif defined(ZX_PLATFORM_ANDROID)
#define ZX_PLATFORM_MOBILE
#endif

#if defined(ZX_PLATFORM_DESKTOP)
#define ZX_API_DEFAULT 0
#elif defined(ZX_PLATFORM_ANDROID)
#define ZX_API_DEFAULT 1
#endif

/*
| Graphics API Switch
| Manually set a ZX_API_SWITCH value instead of ZX_API_DEFAULT to switch the graphics API.
| 0: OpenGL
| 1: Vulkan
| 2: D3D12
*/
#define ZX_API_SWITCH ZX_API_DEFAULT

#if ZX_API_SWITCH == 0
#define ZX_API_OPENGL
#elif ZX_API_SWITCH == 1
#define ZX_API_VULKAN
#elif ZX_API_SWITCH == 2
#define ZX_API_D3D12
#else
#error "No Graphics API"
#endif

/*
| Editor Mode Switch
| Comment the definition of ZX_EDITOR to disable the editor mode.
*/
#if defined(ZX_PLATFORM_DESKTOP)
#define ZX_EDITOR
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