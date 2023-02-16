#pragma once
#define ZX_EDITOR
#define ZX_API_OPENGL
// #define ZX_API_VULKAN

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#ifdef ZX_EDITOR
// ��ֹwindows.h��ĺ궨��max��minӰ�쵽�����������ͬ�ֶ�
#define NOMINMAX
#include <windows.h>
#endif

using std::string; 
using std::cout;
using std::list;
using std::map;
using std::set;
using std::unordered_map;
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