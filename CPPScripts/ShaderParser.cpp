#include "ShaderParser.h"
#include "Resources.h"

namespace ZXEngine
{
	map<string, BlendFactor> blendFactorMap =
	{
		{ "Zero",			BlendFactor::ZERO			}, { "One",					  BlendFactor::ONE						},
		{ "SrcColor",		BlendFactor::SRC_COLOR		}, { "OneMinusSrcColor",	  BlendFactor::ONE_MINUS_SRC_COLOR		},
		{ "DstColor",		BlendFactor::DST_COLOR		}, { "OneMinusDstColor",	  BlendFactor::ONE_MINUS_DST_COLOR		},
		{ "SrcAlpha",		BlendFactor::SRC_ALPHA		}, { "OneMinusSrcAlpha",	  BlendFactor::ONE_MINUS_SRC_ALPHA		},
		{ "DstAlpha",		BlendFactor::DST_ALPHA		}, { "OneMinusDstAlpha",	  BlendFactor::ONE_MINUS_DST_ALPHA		},
		{ "ConstantColor",	BlendFactor::CONSTANT_COLOR }, { "OneMinusConstantColor", BlendFactor::ONE_MINUS_CONSTANT_COLOR },
		{ "ConstantAlpha",	BlendFactor::CONSTANT_ALPHA }, { "OneMinusConstantAlpha", BlendFactor::ONE_MINUS_CONSTANT_ALPHA },
	};

	map<string, BlendOption> blendOptionMap =
	{
		{ "Add", BlendOption::ADD }, { "Sub", BlendOption::SUBTRACT }, { "RevSub", BlendOption::REVERSE_SUBTRACT },
		{ "Min", BlendOption::MIN }, { "Max", BlendOption::MAX		},
	};

	map<string, FaceCullOption> faceCullOptionMap =
	{
		{ "Back", FaceCullOption::Back }, { "Front", FaceCullOption::Front		  },
		{ "Off",  FaceCullOption::None }, { "All",   FaceCullOption::FrontAndBack },
	};

	map<string, CompareOption> depthTestOptionMap =
	{
		{ "Never",  CompareOption::NEVER  }, { "Less",	   CompareOption::LESS		}, { "LessOrEqual",	   CompareOption::LESS_OR_EQUAL	   },
		{ "Always", CompareOption::ALWAYS }, { "Greater",  CompareOption::GREATER	}, { "GreaterOrEqual", CompareOption::GREATER_OR_EQUAL },
		{ "Equal",	CompareOption::EQUAL  }, { "NotEqual", CompareOption::NOT_EQUAL },
	};

	map<string, ShaderPropertyType> shaderPropertyMap =
	{
		{ "vec2", ShaderPropertyType::VEC2 }, { "vec3",  ShaderPropertyType::VEC3  }, { "vec4", ShaderPropertyType::VEC4 }, 
		{ "mat2", ShaderPropertyType::MAT2 }, { "mat3",  ShaderPropertyType::MAT3  }, { "mat4", ShaderPropertyType::MAT4 },
		{ "int",  ShaderPropertyType::INT  }, { "float", ShaderPropertyType::FLOAT }, { "bool", ShaderPropertyType::BOOL },

		{ "sampler", ShaderPropertyType::SAMPLER }, { "sampler2D", ShaderPropertyType::SAMPLER_2D }, { "samplerCube", ShaderPropertyType::SAMPLER_CUBE },

		{ "ENGINE_Model",       ShaderPropertyType::ENGINE_MODEL       }, { "ENGINE_View",            ShaderPropertyType::ENGINE_VIEW            },
		{ "ENGINE_Projection",  ShaderPropertyType::ENGINE_PROJECTION  }, { "ENGINE_Camera_Pos",      ShaderPropertyType::ENGINE_CAMERA_POS      },
		{ "ENGINE_Light_Pos",   ShaderPropertyType::ENGINE_LIGHT_POS   }, { "ENGINE_Light_Dir",       ShaderPropertyType::ENGINE_LIGHT_DIR       },
		{ "ENGINE_Light_Color", ShaderPropertyType::ENGINE_LIGHT_COLOR }, { "ENGINE_Light_Intensity", ShaderPropertyType::ENGINE_LIGHT_INTENSITY },
		{ "ENGINE_Depth_Map",   ShaderPropertyType::ENGINE_DEPTH_MAP   }, { "ENGINE_Depth_Cube_Map",  ShaderPropertyType::ENGINE_DEPTH_CUBE_MAP  },
		{ "ENGINE_Far_Plane",   ShaderPropertyType::ENGINE_FAR_PLANE   },
	};

	map<string, string> enginePropertiesTypeMap =
	{
		{ "ENGINE_Model",          "mat4"        }, { "ENGINE_View",            "mat4"  }, { "ENGINE_Projection", "mat4"      }, 
		{ "ENGINE_Camera_Pos",     "vec3"        }, { "ENGINE_Light_Pos",       "vec3"  }, { "ENGINE_Light_Dir",  "vec3"      }, 
		{ "ENGINE_Light_Color",    "vec3"        }, { "ENGINE_Light_Intensity", "float" }, { "ENGINE_Depth_Map",  "sampler2D" }, 
		{ "ENGINE_Depth_Cube_Map", "samplerCube" }, { "ENGINE_Far_Plane",       "float" },
	};

	map<ShaderPropertyType, string> propertyTypeToGLSLType =
	{
		{ ShaderPropertyType::BOOL, "bool" }, { ShaderPropertyType::INT,  "int"  }, { ShaderPropertyType::FLOAT, "float" },
		{ ShaderPropertyType::VEC2, "vec2" }, { ShaderPropertyType::VEC3, "vec3" }, { ShaderPropertyType::VEC4,  "vec4"  },
		{ ShaderPropertyType::MAT2, "mat2" }, { ShaderPropertyType::MAT3, "mat3" }, { ShaderPropertyType::MAT4,  "mat4"  },

		{ ShaderPropertyType::SAMPLER, "sampler" }, { ShaderPropertyType::SAMPLER_2D, "sampler2D" }, { ShaderPropertyType::SAMPLER_CUBE, "samplerCube" },

		{ ShaderPropertyType::ENGINE_MODEL,       "mat4"      }, { ShaderPropertyType::ENGINE_VIEW,            "mat4"        }, 
		{ ShaderPropertyType::ENGINE_PROJECTION,  "mat4"      }, { ShaderPropertyType::ENGINE_CAMERA_POS,      "vec3"        }, 
		{ ShaderPropertyType::ENGINE_LIGHT_POS,   "vec3"      }, { ShaderPropertyType::ENGINE_LIGHT_DIR,       "vec3"        },
		{ ShaderPropertyType::ENGINE_LIGHT_COLOR, "vec3"      }, { ShaderPropertyType::ENGINE_LIGHT_INTENSITY, "float"       },
		{ ShaderPropertyType::ENGINE_DEPTH_MAP,   "sampler2D" }, { ShaderPropertyType::ENGINE_DEPTH_CUBE_MAP,  "samplerCube" },
		{ ShaderPropertyType::ENGINE_FAR_PLANE,   "float"     },
	};

	ShaderInfo ShaderParser::GetShaderInfo(const string& code)
	{
		ShaderInfo info;
		info.stateSet = GetShaderStateSet(code);

		// 这里数字类型用的是string库里的专用类型，因为string库的find，substr等操作返回的这些数据类型和具体编译环境有关
		// 特别是find，网上很多地方说没找到就会返回-1，其实这个说法不准确，因为find的函数定义返回的类型是size_t
		// 而size_t是一个无符号整数(具体多少位取决于编译环境)，一个无符号整数变成-1，是因为溢出了，实际上没找到的时候真正返回的是npos
		// 其实直接用int来处理也行，会自动隐式转换，也可以用-1来判断是否找到，但是这样会有编译的Warning
		// 为了在各种编译环境下不出错，这里直接采用原定义中的string::size_type和string::npos是最保险的，并且不会有Warning
		string::size_type hasDirLight = code.find("ENGINE_Light_Dir");
		string::size_type hasPointLight = code.find("ENGINE_Light_Pos");
		if (hasDirLight != string::npos)
			info.lightType = LightType::Directional;
		else if (hasPointLight != string::npos)
			info.lightType = LightType::Point;
		else
			info.lightType = LightType::None;

		string::size_type hasDirShadow = code.find("ENGINE_Depth_Map");
		string::size_type hasPointShadow = code.find("ENGINE_Depth_Cube_Map");
		if (hasDirShadow != string::npos)
			info.shadowType = ShadowType::Directional;
		else if (hasPointShadow != string::npos)
			info.shadowType = ShadowType::Point;
		else
			info.shadowType = ShadowType::None;

		string vertCode, geomCode, fragCode;
		ParseShaderCode(code, vertCode, geomCode, fragCode);

		info.stages = ZX_SHADER_STAGE_VERTEX_BIT | ZX_SHADER_STAGE_FRAGMENT_BIT;
		if (!geomCode.empty())
			info.stages |= ZX_SHADER_STAGE_GEOMETRY_BIT;

		info.vertProperties = GetProperties(vertCode);
		info.fragProperties = GetProperties(fragCode);
		if (!geomCode.empty())
			info.geomProperties = GetProperties(geomCode);

		SetupBindings(info);

		return info;
	}

	bool ShaderParser::IsBasePropertyType(ShaderPropertyType type)
	{
		return !(type == ShaderPropertyType::SAMPLER || type == ShaderPropertyType::SAMPLER_2D || type == ShaderPropertyType::SAMPLER_CUBE
			|| type == ShaderPropertyType::ENGINE_DEPTH_MAP || type == ShaderPropertyType::ENGINE_DEPTH_CUBE_MAP);
	}

	ShaderPropertiesInfo ShaderParser::GetProperties(const string& stageCode)
	{
		ShaderPropertiesInfo propertiesInfo;

		string propertiesBlock = GetCodeBlock(stageCode, "Properties");
		if (propertiesBlock.empty())
			return propertiesInfo;

		auto lines = Utils::StringSplit(propertiesBlock, '\n');

		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);

			if (words.size() == 0)
				continue;
			else if (words[0] == "//")
				continue;
			else if (words[0] == "using")
			{
				auto type = shaderPropertyMap[words[1]];
				ShaderProperty property = {};
				property.name = words[1];
				property.type = type;
				if (IsBasePropertyType(type))
					propertiesInfo.baseProperties.push_back(property);
				else
					propertiesInfo.textureProperties.push_back(property);
			}
			else
			{
				auto type = shaderPropertyMap[words[0]];
				ShaderProperty property = {};
				property.name = words[1];
				property.type = type;
				if (IsBasePropertyType(type))
					propertiesInfo.baseProperties.push_back(property);
				else
					propertiesInfo.textureProperties.push_back(property);
			}
		}

		return propertiesInfo;
	}

	void ShaderParser::ParseShaderCode(const string& code, string& vertCode, string& geomCode, string& fragCode)
	{
		vertCode = GetCodeBlock(code, "Vertex");
		geomCode = GetCodeBlock(code, "Geometry");
		fragCode = GetCodeBlock(code, "Fragment");
	}

	string ShaderParser::TranslateToOpenGL(const string& originCode)
	{
		if (originCode.empty())
			return "";

		string glCode = "#version 460 core\n\n";

		string gsInOutBlock = GetCodeBlock(originCode, "GSInOut");
		if (!gsInOutBlock.empty())
		{
			auto lines = Utils::StringSplit(gsInOutBlock, '\n');
			for (auto& line : lines)
				glCode += line + ";\n";
			glCode += "\n";
		}

		string inputBlock = GetCodeBlock(originCode, "Input");
		auto lines = Utils::StringSplit(inputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 3 && words[0] != "//")
				glCode += "layout (location = " + words[0] + ") in " + words[1] + " " + words[2] + ";\n";
		}
		glCode += "\n";

		string outputBlock = GetCodeBlock(originCode, "Output");
		lines = Utils::StringSplit(outputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 3 && words[0] != "//")
				glCode += "layout (location = " + words[0] + ") out " + words[1] + " " + words[2] + ";\n";
		}
		glCode += "\n";

		string propertiesBlock = GetCodeBlock(originCode, "Properties");
		lines = Utils::StringSplit(propertiesBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() == 0)
				continue;
			else if (words[0] == "//")
				continue;
			else if (words[0] == "using")
				glCode += "uniform " + enginePropertiesTypeMap[words[1]] + " " + words[1] + ";\n";
			else
				glCode += "uniform " + words[0] + " " + words[1] + ";\n";
		}
		glCode += "\n";

		glCode += GetCodeBlock(originCode, "Program");

		return glCode;
	}

	string ShaderParser::TranslateToVulkan(const string& originCode, const ShaderPropertiesInfo& info)
	{
		if (originCode.empty())
			return "";

		string vkCode = "#version 460 core\n\n";

		string inputBlock = GetCodeBlock(originCode, "Input");
		auto lines = Utils::StringSplit(inputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 3 && words[0] != "//")
				vkCode += "layout (location = " + words[0] + ") in " + words[1] + " " + words[2] + ";\n";
		}
		vkCode += "\n";

		string outputBlock = GetCodeBlock(originCode, "Output");
		lines = Utils::StringSplit(outputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 3 && words[0] != "//")
				vkCode += "layout (location = " + words[0] + ") out " + words[1] + " " + words[2] + ";\n";
		}
		vkCode += "\n";

		if (!info.baseProperties.empty())
		{
			vkCode += "layout (binding = " + to_string(info.baseProperties[0].binding) + ") uniform UniformBufferObject {\n";
			for (auto& property : info.baseProperties)
				vkCode += "    " + propertyTypeToGLSLType[property.type] + " " + property.name + ";\n";
			vkCode += "} _UBO;\n";
		}
		vkCode += "\n";

		for (auto& property : info.textureProperties)
		{
			vkCode += "layout (binding = " + to_string(property.binding) + ") uniform " + propertyTypeToGLSLType[property.type] + " " + property.name + ";\n";
		}
		vkCode += "\n";

		string programBlock = GetCodeBlock(originCode, "Program");
		if (!info.baseProperties.empty())
		{
			for (auto& property : info.baseProperties)
				Utils::ReplaceAllString(programBlock, property.name, "_UBO." + property.name);
		}
		vkCode += programBlock;

		return vkCode;
	}

	ShaderStateSet ShaderParser::GetShaderStateSet(const string& code)
	{
		ShaderStateSet stateSet;

		string settingBlock = GetCodeBlock(code, "Setting");
		if (settingBlock.empty())
			return stateSet;

		auto lines = Utils::StringSplit(settingBlock, '\n');

		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);

			if (words.size() == 0)
			{
				continue;
			}
			else if (words[0] == "Blend")
			{
				stateSet.srcFactor = blendFactorMap[words[1]];
				stateSet.dstFactor = blendFactorMap[words[2]];
			}
			else if (words[0] == "BlendOp")
			{
				stateSet.blendOp = blendOptionMap[words[1]];
			}
			else if (words[0] == "Cull")
			{
				stateSet.cull = faceCullOptionMap[words[1]];
			}
			else if (words[0] == "ZTest")
			{
				stateSet.depthCompareOp = depthTestOptionMap[words[1]];
			}
			else if (words[0] == "ZWrite")
			{
				stateSet.depthWrite = words[1] == "true";
			}
		}

		return stateSet;
	}

	string ShaderParser::GetCodeBlock(const string& code, const string& blockName)
	{
		auto begin = code.find(blockName);
		if (begin == string::npos)
			return "";

		int level = 0;
		size_t s = 0, e = 0;

		for (size_t i = begin; i < code.size(); i++)
		{
			if (code[i] == '{')
			{
				level++;
				if (level == 1)
				{
					s = i;
				}
			}
			else if (code[i] == '}')
			{
				level--;
				if (level == 0)
				{
					e = i;
					break;
				}
			}
		}

		return code.substr(s + 1, e - s - 1);
	}

	void ShaderParser::SetupBindings(ShaderInfo& info)
	{
		uint32_t binding = 0;

		if (!info.vertProperties.baseProperties.empty())
		{
			for (auto& property : info.vertProperties.baseProperties)
				property.binding = binding;
			binding++;
		}
		for (auto& property : info.vertProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}

		if (!info.geomProperties.baseProperties.empty())
		{
			for (auto& property : info.geomProperties.baseProperties)
				property.binding = binding;
			binding++;
		}
		for (auto& property : info.geomProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}

		if (!info.fragProperties.baseProperties.empty())
		{
			for (auto& property : info.fragProperties.baseProperties)
				property.binding = binding;
			binding++;
		}
		for (auto& property : info.fragProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}
	}
}