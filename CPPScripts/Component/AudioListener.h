#pragma once
#include "Component.h"

namespace ZXEngine
{
	class AudioListener : public Component
	{
	public:
		static ComponentType GetType();

	public:
		AudioListener() {};
		~AudioListener();

		virtual void Awake();
		virtual ComponentType GetInsType();
	};
}