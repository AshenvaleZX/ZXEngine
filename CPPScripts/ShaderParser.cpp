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

	void ShaderParser::ParseFile(string path, ShaderData& data)
	{
		string shaderCode = Resources::LoadTextFile(path);

		ShaderStateSet stateSet = GetShaderStateSet(shaderCode);

		// 这里数字类型用的是string库里的专用类型，因为string库的find，substr等操作返回的这些数据类型和具体编译环境有关
		// 特别是find，网上很多地方说没找到就会返回-1，其实这个说法不准确，因为find的函数定义返回的类型是size_t
		// 而size_t是一个无符号整数(具体多少位取决于编译环境)，一个无符号整数变成-1，是因为溢出了，实际上没找到的时候真正返回的是npos
		// 其实直接用int来处理也行，会自动隐式转换，也可以用-1来判断是否找到，但是这样会有编译的Warning
		// 为了在各种编译环境下不出错，这里直接采用原定义中的string::size_type和string::npos是最保险的，并且不会有Warning
		string::size_type hasDirLight = shaderCode.find("DirLight");
		string::size_type hasPointLight = shaderCode.find("PointLight");
		if (hasDirLight != string::npos)
			data.lightType = LightType::Directional;
		else if (hasPointLight != string::npos)
			data.lightType = LightType::Point;
		else
			data.lightType = LightType::None;

		string::size_type hasDirShadow = shaderCode.find("_DepthMap");
		string::size_type hasPointShadow = shaderCode.find("_DepthCubeMap");
		if (hasDirShadow != string::npos)
			data.shadowType = ShadowType::Directional;
		else if (hasPointShadow != string::npos)
			data.shadowType = ShadowType::Point;
		else
			data.shadowType = ShadowType::None;

		string::size_type vs_begin = shaderCode.find("#vs_begin") + 9;
		string::size_type vs_end = shaderCode.find("#vs_end");
		data.vertexCode = shaderCode.substr(vs_begin, vs_end - vs_begin);

		string::size_type gs_begin = shaderCode.find("#gs_begin") + 9;
		string::size_type gs_end = shaderCode.find("#gs_end");
		data.geometryCode = shaderCode.substr(gs_begin, gs_end - gs_begin);

		string::size_type fs_begin = shaderCode.find("#fs_begin") + 9;
		string::size_type fs_end = shaderCode.find("#fs_end");
		data.fragmentCode = shaderCode.substr(fs_begin, fs_end - fs_begin);
	}

	string ShaderParser::TranslateToVulkan(const string& originCode)
	{
		// 暂未实现
		string vkCode = originCode;
		return vkCode;
	}

	ShaderStateSet ShaderParser::GetShaderStateSet(const string& code)
	{
		string::size_type st_begin = code.find("#st_begin");
		string::size_type st_end = code.find("#st_end");

		ShaderStateSet stateSet;
		if (st_begin == string::npos or st_end == string::npos)
			return stateSet;

		string stSegment = code.substr(st_begin, st_end - st_begin);
		auto lines = Utils::StringSplit(stSegment, '\n');

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
}