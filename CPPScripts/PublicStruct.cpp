#include "PublicStruct.h"
#include <stb_image.h>

namespace ZXEngine
{
	void Vertex::AddBoneData(uint32_t boneID, float weight)
	{
		for (uint32_t i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
		{
			if (Weights[i] == 0.0f)
			{
				BoneIDs[i] = boneID;
				Weights[i] = weight;
				return;
			}
		}
		Debug::LogWarning("Vertex has more than %s bones!", MAX_NUM_BONES_PER_VERTEX);
	}

	BoneNode::~BoneNode()
	{ 
		for (auto child : children)
			delete child; 
	}

	TextureFullData::~TextureFullData() 
	{ 
		if (data) stbi_image_free(data); 
	}

	CubeMapFullData::~CubeMapFullData() 
	{ 
		for (int i = 0; i < 6; i++)
		{
			if (data[i]) stbi_image_free(data[i]);
		}
	}
}