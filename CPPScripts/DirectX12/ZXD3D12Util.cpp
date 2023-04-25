#include "ZXD3D12Util.h"
#include "../Resources.h"
#include "../ShaderParser.h"

namespace ZXEngine
{
	void ZXD3D12Util::TranslateAllShaderToHLSL(const string& path)
	{
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string extension = entry.path().filename().extension().string();
			if (extension == ".zxshader")
				TranslateShaderToHLSL(entry.path().string());
			else if (extension == "")
				TranslateAllShaderToHLSL(entry.path().string());
		}
	}

	void ZXD3D12Util::TranslateShaderToHLSL(const filesystem::path& path)
	{
		string shaderCode = Resources::LoadTextFile(path.string());
		auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode);
		string hlslCode = ShaderParser::TranslateToD3D12(shaderCode, shaderInfo);

		string fileName = path.stem().string();
		string folderPath = path.parent_path().string();
		string writePath = folderPath + "/" + fileName + ".hlsl";

		ofstream writer;
		try
		{
			writer.open(writePath);
			writer << hlslCode;
			writer.close();
		}
		catch (ofstream::failure e)
		{
			Debug::LogError("Generate HLSL file failed: " + path.string());
		}
	}
}