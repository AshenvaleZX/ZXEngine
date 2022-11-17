-- 这里需要和引擎里的PublicEnum.h的EventType对齐。
-- 因为Enum是自动从0开始逐个加一的，所以这里先利用Lua的数组隐式生成下标机制，和C++的Enum对齐。
-- 但是这个是id为Key，名字为value，和C++的Enum是反的，所以后面需要转换一下
EngineEventType = {}
EngineEventType_NumToStr = 
{
    "GAME_START",
    "UPDATE_MOUSE_POS",

    "KEY_0_PRESS",
    "KEY_1_PRESS",
    "KEY_2_PRESS",
    "KEY_3_PRESS",
    "KEY_4_PRESS",
    "KEY_5_PRESS",
    "KEY_6_PRESS",
    "KEY_7_PRESS",
    "KEY_8_PRESS",
    "KEY_9_PRESS",

    "KEY_A_PRESS",
    "KEY_B_PRESS",
    "KEY_C_PRESS",
    "KEY_D_PRESS",
    "KEY_E_PRESS",
    "KEY_F_PRESS",
    "KEY_G_PRESS",
    "KEY_H_PRESS",
    "KEY_I_PRESS",
    "KEY_J_PRESS",
    "KEY_K_PRESS",
    "KEY_L_PRESS",
    "KEY_M_PRESS",
    "KEY_N_PRESS",
    "KEY_O_PRESS",
    "KEY_P_PRESS",
    "KEY_Q_PRESS",
    "KEY_R_PRESS",
    "KEY_S_PRESS",
    "KEY_T_PRESS",
    "KEY_U_PRESS",
    "KEY_V_PRESS",
    "KEY_W_PRESS",
    "KEY_X_PRESS",
    "KEY_Y_PRESS",
    "KEY_Z_PRESS",

    "KEY_SPACE_PRESS",
    "KEY_ESCAPE_PRESS",
    "KEY_RIGHT_PRESS",
    "KEY_LEFT_PRESS",
    "KEY_DOWN_PRESS",
    "KEY_UP_PRESS",
}

-- 这里再生成字符串为key，id为value的table，和C++的Enum对齐。
for i, key in ipairs(EngineEventType_NumToStr) do
    EngineEventType[key] = i
end