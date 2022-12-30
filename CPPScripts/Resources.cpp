#include "Resources.h"

namespace ZXEngine
{
	string Resources::assetsPath;
	const string Resources::builtInAssetsPath = "../../BuiltInAssets/";
	void Resources::SetAssetsPath(string path)
	{
		assetsPath = path;
	}
	
	string Resources::GetAssetsPath()
	{
		return assetsPath;
	}

	string Resources::GetAssetFullPath(string path, bool isBuiltIn)
	{
		if (isBuiltIn)
			return builtInAssetsPath + path;
		else
			return assetsPath + path;
	}

	string Resources::GetAssetLocalPath(string path)
	{
		int s = path.find("Assets");
		if (s > 0)
			return path.substr(s + 7, path.length() - s - 7);
		else
			return path;
	}

	string Resources::GetAssetName(string path)
	{
		int s = path.rfind("/");
		int e = path.rfind(".");
		return path.substr(s + 1, e - s - 1);
	}

	string Resources::JsonStrToString(json data)
	{
		string p = to_string(data);
		// 这个json字符串取出来前后会有双引号，需要去掉再用
		p = p.substr(1, p.length() - 2);
		return p;
	}

	json Resources::LoadJson(string path)
	{
		std::ifstream f(path);
		if (!f.is_open())
		{
			Debug::LogError("Load asset failed: " + path);
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

	json Resources::GetAssetData(string path, bool isBuiltIn)
	{
		string p = Resources::GetAssetFullPath(path, isBuiltIn);
		Debug::Log("Load asset: " + p);
		return Resources::LoadJson(p);
	}

	SceneStruct* Resources::LoadScene(string path)
	{
		json data = Resources::GetAssetData(path);
		SceneStruct* scene = new SceneStruct;

		scene->skyBox = Resources::LoadCubeMap(data["SkyBox"]);

		for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
		{
			string p = Resources::JsonStrToString(data["GameObjects"][i]);
			PrefabStruct* prefab = Resources::LoadPrefab(p);
			scene->prefabs.push_back(prefab);
		}

		return scene;
	}

	PrefabStruct* Resources::LoadPrefab(string path, bool isBuiltIn)
	{
		json data = Resources::GetAssetData(path, isBuiltIn);
		return ParsePrefab(data);
	}

	PrefabStruct* Resources::ParsePrefab(json data)
	{
		PrefabStruct* prefab = new PrefabStruct;

		prefab->name = data["Name"];
		if (data["Layer"].is_null())
			prefab->layer = (int)GameObjectLayer::Default;
		else
			prefab->layer = data["Layer"];

		for (unsigned int i = 0; i < data["Components"].size(); i++)
		{
			json component = data["Components"][i];
			prefab->components.push_back(component);
		}

		if (!data["GameObjects"].is_null())
		{
			for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
			{
				json subData = data["GameObjects"][i];
				auto subPrefab = ParsePrefab(subData);
				subPrefab->parent = prefab;
				prefab->children.push_back(subPrefab);
			}
		}

		return prefab;
	}

	MaterialStruct* Resources::LoadMaterial(string path, bool isBuiltIn)
	{
		MaterialStruct* matStruct = new MaterialStruct;
		matStruct->name = GetAssetName(path);

		json data = Resources::GetAssetData(path, isBuiltIn);
		string p = Resources::JsonStrToString(data["Shader"]);
		matStruct->shaderPath = Resources::GetAssetFullPath(p, isBuiltIn);

		for (unsigned int i = 0; i < data["Textures"].size(); i++)
		{
			json texture = data["Textures"][i];
			TextureStruct* textureStruct = new TextureStruct();
			textureStruct->path = Resources::GetAssetFullPath(Resources::JsonStrToString(texture["Path"]), isBuiltIn);
			textureStruct->uniformName = Resources::JsonStrToString(texture["UniformName"]);

			matStruct->textures.push_back(textureStruct);
		}

		return matStruct;
	}

	vector<string> Resources::LoadCubeMap(json data, bool isBuiltIn)
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