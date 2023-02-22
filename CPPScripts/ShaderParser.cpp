#include "ShaderParser.h"
#include "Resources.h"

namespace ZXEngine
{
	void ShaderParser::ParseFile(string path, ShaderData& data)
	{
		string shaderCode = Resources::LoadTextFile(path);

		// �������������õ���string�����ר�����ͣ���Ϊstring���find��substr�Ȳ������ص���Щ�������ͺ;�����뻷���й�
		// �ر���find�����Ϻܶ�ط�˵û�ҵ��ͻ᷵��-1����ʵ���˵����׼ȷ����Ϊfind�ĺ������巵�ص�������size_t
		// ��size_t��һ���޷�������(�������λȡ���ڱ��뻷��)��һ���޷����������-1������Ϊ����ˣ�ʵ����û�ҵ���ʱ���������ص���npos
		// ��ʵֱ����int������Ҳ�У����Զ���ʽת����Ҳ������-1���ж��Ƿ��ҵ��������������б����Warning
		// Ϊ���ڸ��ֱ��뻷���²���������ֱ�Ӳ���ԭ�����е�string::size_type��string::npos����յģ����Ҳ�����Warning
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
		// ��δʵ��
		string vkCode = originCode;
		return vkCode;
	}
}