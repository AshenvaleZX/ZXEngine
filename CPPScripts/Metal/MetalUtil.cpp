#include "MetalUtil.h"
#include "../Resources.h"
#include "../ShaderParser.h"

namespace ZXEngine
{
	void MetalUtil::TranslateAllShaderToMSL(const string& path)
	{
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string extension = entry.path().filename().extension().string();
			if (extension == ".zxshader")
				TranslateShaderToMSL(entry.path());
			else if (entry.is_directory())
				TranslateAllShaderToMSL(entry.path().string());
		}
	}

	void MetalUtil::TranslateShaderToMSL(const filesystem::path& path)
	{
		string shaderCode = Resources::LoadTextFile(path.string());
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::Metal);
		string mslCode = ShaderParser::TranslateToMetal(shaderCode, shaderInfo);

		string fileName = path.stem().string();
		string folderPath = path.parent_path().string();
		string writePath = folderPath + "/" + fileName + ".msl";

		ofstream writer;
		try
		{
			writer.open(writePath);
			writer << mslCode;
			writer.close();
		}
		catch (ofstream::failure e)
		{
			Debug::LogError("Generate MSL file failed: " + path.string());
		}
	}
}