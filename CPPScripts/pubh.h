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
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

// 防止windows.h里的宏定义max和min影响到其它库里的相同字段
#define NOMINMAX
#include <windows.h>

using std::string; 
using std::cout;
using std::list;
using std::array;
using std::map;
using std::set;
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
using json = nlohmann::json;
namespace filesystem = std::filesystem;

#include "Debug.h"
#include "Utils.h"
#include "PublicEnum.h"
#include "Math.h"