#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class GameLogic : public Component
	{
	public:
		string lua;

		GameLogic();
		~GameLogic();

		void Update();

	};
}