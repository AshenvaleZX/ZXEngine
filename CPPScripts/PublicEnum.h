#pragma once

namespace ZXEngine
{
	enum RenderQueueType 
	{ 
		Qpaque = 1000, 
		Transparent = 2000 
	};

	enum EventType
	{
		GAME_START = 0,
		UPDATE_MOUSE_POS,
	};
}