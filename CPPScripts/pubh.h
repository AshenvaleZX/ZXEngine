#pragma once
#define ZX_EDITOR

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

using std::string; 
using std::cout;
using std::list;
using std::map;
using std::unordered_map;
using std::pair;
using std::make_pair;
using std::vector;
using std::endl;
using std::ifstream;
using std::stringstream;
using std::to_string;
using json = nlohmann::json;
namespace filesystem = std::filesystem;

#include "Debug.h"
#include "Utils.h"
#include "PublicEnum.h"
#include "Math.h"