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

	ShaderInfo ShaderParser::GetShaderInfo(const string& path)
	{
		ShaderInfo info;

		string shaderCode = Resources::LoadTextFile(path);
		info.stateSet = GetShaderStateSet(shaderCode);

		// �������������õ���string�����ר�����ͣ���Ϊstring���find��substr�Ȳ������ص���Щ�������ͺ;�����뻷���й�
		// �ر���find�����Ϻܶ�ط�˵û�ҵ��ͻ᷵��-1����ʵ���˵����׼ȷ����Ϊfind�ĺ������巵�ص�������size_t
		// ��size_t��һ���޷�������(�������λȡ���ڱ��뻷��)��һ���޷����������-1������Ϊ����ˣ�ʵ����û�ҵ���ʱ���������ص���npos
		// ��ʵֱ����int������Ҳ�У����Զ���ʽת����Ҳ������-1���ж��Ƿ��ҵ��������������б����Warning
		// Ϊ���ڸ��ֱ��뻷���²���������ֱ�Ӳ���ԭ�����е�string::size_type��string::npos����յģ����Ҳ�����Warning
		string::size_type hasDirLight = shaderCode.find("DirLight");
		string::size_type hasPointLight = shaderCode.find("PointLight");
		if (hasDirLight != string::npos)
			info.lightType = LightType::Directional;
		else if (hasPointLight != string::npos)
			info.lightType = LightType::Point;
		else
			info.lightType = LightType::None;

		string::size_type hasDirShadow = shaderCode.find("_DepthMap");
		string::size_type hasPointShadow = shaderCode.find("_DepthCubeMap");
		if (hasDirShadow != string::npos)
			info.shadowType = ShadowType::Directional;
		else if (hasPointShadow != string::npos)
			info.shadowType = ShadowType::Point;
		else
			info.shadowType = ShadowType::None;

		return info;
	}

	void ShaderParser::ParseShaderCode(const string& path, string& vertCode, string& geomCode, string& fragCode)
	{
		string shaderCode = Resources::LoadTextFile(path);

		vertCode = GetCodeBlock(shaderCode, "Vertex");
		geomCode = GetCodeBlock(shaderCode, "Geometry");
		fragCode = GetCodeBlock(shaderCode, "Fragment");
	}

	string ShaderParser::TranslateToVulkan(const string& originCode)
	{
		// ��δʵ��
		string vkCode = originCode;
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
}