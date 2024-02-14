#include "Resources.h"

namespace ZXEngine
{
	string Resources::mAssetsPath;
	const string Resources::mBuiltInAssetsPath = "../../BuiltInAssets/";
	void Resources::SetAssetsPath(const string& path)
	{
		mAssetsPath = path;
	}
	
	string Resources::GetAssetsPath()
	{
		return mAssetsPath;
	}

	string Resources::GetAssetFullPath(const string& path, bool isBuiltIn)
	{
		if (isBuiltIn)
			return mBuiltInAssetsPath + path;
		else
			return mAssetsPath + path;
	}

	string Resources::GetAssetLocalPath(const string& path)
	{
		size_t s = path.find("Assets");
		if (s > 0)
			return path.substr(s + 7, path.length() - s - 7);
		else
			return path;
	}

	string Resources::GetAssetName(const string& path)
	{
		size_t s = path.rfind("/");
		size_t e = path.rfind(".");
		return path.substr(s + 1, e - s - 1);
	}

	string Resources::GetAssetExtension(const string& path)
	{
		size_t s = path.rfind(".");
		return path.substr(s + 1, path.length() - s - 1);
	}

	string Resources::JsonStrToString(const json& data)
	{
		string p = to_string(data);
		// 这个json字符串取出来前后会有双引号，需要去掉再用
		p = p.substr(1, p.length() - 2);
		return p;
	}

	json Resources::LoadJson(const string& path)
	{
		std::ifstream f(path);
		if (!f.is_open())
		{
			Debug::LogError("Load asset failed: " + path);
			return NULL;
		}
		json data;
		try
		{
			data = json::parse(f);
		}
		catch (json::exception& e)
		{
			Debug::LogError("Asset format error: " + path);
			string msg = e.what();
			Debug::LogError("Error detail: " + msg);
		}
		return data;
	}

	string Resources::LoadTextFile(const string& path)
	{
		string text = "";
		ifstream file;
		file.exceptions(ifstream::failbit | ifstream::badbit);

		try
		{
			file.open(path);
			stringstream stream;
			stream << file.rdbuf();
			file.close();
			text = stream.str();
		}
		catch (ifstream::failure e)
		{
			Debug::LogError("Failed to load text file: " + path);
		}

		return text;
	}

	vector<char> Resources::LoadBinaryFile(const string& path)
	{
		// ate:在文件末尾开始读取，从文件末尾开始读取的优点是我们可以使用读取位置来确定文件的大小并分配缓冲区
		ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			Debug::LogError("Failed to load binary file: " + path);

		// 使用读取位置来确定文件的大小并分配缓冲区
		size_t fileSize = (size_t)file.tellg();
		vector<char> data(fileSize);

		// 返回文件开头，真正读取内容
		file.seekg(0);
		file.read(data.data(), fileSize);
		file.close();

		return data;
	}

	json Resources::GetAssetData(const string& path, bool isBuiltIn)
	{
		string p = Resources::GetAssetFullPath(path, isBuiltIn);
		Debug::Log("Load asset: " + p);
		return Resources::LoadJson(p);
	}

	SceneStruct* Resources::LoadScene(const string& path)
	{
		json data = Resources::GetAssetData(path);
		SceneStruct* scene = new SceneStruct;

		scene->skyBox = Resources::LoadCubeMap(data["SkyBox"]);

		if (!data["RenderPipelineType"].is_null())
			scene->renderPipelineType = data["RenderPipelineType"];

		for (size_t i = 0; i < data["GameObjects"].size(); i++)
		{
			string p = Resources::JsonStrToString(data["GameObjects"][i]);
			PrefabStruct* prefab = Resources::LoadPrefab(p);
			scene->prefabs.push_back(prefab);
		}

		if (scene->renderPipelineType == RenderPipelineType::RayTracing && !data["RayTracingShaderGroups"].is_null())
		{
			if (!data["RayTracingShaderGroups"]["RayGen"].is_null())
				for (size_t i = 0; i < data["RayTracingShaderGroups"]["RayGen"].size(); i++)
					scene->rtShaderPathGroup.rGenPaths.push_back(Resources::JsonStrToString(data["RayTracingShaderGroups"]["RayGen"][i]));

			if (!data["RayTracingShaderGroups"]["Miss"].is_null())
				for (size_t i = 0; i < data["RayTracingShaderGroups"]["Miss"].size(); i++)
					scene->rtShaderPathGroup.rMissPaths.push_back(Resources::JsonStrToString(data["RayTracingShaderGroups"]["Miss"][i]));

			if (!data["RayTracingShaderGroups"]["HitGroups"].is_null())
			{
				for (size_t i = 0; i < data["RayTracingShaderGroups"]["HitGroups"].size(); i++)
				{
					RayTracingHitGroupPath hitGroupPath;

					if (!data["RayTracingShaderGroups"]["HitGroups"][i]["ClosestHit"].is_null())
						hitGroupPath.rClosestHitPath = Resources::JsonStrToString(data["RayTracingShaderGroups"]["HitGroups"][i]["ClosestHit"]);
					if (!data["RayTracingShaderGroups"]["HitGroups"][i]["AnyHit"].is_null())
						hitGroupPath.rAnyHitPath = Resources::JsonStrToString(data["RayTracingShaderGroups"]["HitGroups"][i]["AnyHit"]);
					if (!data["RayTracingShaderGroups"]["HitGroups"][i]["Intersection"].is_null())
						hitGroupPath.rIntersectionPath = Resources::JsonStrToString(data["RayTracingShaderGroups"]["HitGroups"][i]["Intersection"]);

					scene->rtShaderPathGroup.rHitGroupPaths.push_back(std::move(hitGroupPath));
				}
			}
		}

		return scene;
	}

	PrefabStruct* Resources::LoadPrefab(const string& path, bool isBuiltIn)
	{
		json data = Resources::GetAssetData(path, isBuiltIn);
		return ParsePrefab(data);
	}

	PrefabStruct* Resources::ParsePrefab(json data)
	{
		PrefabStruct* prefab = new PrefabStruct;

		prefab->name = data["Name"];
		if (data["Layer"].is_null())
			prefab->layer = static_cast<uint32_t>(GameObjectLayer::Default);
		else
			prefab->layer = data["Layer"];

		for (unsigned int i = 0; i < data["Components"].size(); i++)
		{
			const json& component = data["Components"][i];
			prefab->components.push_back(component);
		}

		if (!data["GameObjects"].is_null())
		{
			for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
			{
				const json& subData = data["GameObjects"][i];
				auto subPrefab = ParsePrefab(subData);
				subPrefab->parent = prefab;
				prefab->children.push_back(subPrefab);
			}
		}

		return prefab;
	}

	MaterialStruct* Resources::LoadMaterial(const string& path, bool isBuiltIn)
	{
		MaterialStruct* matStruct = new MaterialStruct;
		matStruct->path = path;
		matStruct->name = GetAssetName(path);
		matStruct->type = GetAssetExtension(path) == "zxmat" ? MaterialType::Rasterization : MaterialType::RayTracing;

		json data = Resources::GetAssetData(path, isBuiltIn);

		if (matStruct->type == MaterialType::Rasterization)
		{
			string p = Resources::JsonStrToString(data["Shader"]);
			matStruct->shaderPath = Resources::GetAssetFullPath(p, isBuiltIn);
		}
		else
		{
			if (!data["HitGroup"].is_null())
				matStruct->hitGroupIdx = data["HitGroup"];
		}

		for (size_t i = 0; i < data["Float"].size(); i++)
		{
			matStruct->floatDatas[data["Float"][i]["UniformName"]] = data["Float"][i]["Value"];
		}

		for (size_t i = 0; i < data["UInt"].size(); i++)
		{
			matStruct->uintDatas[data["UInt"][i]["UniformName"]] = data["UInt"][i]["Value"];
		}

		for (size_t i = 0; i < data["Vector2"].size(); i++)
		{
			const json& vec2 = data["Vector2"][i];
			Vector2 value = Vector2(vec2["Value"][0], vec2["Value"][1]);
			matStruct->vec2Datas[vec2["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Vector3"].size(); i++)
		{
			const json& vec3 = data["Vector3"][i];
			Vector3 value = Vector3(vec3["Value"][0], vec3["Value"][1], vec3["Value"][2]);
			matStruct->vec3Datas[vec3["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Vector4"].size(); i++)
		{
			const json& vec4 = data["Vector4"][i];
			Vector4 value = Vector4(vec4["Value"][0], vec4["Value"][1], vec4["Value"][2], vec4["Value"][3]);
			matStruct->vec4Datas[vec4["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Textures"].size(); i++)
		{
			const json& texture = data["Textures"][i];

			TextureStruct* textureStruct = new TextureStruct();
			textureStruct->path = Resources::GetAssetFullPath(Resources::JsonStrToString(texture["Path"]), isBuiltIn);
			textureStruct->uniformName = Resources::JsonStrToString(texture["UniformName"]);

			matStruct->textures.push_back(textureStruct);
		}

		for (size_t i = 0; i < data["CubeMaps"].size(); i++)
		{
			const json& cubeMap = data["CubeMaps"][i];

			CubeMapStruct* cubeMapStruct = new CubeMapStruct();
			cubeMapStruct->paths = Resources::LoadCubeMap(cubeMap, isBuiltIn);
			cubeMapStruct->uniformName = Resources::JsonStrToString(cubeMap["UniformName"]);

			matStruct->cubeMaps.push_back(cubeMapStruct);
		}

		return matStruct;
	}

	vector<string> Resources::LoadCubeMap(const json& data, bool isBuiltIn)
	{
		vector<string> cube;
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Right"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Left"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Up"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Down"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Front"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Back"]));
		return cube;
	}
}