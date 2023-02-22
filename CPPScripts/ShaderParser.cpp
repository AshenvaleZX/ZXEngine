#include "ShaderParser.h"
#include "Resources.h"

namespace ZXEngine
{
	void ShaderParser::ParseFile(string path, ShaderData& data)
	{
		string shaderCode = Resources::LoadTextFile(path);

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
}