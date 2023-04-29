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
		{ "ENGINE_Far_Plane",   ShaderPropertyType::ENGINE_FAR_PLANE   }, { "ENGINE_Model_Inv",       ShaderPropertyType::ENGINE_MODEL_INV       },
	};

	map<string, string> enginePropertiesTypeMap =
	{
		{ "ENGINE_Model",          "mat4"        }, { "ENGINE_View",            "mat4"  }, { "ENGINE_Projection", "mat4"      }, 
		{ "ENGINE_Camera_Pos",     "vec3"        }, { "ENGINE_Light_Pos",       "vec3"  }, { "ENGINE_Light_Dir",  "vec3"      }, 
		{ "ENGINE_Light_Color",    "vec3"        }, { "ENGINE_Light_Intensity", "float" }, { "ENGINE_Depth_Map",  "sampler2D" }, 
		{ "ENGINE_Depth_Cube_Map", "samplerCube" }, { "ENGINE_Far_Plane",       "float" }, { "ENGINE_Model_Inv",  "mat4"      },
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
		{ ShaderPropertyType::ENGINE_FAR_PLANE,   "float"     }, { ShaderPropertyType::ENGINE_MODEL_INV,       "mat4"        },
	};

	map<ShaderPropertyType, string> propertyTypeToHLSLType =
	{
		{ ShaderPropertyType::BOOL, "bool"     }, { ShaderPropertyType::INT,  "int"      }, { ShaderPropertyType::FLOAT, "float"    },
		{ ShaderPropertyType::VEC2, "float2"   }, { ShaderPropertyType::VEC3, "float3"   }, { ShaderPropertyType::VEC4,  "float4"   },
		{ ShaderPropertyType::MAT2, "float2x2" }, { ShaderPropertyType::MAT3, "float3x3" }, { ShaderPropertyType::MAT4,  "float4x4" },

		{ ShaderPropertyType::SAMPLER, "Texture2D" }, { ShaderPropertyType::SAMPLER_2D, "Texture2D" }, { ShaderPropertyType::SAMPLER_CUBE, "TextureCube" },

		{ ShaderPropertyType::ENGINE_MODEL,       "float4x4"  }, { ShaderPropertyType::ENGINE_VIEW,            "float4x4"    },
		{ ShaderPropertyType::ENGINE_PROJECTION,  "float4x4"  }, { ShaderPropertyType::ENGINE_CAMERA_POS,      "float3"      },
		{ ShaderPropertyType::ENGINE_LIGHT_POS,   "float3"    }, { ShaderPropertyType::ENGINE_LIGHT_DIR,       "float3"      },
		{ ShaderPropertyType::ENGINE_LIGHT_COLOR, "float3"    }, { ShaderPropertyType::ENGINE_LIGHT_INTENSITY, "float"       },
		{ ShaderPropertyType::ENGINE_DEPTH_MAP,   "Texture2D" }, { ShaderPropertyType::ENGINE_DEPTH_CUBE_MAP,  "TextureCube" },
		{ ShaderPropertyType::ENGINE_FAR_PLANE,   "float"     }, { ShaderPropertyType::ENGINE_MODEL_INV,       "float4x4"    },
	};

	ShaderInfo ShaderParser::GetShaderInfo(const string& code)
	{
		ShaderInfo info;
		info.stateSet = GetShaderStateSet(code);

		// �������������õ���string�����ר�����ͣ���Ϊstring���find��substr�Ȳ������ص���Щ�������ͺ;�����뻷���й�
		// �ر���find�����Ϻܶ�ط�˵û�ҵ��ͻ᷵��-1����ʵ���˵����׼ȷ����Ϊfind�ĺ������巵�ص�������size_t
		// ��size_t��һ���޷�������(�������λȡ���ڱ��뻷��)��һ���޷����������-1������Ϊ����ˣ�ʵ����û�ҵ���ʱ���������ص���npos
		// ��ʵֱ����int������Ҳ�У����Զ���ʽת����Ҳ������-1���ж��Ƿ��ҵ��������������б����Warning
		// Ϊ���ڸ��ֱ��뻷���²���������ֱ�Ӳ���ԭ�����е�string::size_type��string::npos����յģ����Ҳ�����Warning
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

#ifdef ZX_API_D3D12
		SetUpPropertiesHLSL(info);
#else
		SetUpPropertiesStd140(info);
#endif

		return info;
	}

	bool ShaderParser::IsBasePropertyType(ShaderPropertyType type)
	{
		return !(type == ShaderPropertyType::SAMPLER || type == ShaderPropertyType::SAMPLER_2D || type == ShaderPropertyType::SAMPLER_CUBE
			|| type == ShaderPropertyType::ENGINE_DEPTH_MAP || type == ShaderPropertyType::ENGINE_DEPTH_CUBE_MAP);
	}

	UniformAlignInfo ShaderParser::GetPropertyAlignInfoStd140(ShaderPropertyType type, uint32_t arrayLength)
	{
		// ����Ҫע��Vulkan���ڴ����淶: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout
		// �����ǰ���std140��׼ʵ�ֵ��ڴ����

		uint32_t std_size = sizeof(float);
		if (type == ShaderPropertyType::BOOL || type == ShaderPropertyType::INT || type == ShaderPropertyType::FLOAT
			|| type == ShaderPropertyType::ENGINE_LIGHT_INTENSITY || type == ShaderPropertyType::ENGINE_FAR_PLANE)
			if (arrayLength == 0)
				return { .size = std_size, .align = std_size };
			else
				return { .size = (std_size * 4) * (arrayLength - 1) + std_size, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::VEC2)
			if (arrayLength == 0)
				return { .size = std_size * 2, .align = std_size * 2 };
			else
				return { .size = (std_size * 4) * (arrayLength - 1) + std_size * 2, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::VEC3 || type == ShaderPropertyType::ENGINE_CAMERA_POS
			|| type == ShaderPropertyType::ENGINE_LIGHT_POS || type == ShaderPropertyType::ENGINE_LIGHT_DIR
			|| type == ShaderPropertyType::ENGINE_LIGHT_COLOR)
			if (arrayLength == 0)
				return { .size = std_size * 3, .align = std_size * 4 };
			else
				return { .size = (std_size * 4) * (arrayLength - 1) + std_size * 3, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::VEC4)
			if (arrayLength == 0)
				return { .size = std_size * 4, .align = std_size * 4 };
			else
				return { .size = (std_size * 4) * arrayLength, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::MAT2)
			if (arrayLength == 0)
				return { .size = std_size * (4 + 2), .align = std_size * 4 };
			else
				return { .size = std_size * ((arrayLength * 2 - 1) * 4 + 2), .align = std_size * 4, .arrayOffset = std_size * 4 * 2 };

		else if (type == ShaderPropertyType::MAT3)
			if (arrayLength == 0)
				return { .size = std_size * (4 * 2 + 3), .align = std_size * 4 };
			else
				return { .size = std_size * ((arrayLength * 3 - 1) * 4 + 3), .align = std_size * 4, .arrayOffset = std_size * 4 * 3 };

		else if (type == ShaderPropertyType::MAT4 || type == ShaderPropertyType::ENGINE_MODEL
			|| type == ShaderPropertyType::ENGINE_VIEW || type == ShaderPropertyType::ENGINE_PROJECTION
			|| type == ShaderPropertyType::ENGINE_MODEL_INV)
			if (arrayLength == 0)
				return { .size = std_size * 16, .align = std_size * 4 };
			else
				return { .size = std_size * ((arrayLength * 4) * 4), .align = std_size * 4, .arrayOffset = std_size * 4 * 4 };
		else
		{
			Debug::LogError("Invalid shader property type !");
			return {};
		}
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
				GetPropertyNameAndArrayLength(words[1], property.name, property.arrayLength);
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
			if (words.size() >= 5 && words[0] != "//")
				glCode += "layout (location = " + words[0] + ") in " + words[1] + " " + words[2] + ";\n";
		}
		glCode += "\n";

		string outputBlock = GetCodeBlock(originCode, "Output");
		lines = Utils::StringSplit(outputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 5 && words[0] != "//")
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

		string programCode = GetCodeBlock(originCode, "Program");
		lines = Utils::StringSplit(programCode, '\n');
		for (auto& line : lines)
		{
			// �������˷�
			size_t pos = Utils::FindWord(line, "mul", 0);
			if (pos != string::npos)
			{
				size_t sPos = string::npos;
				size_t ePos = string::npos;
				Utils::GetNextStringBlockPos(line, pos, '(', ')', sPos, ePos);
				// ɾ�� mul ����������
				line.replace(pos, ePos - pos + 1, line.substr(sPos + 1, ePos - sPos - 1));
				glCode += line + "\n";
			}
			else
			{
				glCode += line + "\n";
			}
		}

		Utils::ReplaceAllWord(glCode, "to_vec2", "vec2");
		Utils::ReplaceAllWord(glCode, "to_vec3", "vec3");
		Utils::ReplaceAllWord(glCode, "to_mat3", "mat3");
		Utils::ReplaceAllString(glCode, "ZX_Depth", "gl_FragDepth");
		Utils::ReplaceAllString(glCode, "ZX_Position", "gl_Position");

		return glCode;
	}

	string ShaderParser::TranslateToVulkan(const string& originCode, const ShaderPropertiesInfo& info)
	{
		if (originCode.empty())
			return "";

		string vkCode = "#version 460 core\n\n";

		string gsInOutBlock = GetCodeBlock(originCode, "GSInOut");
		if (!gsInOutBlock.empty())
		{
			auto lines = Utils::StringSplit(gsInOutBlock, '\n');
			for (auto& line : lines)
				vkCode += line + ";\n";
			vkCode += "\n";
		}

		string inputBlock = GetCodeBlock(originCode, "Input");
		auto lines = Utils::StringSplit(inputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 5 && words[0] != "//")
				vkCode += "layout (location = " + words[0] + ") in " + words[1] + " " + words[2] + ";\n";
		}
		vkCode += "\n";

		string outputBlock = GetCodeBlock(originCode, "Output");
		lines = Utils::StringSplit(outputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 5 && words[0] != "//")
				vkCode += "layout (location = " + words[0] + ") out " + words[1] + " " + words[2] + ";\n";
		}
		vkCode += "\n";

		if (!info.baseProperties.empty())
		{
			vkCode += "layout (binding = " + to_string(info.baseProperties[0].binding) + ") uniform UniformBufferObject {\n";
			for (size_t i = 0; i < info.baseProperties.size(); i++)
			{
				auto& property = info.baseProperties[i];
				if (property.arrayLength == 0)
					vkCode += "    " + propertyTypeToGLSLType[property.type] + " " + property.name + ";\n";
				else
					vkCode += "    " + propertyTypeToGLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "];\n";
			}
			vkCode += "} _UBO;\n";
		}
		vkCode += "\n";

		for (auto& property : info.textureProperties)
		{
			vkCode += "layout (binding = " + to_string(property.binding) + ") uniform " + propertyTypeToGLSLType[property.type] + " " + property.name + ";\n";
		}
		vkCode += "\n";

		// ����UBO��������
		string programBlock = GetCodeBlock(originCode, "Program");
		if (!info.baseProperties.empty())
		{
			for (auto& property : info.baseProperties)
				Utils::ReplaceAllWord(programBlock, property.name, "_UBO." + property.name);
		}

		lines = Utils::StringSplit(programBlock, '\n');
		for (auto& line : lines)
		{
			// �������˷�
			size_t pos = Utils::FindWord(line, "mul", 0);
			if (pos != string::npos)
			{
				size_t sPos = string::npos;
				size_t ePos = string::npos;
				Utils::GetNextStringBlockPos(line, pos, '(', ')', sPos, ePos);
				// ɾ�� mul ����������
				line.replace(pos, ePos - pos + 1, line.substr(sPos + 1, ePos - sPos - 1));
				vkCode += line + "\n";
			}
			else
			{
				vkCode += line + "\n";
			}
		}

		Utils::ReplaceAllWord(vkCode, "to_vec2", "vec2");
		Utils::ReplaceAllWord(vkCode, "to_vec3", "vec3");
		Utils::ReplaceAllWord(vkCode, "to_mat3", "mat3");
		Utils::ReplaceAllString(vkCode, "ZX_Depth", "gl_FragDepth");
		Utils::ReplaceAllString(vkCode, "ZX_Position", "gl_Position");

		return vkCode;
	}

	string ShaderParser::TranslateToD3D12(const string& originCode, const ShaderInfo& shaderInfo)
	{
		if (originCode.empty())
			return "";

		string vertCode = GetCodeBlock(originCode, "Vertex");
		string geomCode = GetCodeBlock(originCode, "Geometry");
		string fragCode = GetCodeBlock(originCode, "Fragment");

		// ������ɫ������ṹ��
		string dxCode = "struct VertexInput\n{\n";
		string vertInputBlock = GetCodeBlock(vertCode, "Input");
		vector<string> vertInputVariables;
		auto lines = Utils::StringSplit(vertInputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 5 && words[0] != "//")
			{
				dxCode += "    " + words[1] + " " + words[2] + " " + words[3] + " " + words[4] + ";\n";
				vertInputVariables.push_back(words[2]);
			}
		}
		dxCode += "};\n\n";

		// ������ɫ������ṹ��
		dxCode += "struct VertexOutput\n{\n";
		string vertOutputBlock = GetCodeBlock(vertCode, "Output");
		vector<string> vertOutputVariables;
		lines = Utils::StringSplit(vertOutputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 5 && words[0] != "//")
			{
				dxCode += "    " + words[1] + " " + words[2] + " " + words[3] + " " + words[4] + ";\n";
				vertOutputVariables.push_back(words[2]);
			}
		}
		dxCode += "    float4 SVPos : SV_POSITION;\n";
		dxCode += "};\n\n";

		// ƬԪ(����)��ɫ������ṹ��
		dxCode += "struct PixelOutput\n{\n";
		string fragOutputBlock = GetCodeBlock(fragCode, "Output");
		vector<string> fragOutputVariables;
		lines = Utils::StringSplit(fragOutputBlock, '\n');
		for (auto& line : lines)
		{
			auto words = Utils::ExtractWords(line);
			if (words.size() >= 5 && words[0] != "//")
			{
				dxCode += "    " + words[1] + " " + words[2] + " " + words[3] + " " + words[4] + ";\n";
				fragOutputVariables.push_back(words[2]);
			}
		}
		dxCode += "};\n\n";

		// CPU�˳���Buffer
		dxCode += "cbuffer constantBuffer : register(b0)\n{\n";
		for (auto& property : shaderInfo.vertProperties.baseProperties)
		{
			if (property.arrayLength == 0)
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + ";\n";
			else
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "];\n";
		}
		for (auto& property : shaderInfo.geomProperties.baseProperties)
		{
			if (property.arrayLength == 0)
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + ";\n";
			else
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "];\n";
		}
		for (auto& property : shaderInfo.fragProperties.baseProperties)
		{
			if (property.arrayLength == 0)
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + ";\n";
			else
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "];\n";
		}
		dxCode += "};\n\n";

		// ����
		uint32_t textureIdx = 0;
		for (auto& property : shaderInfo.vertProperties.textureProperties)
		{
			if (property.arrayLength == 0)
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + " : register(t" + to_string(textureIdx) + ");\n";
			else
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "] : register(t" + to_string(textureIdx) + ");\n";
			textureIdx++;
		}
		for (auto& property : shaderInfo.geomProperties.textureProperties)
		{
			if (property.arrayLength == 0)
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + " : register(t" + to_string(textureIdx) + ");\n";
			else
				dxCode += "    " + propertyTypeToHLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "] : register(t" + to_string(textureIdx) + ");\n";
			textureIdx++;
		}
		for (auto& property : shaderInfo.fragProperties.textureProperties)
		{
			if (property.arrayLength == 0)
				dxCode += propertyTypeToHLSLType[property.type] + " " + property.name + " : register(t" + to_string(textureIdx) + ");\n";
			else
				dxCode += propertyTypeToHLSLType[property.type] + " " + property.name + "[" + to_string(property.arrayLength) + "] : register(t" + to_string(textureIdx) + ");\n";
			textureIdx++;
		}
		dxCode += "\n";

		// ������
		dxCode += "SamplerState sampleLinearWrap       : register(s0);\n";
		dxCode += "SamplerState sampleLinearClamp      : register(s1);\n";
		dxCode += "SamplerState sampleAnisotropicWrap  : register(s2);\n";
		dxCode += "SamplerState sampleAnisotropicClamp : register(s3);\n\n";

		// ������ɫ��
		string vertProgramBlock = GetCodeBlock(vertCode, "Program");
		lines = Utils::StringSplit(vertProgramBlock, '\n');
		// ���м�Ⲣ�������˷�
		for (auto& line : lines)
		{
			size_t pos = Utils::FindWord(line, "mul", 0);
			if (pos != string::npos)
			{
				size_t sPos = string::npos;
				size_t ePos = string::npos;
				Utils::GetNextStringBlockPos(line, pos, '(', ')', sPos, ePos);
				string multiplication = line.substr(sPos + 1, ePos - sPos - 1);
				vector<string> multipliers = Utils::StringSplit(multiplication, '*');
				string newMultiplication = "";
				while (multipliers.size() > 1)
				{
					string rightMul = multipliers.back();
					multipliers.pop_back();
					string leftMul = multipliers.back();
					multipliers.pop_back();

					multipliers.push_back("mul(" + leftMul + ", " + rightMul + ")");
				}
				// ɾ�� mul ����������
				line.replace(pos, ePos - pos + 1, multipliers[0]);
			}
			line += "\n";
		}
		vertProgramBlock = Utils::ConcatenateStrings(lines);
		// ȥ��main����
		for (auto& line : lines)
		{
			if (line.find("main") != string::npos)
				break;
			dxCode += line + "\n";
		}
		// ��������VS main����
		string vertMainBlock = GetCodeBlock(vertProgramBlock, "main");
		Utils::ReplaceAllWord(vertMainBlock, "ZX_Position", "output.SVPos");
		for (auto& varName : vertInputVariables)
			Utils::ReplaceAllWord(vertMainBlock, varName, "input." + varName);
		for (auto& varName : vertOutputVariables)
			Utils::ReplaceAllWord(vertMainBlock, varName, "output." + varName);
		dxCode += "VertexOutput VS(VertexInput input)\n";
		dxCode += "{\n";
		dxCode += "    VertexOutput output;\n";
		dxCode += vertMainBlock;
		dxCode += "    return output;\n";
		dxCode += "}\n\n";

		// ƬԪ(����)��ɫ��
		string fragProgramBlock = GetCodeBlock(fragCode, "Program");
		lines = Utils::StringSplit(fragProgramBlock, '\n');
		// ���м�Ⲣ�������˷�
		for (auto& line : lines)
		{
			size_t pos = Utils::FindWord(line, "mul", 0);
			if (pos != string::npos)
			{
				size_t sPos = string::npos;
				size_t ePos = string::npos;
				Utils::GetNextStringBlockPos(line, pos, '(', ')', sPos, ePos);
				string multiplication = line.substr(sPos + 1, ePos - sPos - 1);
				vector<string> multipliers = Utils::StringSplit(multiplication, '*');
				string newMultiplication = "";
				while (multipliers.size() > 1)
				{
					string rightMul = multipliers.back();
					multipliers.pop_back();
					string leftMul = multipliers.back();
					multipliers.pop_back();

					multipliers.push_back("mul(" + leftMul + ", " + rightMul + ")");
				}
				// ɾ�� mul ����������
				line.replace(pos, ePos - pos + 1, multipliers[0]);
			}
			line += "\n";
		}
		fragProgramBlock = Utils::ConcatenateStrings(lines);
		// ȥ��main����
		for (auto& line : lines)
		{
			if (line.find("main") != string::npos)
				break;
			dxCode += line + "\n";
		}
		// ��������PS main����
		string fragMainBlock = GetCodeBlock(fragProgramBlock, "main");
		for (auto& varName : vertOutputVariables)
			Utils::ReplaceAllWord(fragMainBlock, varName, "input." + varName);
		for (auto& varName : fragOutputVariables)
			Utils::ReplaceAllWord(fragMainBlock, varName, "output." + varName);
		dxCode += "PixelOutput PS(VertexOutput input)\n";
		dxCode += "{\n";
		dxCode += "    PixelOutput output;\n";
		dxCode += fragMainBlock;
		dxCode += "    return output;\n";
		dxCode += "}\n";

		// �滻��������﷨
		size_t pos = 0;
		while ((pos = dxCode.find("texture", pos)) != string::npos)
		{
			size_t s = 0, e = 0;
			for (size_t i = pos; i < dxCode.size(); i++)
			{
				if (dxCode[i] == '(')
				{
					s = i;
				}
				else if (dxCode[i] == ')')
				{
					e = i;
					break;
				}
			}

			string sampleSentence = dxCode.substr(s + 1, e - s - 1);
			auto sampleArgs = Utils::StringSplit(sampleSentence, ',');
			string oldSentence = dxCode.substr(pos, e - pos + 1);
			string newSentence = sampleArgs[0] + ".Sample(sampleLinearWrap," + sampleArgs[1] + ")";
			dxCode.replace(pos, oldSentence.length(), newSentence);

			pos += newSentence.length();
		}

		// ǿ������ת��
		Utils::ReplaceAllWord(dxCode, "to_vec2", "(float2)");
		Utils::ReplaceAllWord(dxCode, "to_vec3", "(float3)");
		Utils::ReplaceAllWord(dxCode, "to_mat3", "(float3x3)");

		// �滻������������
		Utils::ReplaceAllWord(dxCode, "vec2", "float2");
		Utils::ReplaceAllWord(dxCode, "vec3", "float3");
		Utils::ReplaceAllWord(dxCode, "vec4", "float4");
		Utils::ReplaceAllWord(dxCode, "mat3", "float3x3");
		Utils::ReplaceAllWord(dxCode, "mat4", "float4x4");

		return dxCode;
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
				stateSet.depthWrite = words[1] == "On";
			}
		}

		return stateSet;
	}

	string ShaderParser::GetCodeBlock(const string& code, const string& blockName)
	{
		auto begin = code.find(blockName);
		if (begin == string::npos)
			return "";

		// BlockName������һ�������Ĵʣ������������������߱�����һ����
		if ((begin > 0 && isalnum(code[begin - 1])) || (begin + blockName.size() < code.size() && isalnum(code[begin + blockName.size()])))
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

	void ShaderParser::GetPropertyNameAndArrayLength(const string& propertyStr, string& name, uint32_t& arrayLength)
	{
		size_t s = 0, e = 0;
		for (size_t i = 0; i < propertyStr.size(); i++)
		{
			if (propertyStr[i] == '[')
				s = i;
			else if (propertyStr[i] == ']')
				e = i;
		}

		// �����������
		if (s == 0 || e == 0)
		{
			name = propertyStr;
			return;
		}

		name = propertyStr.substr(0, s);
		string lengthStr = propertyStr.substr(s + 1, e - s - 1);
		arrayLength = static_cast<uint32_t>(std::stoi(lengthStr));
	}

	void ShaderParser::SetUpPropertiesStd140(ShaderInfo& info)
	{
		// �����ǰ���std140��׼ʵ�ֵ��ڴ����
		uint32_t binding = 0;

		if (!info.vertProperties.baseProperties.empty())
		{
			uint32_t offset = 0;
			for (auto& property : info.vertProperties.baseProperties)
			{
				auto alignInfo = GetPropertyAlignInfoStd140(property.type, property.arrayLength);
				property.binding = binding;
				property.size = alignInfo.size;
				property.align = alignInfo.align;
				property.arrayOffset = alignInfo.arrayOffset;
				uint32_t remainder = offset % alignInfo.align;
				if (remainder == 0)
					property.offset = offset;
				else
					property.offset = offset - remainder + alignInfo.align;

				offset = property.offset + property.size;
			}
			binding++;
		}
		for (auto& property : info.vertProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}

		if (!info.geomProperties.baseProperties.empty())
		{
			uint32_t offset = 0;
			for (auto& property : info.geomProperties.baseProperties)
			{
				auto alignInfo = GetPropertyAlignInfoStd140(property.type, property.arrayLength);
				property.binding = binding;
				property.size = alignInfo.size;
				property.align = alignInfo.align;
				property.arrayOffset = alignInfo.arrayOffset;
				uint32_t remainder = offset % alignInfo.align;
				if (remainder == 0)
					property.offset = offset;
				else
					property.offset = offset - remainder + alignInfo.align;

				offset = property.offset + property.size;
			}
			binding++;
		}
		for (auto& property : info.geomProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}

		if (!info.fragProperties.baseProperties.empty())
		{
			uint32_t offset = 0;
			for (auto& property : info.fragProperties.baseProperties)
			{
				auto alignInfo = GetPropertyAlignInfoStd140(property.type, property.arrayLength);
				property.binding = binding;
				property.size = alignInfo.size;
				property.align = alignInfo.align;
				property.arrayOffset = alignInfo.arrayOffset;
				uint32_t remainder = offset % alignInfo.align;
				if (remainder == 0)
					property.offset = offset;
				else
					property.offset = offset - remainder + alignInfo.align;

				offset = property.offset + property.size;
			}
			binding++;
		}
		for (auto& property : info.fragProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}
	}

	void ShaderParser::SetUpPropertiesHLSL(ShaderInfo& info)
	{
		// �����ǰ��� HLSL Packing Rule ��ʵ�ֵ�
		// ΢������ĵ�����: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
		// ΢��Github�ĵ�����: https://github.com/microsoft/DirectXShaderCompiler/wiki/Buffer-Packing
		// ֪���ĵ�����: https://zhuanlan.zhihu.com/p/560076693

		// Ŀǰ��ZXEngine��һ��HLSL Shaderֻ��һ��Constant Buffer�����б����ϸ���VS��GS��PS���Properties����˳������
		uint32_t offset = 0;
		if (!info.vertProperties.baseProperties.empty())
		{
			for (auto& property : info.vertProperties.baseProperties)
			{
				auto alignInfo = GetPropertyAlignInfoHLSL(property.type, property.arrayLength);
				property.size = alignInfo.size;
				property.align = alignInfo.align;
				property.arrayOffset = alignInfo.arrayOffset;
				// ��ǰ��16�ֽڼĴ�����ʣ�����ֽ�
				uint32_t remainder = 16 - (offset % 16);
				// �ж�һ��ʣ���⼸���ֽڹ�����װ��һ�����ݣ���һ����������Ǿ����������Ļ����ǿ�������װһ��Ԫ�أ�������������һ��Ԫ�رض���Ҫ16�ֽڶ���
				if (remainder >= alignInfo.align)
					// �����װ�Ļ�����ֱ�ӷŽ�ȥ��Ҳ����offsetֱ���ǵ�ǰʵ��ռ�õ�offset
					property.offset = offset;
				else
					// �������װ�Ļ�������Ҫ�������16�ֽڼĴ���������һ��16�ֽڼĴ�����Ҳ����offset��Ҫ����remainder�����뵽��һ������16�ֽڵ�λ��
					property.offset = offset + remainder;

				// offset��������ʵ��ռ�ô�С����ƫ��
				offset = property.offset + property.size;
			}
		}
		if (!info.geomProperties.baseProperties.empty())
		{
			for (auto& property : info.geomProperties.baseProperties)
			{
				auto alignInfo = GetPropertyAlignInfoHLSL(property.type, property.arrayLength);
				property.size = alignInfo.size;
				property.align = alignInfo.align;
				property.arrayOffset = alignInfo.arrayOffset;
				uint32_t remainder = 16 - (offset % 16);
				if (remainder >= alignInfo.align)
					property.offset = offset;
				else
					property.offset = offset + remainder;

				offset = property.offset + property.size;
			}
		}
		if (!info.fragProperties.baseProperties.empty())
		{
			for (auto& property : info.fragProperties.baseProperties)
			{
				auto alignInfo = GetPropertyAlignInfoHLSL(property.type, property.arrayLength);
				property.size = alignInfo.size;
				property.align = alignInfo.align;
				property.arrayOffset = alignInfo.arrayOffset;
				uint32_t remainder = 16 - (offset % 16);
				if (remainder >= alignInfo.align)
					property.offset = offset;
				else
					property.offset = offset + remainder;

				offset = property.offset + property.size;
			}
		}

		// �����binding��ָ������HLSL���register(t)�������ϸ���VS��GS��PS�����������˳������
		uint32_t binding = 0;
		for (auto& property : info.vertProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}
		for (auto& property : info.geomProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}
		for (auto& property : info.fragProperties.textureProperties)
		{
			property.binding = binding;
			binding++;
		}
	}

	D3D12ConstAlignInfo ShaderParser::GetPropertyAlignInfoHLSL(ShaderPropertyType type, uint32_t arrayLength)
	{
		uint32_t std_size = sizeof(float);
		if (type == ShaderPropertyType::BOOL || type == ShaderPropertyType::INT || type == ShaderPropertyType::FLOAT
			|| type == ShaderPropertyType::ENGINE_LIGHT_INTENSITY || type == ShaderPropertyType::ENGINE_FAR_PLANE)
			if (arrayLength == 0)
				return { .size = std_size, .align = std_size };
			else
				return { .size = (std_size * 4) * (arrayLength - 1) + std_size, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::VEC2)
			if (arrayLength == 0)
				return { .size = std_size * 2, .align = std_size * 2 };
			else
				return { .size = (std_size * 4) * (arrayLength - 1) + std_size * 2, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::VEC3 || type == ShaderPropertyType::ENGINE_CAMERA_POS
			|| type == ShaderPropertyType::ENGINE_LIGHT_POS || type == ShaderPropertyType::ENGINE_LIGHT_DIR
			|| type == ShaderPropertyType::ENGINE_LIGHT_COLOR)
			if (arrayLength == 0)
				return { .size = std_size * 3, .align = std_size * 3 };
			else
				return { .size = (std_size * 4) * (arrayLength - 1) + std_size * 3, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::VEC4)
			if (arrayLength == 0)
				return { .size = std_size * 4, .align = std_size * 4 };
			else
				return { .size = (std_size * 4) * arrayLength, .align = std_size * 4, .arrayOffset = std_size * 4 };

		else if (type == ShaderPropertyType::MAT2)
			if (arrayLength == 0)
				return { .size = std_size * (4 + 2), .align = std_size * 4 };
			else
				return { .size = std_size * ((arrayLength * 2 - 1) * 4 + 2), .align = std_size * 4, .arrayOffset = std_size * 4 * 2 };

		else if (type == ShaderPropertyType::MAT3)
			if (arrayLength == 0)
				return { .size = std_size * (4 * 2 + 3), .align = std_size * 4 };
			else
				return { .size = std_size * ((arrayLength * 3 - 1) * 4 + 3), .align = std_size * 4, .arrayOffset = std_size * 4 * 3 };

		else if (type == ShaderPropertyType::MAT4 || type == ShaderPropertyType::ENGINE_MODEL
			|| type == ShaderPropertyType::ENGINE_VIEW || type == ShaderPropertyType::ENGINE_PROJECTION
			|| type == ShaderPropertyType::ENGINE_MODEL_INV)
			if (arrayLength == 0)
				return { .size = std_size * 16, .align = std_size * 4 };
			else
				return { .size = std_size * ((arrayLength * 4) * 4), .align = std_size * 4, .arrayOffset = std_size * 4 * 4 };
		else
		{
			Debug::LogError("Invalid shader property type !");
			return {};
		}
	}
}