#pragma once

namespace ZXEngine
{
	enum RenderQueueType 
	{ 
		Qpaque = 1000, 
		Transparent = 2000 
	};

	enum LightType
	{
		None,
		Directional,
		Point,
	};

	enum EventType
	{
		PLACE_HOLDER = 0, // 这里加一个占位符，因为Lua也有一份EventType需要和这里对齐，但是Lua下标是从1开始的
		GAME_START,
		UPDATE_MOUSE_POS,

		KEY_0_PRESS,
		KEY_1_PRESS,
		KEY_2_PRESS,
		KEY_3_PRESS,
		KEY_4_PRESS,
		KEY_5_PRESS,
		KEY_6_PRESS,
		KEY_7_PRESS,
		KEY_8_PRESS,
		KEY_9_PRESS,

		KEY_A_PRESS,
		KEY_B_PRESS,
		KEY_C_PRESS,
		KEY_D_PRESS,
		KEY_E_PRESS,
		KEY_F_PRESS,
		KEY_G_PRESS,
		KEY_H_PRESS,
		KEY_I_PRESS,
		KEY_J_PRESS,
		KEY_K_PRESS,
		KEY_L_PRESS,
		KEY_M_PRESS,
		KEY_N_PRESS,
		KEY_O_PRESS,
		KEY_P_PRESS,
		KEY_Q_PRESS,
		KEY_R_PRESS,
		KEY_S_PRESS,
		KEY_T_PRESS,
		KEY_U_PRESS,
		KEY_V_PRESS,
		KEY_W_PRESS,
		KEY_X_PRESS,
		KEY_Y_PRESS,
		KEY_Z_PRESS,

		KEY_SPACE_PRESS,
		KEY_ESCAPE_PRESS,

		KEY_RIGHT_PRESS,
		KEY_LEFT_PRESS,
		KEY_DOWN_PRESS,
		KEY_UP_PRESS,
	};
}