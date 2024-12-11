#pragma once

namespace ZXEngine
{
	typedef uint32_t FrameBufferClearFlags;
	typedef enum FrameBufferClearFlagBit {
		ZX_CLEAR_FRAME_BUFFER_NONE_BIT    = 0x00000000,
		ZX_CLEAR_FRAME_BUFFER_COLOR_BIT   = 0x00000001,
		ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT   = 0x00000002,
		ZX_CLEAR_FRAME_BUFFER_STENCIL_BIT = 0x00000004,
	} FrameBufferClearFlagBit;

	typedef uint32_t FrameBufferPieceFlags;
	typedef enum FrameBufferPieceBit {
		ZX_FRAME_BUFFER_PIECE_COLOR    = 0x00000001,
		ZX_FRAME_BUFFER_PIECE_DEPTH    = 0x00000002,
		ZX_FRAME_BUFFER_PIECE_POSITION = 0x00000004,
		ZX_FRAME_BUFFER_PIECE_NORMAL   = 0x00000008,
	} FrameBufferPieceBit;

	typedef uint32_t ShaderStageFlags;
	typedef enum ShaderStageFlagBit {
		ZX_SHADER_STAGE_VERTEX_BIT       = 0x00000001,
		ZX_SHADER_STAGE_GEOMETRY_BIT     = 0x00000002,
		ZX_SHADER_STAGE_FRAGMENT_BIT     = 0x00000004,
		ZX_SHADER_STAGE_RAYGEN_BIT       = 0x00000008,
		ZX_SHADER_STAGE_ANY_HIT_BIT      = 0x00000010,
		ZX_SHADER_STAGE_CLOSEST_HIT_BIT  = 0x00000020,
		ZX_SHADER_STAGE_MISS_BIT         = 0x00000040,
		ZX_SHADER_STAGE_INTERSECTION_BIT = 0x00000080,
	} ShaderStageFlagBit;

	typedef uint32_t EditorPanelEdgeFlags;
	typedef enum EditorPanelEdgeFlagBit {
		ZX_EDITOR_PANEL_EDGE_NONE   = 0x00000000,
		ZX_EDITOR_PANEL_EDGE_LEFT   = 0x00000001,
		ZX_EDITOR_PANEL_EDGE_RIGHT  = 0x00000002,
		ZX_EDITOR_PANEL_EDGE_TOP    = 0x00000004,
		ZX_EDITOR_PANEL_EDGE_BOTTOM = 0x00000008,
	} EditorPanelEdgeFlagBit;

	enum class GraphicsAPI
	{
		OpenGL,
		Vulkan,
		D3D12
	};

	enum class RenderPipelineType
	{
		Forward,
		Deferred,
		RayTracing,
	};

	enum class MaterialType
	{
		Forward,
		Deferred,
		RayTracing,
	};

	enum class TextureType
	{
		ZX_2D,
		ZX_Cube,
	};

	enum class LogType
	{
		Message,
		Warning,
		Error,
	};

	enum class RenderQueueType
	{
		Deferred    = 500,
		Opaque      = 1000, 
		Transparent = 2000 
	};

	enum class LightType
	{
		None,
		Directional,
		Point,
	};

	enum class GeometryType
	{
		Box,
		Sphere,
		TessellationSphere,
		Cylinder,
		Plane,
		Quad,
		DynamicPlane,
		Cone,
	};

	enum class ShadowType
	{
		None,
		Directional,
		Point,
	};

	enum class CameraType
	{
		GameCamera,
		EditorCamera,
	};

	enum class FrameBufferType
	{
		Present, // For swap chain
		Normal,
		HigthPrecision,
		Color,
		ShadowMap,
		ShadowCubeMap,
		GBuffer,
		Deferred,
		RayTracing,
	};

	enum class CommandType
	{
		NotCare,
		ShadowGeneration,
		ForwardRendering,
		GBufferGeneration,
		DeferredRendering,
		AfterEffectRendering,
		UIRendering,
		AssetPreviewer,
		RayTracing,
	};

	enum class GameObjectLayer
	{
		Default,
		UI,
	};

	enum class ComponentType
	{
		Component,
		Transform,
		RectTransform,
		MeshRenderer,
		Camera,
		AudioListener,
		Light,
		GameLogic,
		UITextRenderer,
		UITextureRenderer,
		ParticleSystem,
		Collider,
		BoxCollider,
		PlaneCollider,
		SphereCollider,
		RigidBody,
		Animator,
		SpringJoint,
		DistanceJoint,
		Cloth,
		AudioSource,
		UIButton,
	};

	enum class FaceCullOption
	{
		None,
		Back,
		Front,
		FrontAndBack,
	};

	enum class CompareOption
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_OR_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_OR_EQUAL,
		ALWAYS,
	};

	enum class BlendOption
	{
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX,
	};

	enum class BlendFactor
	{
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA
	};

	enum class ShaderPropertyType
	{
		BOOL,
		INT,
		UINT,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		IVEC2,
		IVEC3,
		IVEC4,
		UVEC2,
		UVEC3,
		UVEC4,
		MAT2,
		MAT3,
		MAT4,

		SAMPLER,
		SAMPLER_2D,
		SAMPLER_CUBE,

		TEXTURE_INDEX,

		ENGINE_MODEL,
		ENGINE_VIEW,
		ENGINE_PROJECTION,
		ENGINE_CAMERA_POS,
		ENGINE_LIGHT_MAT,
		ENGINE_LIGHT_POS,
		ENGINE_LIGHT_DIR,
		ENGINE_LIGHT_COLOR,
		ENGINE_LIGHT_INTENSITY,
		ENGINE_DEPTH_MAP,
		ENGINE_SHADOW_MAP,
		ENGINE_SHADOW_CUBE_MAP,
		ENGINE_FAR_PLANE,
		ENGINE_MODEL_INV,
		ENGINE_VIEW_INV,
		ENGINE_PROJECTION_INV,
		ENGINE_TIME,
	};

	enum class InputButton
	{
		MOUSE_BUTTON_1,
		MOUSE_BUTTON_2,

		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,

		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,

		KEY_SPACE,
		KEY_ESCAPE,
		KEY_LSHIFT,
		KEY_RSHIFT,

		KEY_RIGHT,
		KEY_LEFT,
		KEY_DOWN,
		KEY_UP,

		END,
	};

	enum class EventType
	{
		PLACE_HOLDER = 0, // 这里加一个占位符，因为Lua也有一份EventType需要和这里对齐，但是Lua下标是从1开始的
		GAME_START,

		WINDOW_RESIZE,

		UPDATE_MOUSE_POS,
		UPDATE_MOUSE_SCROLL,

		MOUSE_BUTTON_1_PRESS,
		MOUSE_BUTTON_1_DOWN,
		MOUSE_BUTTON_1_UP,
		MOUSE_BUTTON_2_PRESS,
		MOUSE_BUTTON_2_DOWN,
		MOUSE_BUTTON_2_UP,

		KEY_0_PRESS,
		KEY_0_DOWN,
		KEY_0_UP,
		KEY_1_PRESS,
		KEY_1_DOWN,
		KEY_1_UP,
		KEY_2_PRESS,
		KEY_2_DOWN,
		KEY_2_UP,
		KEY_3_PRESS,
		KEY_3_DOWN,
		KEY_3_UP,
		KEY_4_PRESS,
		KEY_4_DOWN,
		KEY_4_UP,
		KEY_5_PRESS,
		KEY_5_DOWN,
		KEY_5_UP,
		KEY_6_PRESS,
		KEY_6_DOWN,
		KEY_6_UP,
		KEY_7_PRESS,
		KEY_7_DOWN,
		KEY_7_UP,
		KEY_8_PRESS,
		KEY_8_DOWN,
		KEY_8_UP,
		KEY_9_PRESS,
		KEY_9_DOWN,
		KEY_9_UP,

		KEY_A_PRESS,
		KEY_A_DOWN,
		KEY_A_UP,
		KEY_B_PRESS,
		KEY_B_DOWN,
		KEY_B_UP,
		KEY_C_PRESS,
		KEY_C_DOWN,
		KEY_C_UP,
		KEY_D_PRESS,
		KEY_D_DOWN,
		KEY_D_UP,
		KEY_E_PRESS,
		KEY_E_DOWN,
		KEY_E_UP,
		KEY_F_PRESS,
		KEY_F_DOWN,
		KEY_F_UP,
		KEY_G_PRESS,
		KEY_G_DOWN,
		KEY_G_UP,
		KEY_H_PRESS,
		KEY_H_DOWN,
		KEY_H_UP,
		KEY_I_PRESS,
		KEY_I_DOWN,
		KEY_I_UP,
		KEY_J_PRESS,
		KEY_J_DOWN,
		KEY_J_UP,
		KEY_K_PRESS,
		KEY_K_DOWN,
		KEY_K_UP,
		KEY_L_PRESS,
		KEY_L_DOWN,
		KEY_L_UP,
		KEY_M_PRESS,
		KEY_M_DOWN,
		KEY_M_UP,
		KEY_N_PRESS,
		KEY_N_DOWN,
		KEY_N_UP,
		KEY_O_PRESS,
		KEY_O_DOWN,
		KEY_O_UP,
		KEY_P_PRESS,
		KEY_P_DOWN,
		KEY_P_UP,
		KEY_Q_PRESS,
		KEY_Q_DOWN,
		KEY_Q_UP,
		KEY_R_PRESS,
		KEY_R_DOWN,
		KEY_R_UP,
		KEY_S_PRESS,
		KEY_S_DOWN,
		KEY_S_UP,
		KEY_T_PRESS,
		KEY_T_DOWN,
		KEY_T_UP,
		KEY_U_PRESS,
		KEY_U_DOWN,
		KEY_U_UP,
		KEY_V_PRESS,
		KEY_V_DOWN,
		KEY_V_UP,
		KEY_W_PRESS,
		KEY_W_DOWN,
		KEY_W_UP,
		KEY_X_PRESS,
		KEY_X_DOWN,
		KEY_X_UP,
		KEY_Y_PRESS,
		KEY_Y_DOWN,
		KEY_Y_UP,
		KEY_Z_PRESS,
		KEY_Z_DOWN,
		KEY_Z_UP,

		KEY_SPACE_PRESS,
		KEY_SPACE_DOWN,
		KEY_SPACE_UP,
		KEY_ESCAPE_PRESS,
		KEY_ESCAPE_DOWN,
		KEY_ESCAPE_UP,
		KEY_LSHIFT_PRESS,
		KEY_LSHIFT_DOWN,
		KEY_LSHIFT_UP,
		KEY_RSHIFT_PRESS,
		KEY_RSHIFT_DOWN,
		KEY_RSHIFT_UP,

		KEY_RIGHT_PRESS,
		KEY_RIGHT_DOWN,
		KEY_RIGHT_UP,
		KEY_LEFT_PRESS,
		KEY_LEFT_DOWN,
		KEY_LEFT_UP,
		KEY_DOWN_PRESS,
		KEY_DOWN_DOWN,
		KEY_DOWN_UP,
		KEY_UP_PRESS,
		KEY_UP_DOWN,
		KEY_UP_UP,
	};
}