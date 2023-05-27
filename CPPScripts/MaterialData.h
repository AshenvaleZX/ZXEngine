#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Texture;
	class MaterialData
	{
	public:
		bool initialized = false;
		vector<pair<string, Texture*>> textures;

		MaterialData();
		~MaterialData();

		void Use();
		uint32_t GetID();
		uint32_t GetRTID();

	private:
		uint32_t ID = 0;
		uint32_t rtID = 0;
	};
}