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

#pragma once
#define ZX_EDITOR
#define ZX_API_OPENGL
// #define ZX_API_VULKAN
// #define ZX_API_D3D12

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>

using std::string; 
using std::wstring;
using std::cout;
using std::list;
using std::array;
using std::map;
using std::set;
using std::deque;
using std::multimap;
using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::make_pair;
using std::vector;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::to_string;
using std::to_wstring;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using json = nlohmann::json;
namespace filesystem = std::filesystem;

#include "Debug.h"
#include "Utils.h"
#include "PublicEnum.h"
#include "Math.h"