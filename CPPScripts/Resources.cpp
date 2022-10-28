#include "Resources.h"

namespace ZXEngine
{
	const char* Resources::assetsPath;
	void Resources::SetAssetsPath(const char* path)
	{
		assetsPath = path;
	}

	string Resources::GetAssetFullPath(const char* path)
	{
		std::stringstream ss;
		ss << assetsPath << path;
		return ss.str();
	}

	json Resources::GetAssetData(const char* path)
	{
		string p = Resources::GetAssetFullPath(path);
		Debug::Log("Load asset: " + p);
		std::ifstream f(p);
		if (!f.is_open())
		{
			Debug::LogError("Load asset failed: " + p);
		}
		return json::parse(f);
	}

	SceneStruct* Resources::LoadScene(const char* path)
	{
		json data = Resources::GetAssetData(path);
		SceneStruct* scene = new SceneStruct;

		for (unsigned int i = 0; i < data["Cameras"].size(); i++)
		{
			Debug::Log(to_string(data["Cameras"][i]));
			CameraStruct* cam = new CameraStruct;
			scene->cameras.push_back(cam);
		}

		for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
		{
			string p = to_string(data["GameObjects"][i]);
			// ���json�ַ���ȡ����ǰ�����˫���ţ���Ҫȥ������
			p = p.substr(1, p.length()-2);
			PrefabStruct* prefab = Resources::LoadPrefab(p.c_str());
			scene->prefabs.push_back(prefab);
		}

		return scene;
	}

	PrefabStruct* Resources::LoadPrefab(const char* path)
	{
		json data = Resources::GetAssetData(path);
		PrefabStruct* prefab = new PrefabStruct;

		for (unsigned int i = 0; i < data["Components"].size(); i++)
		{
			json component = data["Components"][i];
			prefab->components.push_back(component);
		}

		return prefab;
	}

	MaterialStruct* Resources::LoadMaterial(const char* path)
	{
		json data = Resources::GetAssetData(path);
		MaterialStruct* matStruct = new MaterialStruct;

		string p = to_string(data["Shader"]);
		// ���json�ַ���ȡ����ǰ�����˫���ţ���Ҫȥ������
		p = p.substr(1, p.length() - 2);
		matStruct->shaderPath = Resources::GetAssetFullPath(p.c_str());

		return matStruct;
	}
}