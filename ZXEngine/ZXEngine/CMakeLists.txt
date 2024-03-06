set(PROJECT_NAME ZXEngine)

################################################################################
# Source groups
################################################################################
set(Animation
    "../../CPPScripts/Animation/Animation.cpp"
    "../../CPPScripts/Animation/Animation.h"
    "../../CPPScripts/Animation/AnimationController.cpp"
    "../../CPPScripts/Animation/AnimationController.h"
    "../../CPPScripts/Animation/NodeAnimation.cpp"
    "../../CPPScripts/Animation/NodeAnimation.h"
)
source_group("Animation" FILES ${Animation})

set(Audio
    "../../CPPScripts/Audio/AudioClip.cpp"
    "../../CPPScripts/Audio/AudioClip.h"
    "../../CPPScripts/Audio/AudioEngine.cpp"
    "../../CPPScripts/Audio/AudioEngine.h"
    "../../CPPScripts/Audio/AudioStream.cpp"
    "../../CPPScripts/Audio/AudioStream.h"
    "../../CPPScripts/Audio/ZAudio.h"
)
source_group("Audio" FILES ${Audio})

set(Component
    "../../CPPScripts/Component/Animator.cpp"
    "../../CPPScripts/Component/Animator.h"
    "../../CPPScripts/Component/AudioListener.cpp"
    "../../CPPScripts/Component/AudioListener.h"
    "../../CPPScripts/Component/AudioSource.cpp"
    "../../CPPScripts/Component/AudioSource.h"
    "../../CPPScripts/Component/Component.cpp"
    "../../CPPScripts/Component/Component.h"
    "../../CPPScripts/Component/GameLogic.cpp"
    "../../CPPScripts/Component/GameLogic.h"
    "../../CPPScripts/Component/Light.cpp"
    "../../CPPScripts/Component/Light.h"
    "../../CPPScripts/Component/MeshRenderer.cpp"
    "../../CPPScripts/Component/MeshRenderer.h"
    "../../CPPScripts/Component/ParticleSystem.cpp"
    "../../CPPScripts/Component/ParticleSystem.h"
    "../../CPPScripts/Component/Transform.cpp"
    "../../CPPScripts/Component/Transform.h"
    "../../CPPScripts/Component/UITextRenderer.cpp"
    "../../CPPScripts/Component/UITextRenderer.h"
    "../../CPPScripts/Component/UITextureRenderer.cpp"
    "../../CPPScripts/Component/UITextureRenderer.h"
    "../../CPPScripts/Component/ZCamera.cpp"
    "../../CPPScripts/Component/ZCamera.h"
)
source_group("Component" FILES ${Component})

set(Component__Physics
    "../../CPPScripts/Component/Physics/BoxCollider.cpp"
    "../../CPPScripts/Component/Physics/BoxCollider.h"
    "../../CPPScripts/Component/Physics/Cloth.cpp"
    "../../CPPScripts/Component/Physics/Cloth.h"
    "../../CPPScripts/Component/Physics/Collider.cpp"
    "../../CPPScripts/Component/Physics/Collider.h"
    "../../CPPScripts/Component/Physics/PlaneCollider.cpp"
    "../../CPPScripts/Component/Physics/PlaneCollider.h"
    "../../CPPScripts/Component/Physics/SphereCollider.cpp"
    "../../CPPScripts/Component/Physics/SphereCollider.h"
    "../../CPPScripts/Component/Physics/SpringJoint.cpp"
    "../../CPPScripts/Component/Physics/SpringJoint.h"
    "../../CPPScripts/Component/Physics/ZDistanceJoint.cpp"
    "../../CPPScripts/Component/Physics/ZDistanceJoint.h"
    "../../CPPScripts/Component/Physics/ZRigidBody.cpp"
    "../../CPPScripts/Component/Physics/ZRigidBody.h"
)
source_group("Component\\Physics" FILES ${Component__Physics})

set(Concurrent
    "../../CPPScripts/Concurrent/LockFreeQueue.h"
    "../../CPPScripts/Concurrent/Queue.h"
)
source_group("Concurrent" FILES ${Concurrent})

set(DirectX12
    "../../CPPScripts/DirectX12/D3D12EnumStruct.h"
    "../../CPPScripts/DirectX12/ZXD3D12DescriptorAllocator.cpp"
    "../../CPPScripts/DirectX12/ZXD3D12DescriptorAllocator.h"
    "../../CPPScripts/DirectX12/ZXD3D12DescriptorManager.cpp"
    "../../CPPScripts/DirectX12/ZXD3D12DescriptorManager.h"
    "../../CPPScripts/DirectX12/ZXD3D12Util.cpp"
    "../../CPPScripts/DirectX12/ZXD3D12Util.h"
)
source_group("DirectX12" FILES ${DirectX12})

set(Editor
    "../../CPPScripts/Editor/EditorAssetPreviewer.cpp"
    "../../CPPScripts/Editor/EditorAssetPreviewer.h"
    "../../CPPScripts/Editor/EditorConsolePanel.cpp"
    "../../CPPScripts/Editor/EditorConsolePanel.h"
    "../../CPPScripts/Editor/EditorDataManager.cpp"
    "../../CPPScripts/Editor/EditorDataManager.h"
    "../../CPPScripts/Editor/EditorDialogBoxManager.cpp"
    "../../CPPScripts/Editor/EditorDialogBoxManager.h"
    "../../CPPScripts/Editor/EditorEnumStruct.cpp"
    "../../CPPScripts/Editor/EditorEnumStruct.h"
    "../../CPPScripts/Editor/EditorGUIManager.cpp"
    "../../CPPScripts/Editor/EditorGUIManager.h"
    "../../CPPScripts/Editor/EditorGUIManagerDirectX12.cpp"
    "../../CPPScripts/Editor/EditorGUIManagerDirectX12.h"
    "../../CPPScripts/Editor/EditorGUIManagerOpenGL.cpp"
    "../../CPPScripts/Editor/EditorGUIManagerOpenGL.h"
    "../../CPPScripts/Editor/EditorGUIManagerVulkan.cpp"
    "../../CPPScripts/Editor/EditorGUIManagerVulkan.h"
    "../../CPPScripts/Editor/EditorHierarchyPanel.cpp"
    "../../CPPScripts/Editor/EditorHierarchyPanel.h"
    "../../CPPScripts/Editor/EditorInputManager.cpp"
    "../../CPPScripts/Editor/EditorInputManager.h"
    "../../CPPScripts/Editor/EditorInspectorPanel.cpp"
    "../../CPPScripts/Editor/EditorInspectorPanel.h"
    "../../CPPScripts/Editor/EditorMainBarPanel.cpp"
    "../../CPPScripts/Editor/EditorMainBarPanel.h"
    "../../CPPScripts/Editor/EditorPanel.h"
    "../../CPPScripts/Editor/EditorProjectPanel.cpp"
    "../../CPPScripts/Editor/EditorProjectPanel.h"
    "../../CPPScripts/Editor/ImGuiTextureManager.cpp"
    "../../CPPScripts/Editor/ImGuiTextureManager.h"
    "../../CPPScripts/Editor/ImGuiTextureManagerD3D12.cpp"
    "../../CPPScripts/Editor/ImGuiTextureManagerD3D12.h"
    "../../CPPScripts/Editor/ImGuiTextureManagerOpenGL.cpp"
    "../../CPPScripts/Editor/ImGuiTextureManagerOpenGL.h"
    "../../CPPScripts/Editor/ImGuiTextureManagerVulkan.cpp"
    "../../CPPScripts/Editor/ImGuiTextureManagerVulkan.h"
)
source_group("Editor" FILES ${Editor})

set(External
    "../../Vendor/Src/d3dx12_property_format_table.cpp"
    "../../Vendor/Src/glad.c"
    "../../Vendor/Src/volk.c"
)
source_group("External" FILES ${External})

set(External__ImGui
    "../../CPPScripts/External/ImGui/imconfig.h"
    "../../CPPScripts/External/ImGui/imgui.cpp"
    "../../CPPScripts/External/ImGui/imgui.h"
    "../../CPPScripts/External/ImGui/imgui_draw.cpp"
    "../../CPPScripts/External/ImGui/imgui_impl_dx12.cpp"
    "../../CPPScripts/External/ImGui/imgui_impl_dx12.h"
    "../../CPPScripts/External/ImGui/imgui_impl_glfw.cpp"
    "../../CPPScripts/External/ImGui/imgui_impl_glfw.h"
    "../../CPPScripts/External/ImGui/imgui_impl_opengl3.cpp"
    "../../CPPScripts/External/ImGui/imgui_impl_opengl3.h"
    "../../CPPScripts/External/ImGui/imgui_impl_opengl3_loader.h"
    "../../CPPScripts/External/ImGui/imgui_impl_vulkan.cpp"
    "../../CPPScripts/External/ImGui/imgui_impl_vulkan.h"
    "../../CPPScripts/External/ImGui/imgui_impl_win32.cpp"
    "../../CPPScripts/External/ImGui/imgui_impl_win32.h"
    "../../CPPScripts/External/ImGui/imgui_internal.h"
    "../../CPPScripts/External/ImGui/imgui_tables.cpp"
    "../../CPPScripts/External/ImGui/imgui_widgets.cpp"
    "../../CPPScripts/External/ImGui/imstb_rectpack.h"
    "../../CPPScripts/External/ImGui/imstb_textedit.h"
    "../../CPPScripts/External/ImGui/imstb_truetype.h"
)
source_group("External\\ImGui" FILES ${External__ImGui})

set(External__Lua
    "../../CPPScripts/External/Lua/lapi.c"
    "../../CPPScripts/External/Lua/lapi.h"
    "../../CPPScripts/External/Lua/lauxlib.c"
    "../../CPPScripts/External/Lua/lauxlib.h"
    "../../CPPScripts/External/Lua/lbaselib.c"
    "../../CPPScripts/External/Lua/lcode.c"
    "../../CPPScripts/External/Lua/lcode.h"
    "../../CPPScripts/External/Lua/lcorolib.c"
    "../../CPPScripts/External/Lua/lctype.c"
    "../../CPPScripts/External/Lua/lctype.h"
    "../../CPPScripts/External/Lua/ldblib.c"
    "../../CPPScripts/External/Lua/ldebug.c"
    "../../CPPScripts/External/Lua/ldebug.h"
    "../../CPPScripts/External/Lua/ldo.c"
    "../../CPPScripts/External/Lua/ldo.h"
    "../../CPPScripts/External/Lua/ldump.c"
    "../../CPPScripts/External/Lua/lfunc.c"
    "../../CPPScripts/External/Lua/lfunc.h"
    "../../CPPScripts/External/Lua/lgc.c"
    "../../CPPScripts/External/Lua/lgc.h"
    "../../CPPScripts/External/Lua/linit.c"
    "../../CPPScripts/External/Lua/liolib.c"
    "../../CPPScripts/External/Lua/ljumptab.h"
    "../../CPPScripts/External/Lua/llex.c"
    "../../CPPScripts/External/Lua/llex.h"
    "../../CPPScripts/External/Lua/llimits.h"
    "../../CPPScripts/External/Lua/lmathlib.c"
    "../../CPPScripts/External/Lua/lmem.c"
    "../../CPPScripts/External/Lua/lmem.h"
    "../../CPPScripts/External/Lua/loadlib.c"
    "../../CPPScripts/External/Lua/lobject.c"
    "../../CPPScripts/External/Lua/lobject.h"
    "../../CPPScripts/External/Lua/lopcodes.c"
    "../../CPPScripts/External/Lua/lopcodes.h"
    "../../CPPScripts/External/Lua/lopnames.h"
    "../../CPPScripts/External/Lua/loslib.c"
    "../../CPPScripts/External/Lua/lparser.c"
    "../../CPPScripts/External/Lua/lparser.h"
    "../../CPPScripts/External/Lua/lprefix.h"
    "../../CPPScripts/External/Lua/lstate.c"
    "../../CPPScripts/External/Lua/lstate.h"
    "../../CPPScripts/External/Lua/lstring.c"
    "../../CPPScripts/External/Lua/lstring.h"
    "../../CPPScripts/External/Lua/lstrlib.c"
    "../../CPPScripts/External/Lua/ltable.c"
    "../../CPPScripts/External/Lua/ltable.h"
    "../../CPPScripts/External/Lua/ltablib.c"
    "../../CPPScripts/External/Lua/ltm.c"
    "../../CPPScripts/External/Lua/ltm.h"
    "../../CPPScripts/External/Lua/lua.c"
    "../../CPPScripts/External/Lua/lua.h"
    "../../CPPScripts/External/Lua/lua.hpp"
    "../../CPPScripts/External/Lua/luac.c"
    "../../CPPScripts/External/Lua/luaconf.h"
    "../../CPPScripts/External/Lua/lualib.h"
    "../../CPPScripts/External/Lua/lundump.c"
    "../../CPPScripts/External/Lua/lundump.h"
    "../../CPPScripts/External/Lua/lutf8lib.c"
    "../../CPPScripts/External/Lua/lvm.c"
    "../../CPPScripts/External/Lua/lvm.h"
    "../../CPPScripts/External/Lua/lzio.c"
    "../../CPPScripts/External/Lua/lzio.h"
)
source_group("External\\Lua" FILES ${External__Lua})

set(Input
    "../../CPPScripts/Input/InputManager.cpp"
    "../../CPPScripts/Input/InputManager.h"
    "../../CPPScripts/Input/InputManagerGLFW.cpp"
    "../../CPPScripts/Input/InputManagerGLFW.h"
    "../../CPPScripts/Input/InputManagerWindows.cpp"
    "../../CPPScripts/Input/InputManagerWindows.h"
)
source_group("Input" FILES ${Input})

set(LuaWrap
    "../../CPPScripts/LuaWrap/Lua_Animator.h"
    "../../CPPScripts/LuaWrap/Lua_Debug.h"
    "../../CPPScripts/LuaWrap/Lua_GameObject.h"
    "../../CPPScripts/LuaWrap/Lua_InputManager.h"
    "../../CPPScripts/LuaWrap/Lua_Resources.h"
    "../../CPPScripts/LuaWrap/Lua_Rigidbody.h"
    "../../CPPScripts/LuaWrap/Lua_Time.h"
    "../../CPPScripts/LuaWrap/Lua_Transform.h"
    "../../CPPScripts/LuaWrap/Lua_UITextRenderer.h"
    "../../CPPScripts/LuaWrap/LuaWrap.h"
)
source_group("LuaWrap" FILES ${LuaWrap})

set(Math
    "../../CPPScripts/Math/Matrix3.cpp"
    "../../CPPScripts/Math/Matrix3.h"
    "../../CPPScripts/Math/Matrix4.cpp"
    "../../CPPScripts/Math/Matrix4.h"
    "../../CPPScripts/Math/Quaternion.cpp"
    "../../CPPScripts/Math/Quaternion.h"
    "../../CPPScripts/Math/Vector2.cpp"
    "../../CPPScripts/Math/Vector2.h"
    "../../CPPScripts/Math/Vector3.cpp"
    "../../CPPScripts/Math/Vector3.h"
    "../../CPPScripts/Math/Vector4.cpp"
    "../../CPPScripts/Math/Vector4.h"
)
source_group("Math" FILES ${Math})

set(PhysZ
    "../../CPPScripts/PhysZ/BVHNode.cpp"
    "../../CPPScripts/PhysZ/BVHNode.h"
    "../../CPPScripts/PhysZ/CollisionData.cpp"
    "../../CPPScripts/PhysZ/CollisionData.h"
    "../../CPPScripts/PhysZ/CollisionDetector.cpp"
    "../../CPPScripts/PhysZ/CollisionDetector.h"
    "../../CPPScripts/PhysZ/CollisionPrimitive.cpp"
    "../../CPPScripts/PhysZ/CollisionPrimitive.h"
    "../../CPPScripts/PhysZ/Contact.cpp"
    "../../CPPScripts/PhysZ/Contact.h"
    "../../CPPScripts/PhysZ/ContactResolver.cpp"
    "../../CPPScripts/PhysZ/ContactResolver.h"
    "../../CPPScripts/PhysZ/IntersectionDetector.cpp"
    "../../CPPScripts/PhysZ/IntersectionDetector.h"
    "../../CPPScripts/PhysZ/PhysZ.h"
    "../../CPPScripts/PhysZ/PhysZEnumStruct.h"
    "../../CPPScripts/PhysZ/PointMass.cpp"
    "../../CPPScripts/PhysZ/PointMass.h"
    "../../CPPScripts/PhysZ/PScene.cpp"
    "../../CPPScripts/PhysZ/PScene.h"
    "../../CPPScripts/PhysZ/RigidBody.cpp"
    "../../CPPScripts/PhysZ/RigidBody.h"
)
source_group("PhysZ" FILES ${PhysZ})

set(PhysZ__BoundingVolume
    "../../CPPScripts/PhysZ/BoundingVolume/BoundingSphere.cpp"
    "../../CPPScripts/PhysZ/BoundingVolume/BoundingSphere.h"
)
source_group("PhysZ\\BoundingVolume" FILES ${PhysZ__BoundingVolume})

set(PhysZ__Force
    "../../CPPScripts/PhysZ/Force/FGGravity.cpp"
    "../../CPPScripts/PhysZ/Force/FGGravity.h"
    "../../CPPScripts/PhysZ/Force/FGSpring.cpp"
    "../../CPPScripts/PhysZ/Force/FGSpring.h"
    "../../CPPScripts/PhysZ/Force/ForceGenerator.h"
)
source_group("PhysZ\\Force" FILES ${PhysZ__Force})

set(PhysZ__Joint
    "../../CPPScripts/PhysZ/Joint/DistanceJoint.cpp"
    "../../CPPScripts/PhysZ/Joint/DistanceJoint.h"
    "../../CPPScripts/PhysZ/Joint/Joint.cpp"
    "../../CPPScripts/PhysZ/Joint/Joint.h"
)
source_group("PhysZ\\Joint" FILES ${PhysZ__Joint})

set(Vulkan
    "../../CPPScripts/Vulkan/SPIRVCompiler.cpp"
    "../../CPPScripts/Vulkan/SPIRVCompiler.h"
    "../../CPPScripts/Vulkan/VulkanEnumStruct.h"
)
source_group("Vulkan" FILES ${Vulkan})

set(Window
    "../../CPPScripts/Window/WindowManager.cpp"
    "../../CPPScripts/Window/WindowManager.h"
    "../../CPPScripts/Window/WindowManagerGLFW.cpp"
    "../../CPPScripts/Window/WindowManagerGLFW.h"
    "../../CPPScripts/Window/WindowManagerWindows.cpp"
    "../../CPPScripts/Window/WindowManagerWindows.h"
)
source_group("Window" FILES ${Window})

set(
    "../../CPPScripts/CubeMap.h"
    "../../CPPScripts/Debug.h"
    "../../CPPScripts/DynamicMesh.h"
    "../../CPPScripts/EventManager.h"
    "../../CPPScripts/FBOManager.h"
    "../../CPPScripts/FrameBufferObject.h"
    "../../CPPScripts/Game.h"
    "../../CPPScripts/GameLogicManager.h"
    "../../CPPScripts/GameObject.h"
    "../../CPPScripts/GeometryGenerator.h"
    "../../CPPScripts/GlobalData.h"
    "../../CPPScripts/LuaManager.h"
    "../../CPPScripts/Material.h"
    "../../CPPScripts/MaterialData.h"
    "../../CPPScripts/Math.h"
    "../../CPPScripts/ModelUtil.h"
    "../../CPPScripts/OpenGLEnumStruct.h"
    "../../CPPScripts/ParticleSystemManager.h"
    "../../CPPScripts/ProjectSetting.h"
    "../../CPPScripts/pubh.h"
    "../../CPPScripts/PublicEnum.h"
    "../../CPPScripts/PublicStruct.h"
    "../../CPPScripts/RenderAPI.h"
    "../../CPPScripts/RenderAPID3D12.h"
    "../../CPPScripts/RenderAPIOpenGL.h"
    "../../CPPScripts/RenderAPIVulkan.h"
    "../../CPPScripts/RenderEngine.h"
    "../../CPPScripts/RenderEngineProperties.h"
    "../../CPPScripts/RenderPass.h"
    "../../CPPScripts/RenderPassAfterEffectRendering.h"
    "../../CPPScripts/RenderPassForwardRendering.h"
    "../../CPPScripts/RenderPassManager.h"
    "../../CPPScripts/RenderPassRayTracing.h"
    "../../CPPScripts/RenderPassShadowGeneration.h"
    "../../CPPScripts/RenderPassUIRendering.h"
    "../../CPPScripts/RenderQueue.h"
    "../../CPPScripts/RenderQueueManager.h"
    "../../CPPScripts/RenderStateSetting.h"
    "../../CPPScripts/Resources.h"
    "../../CPPScripts/Scene.h"
    "../../CPPScripts/SceneManager.h"
    "../../CPPScripts/ShaderParser.h"
    "../../CPPScripts/StaticMesh.h"
    "../../CPPScripts/TextCharactersManager.h"
    "../../CPPScripts/Texture.h"
    "../../CPPScripts/Time.h"
    "../../CPPScripts/Utils.h"
    "../../CPPScripts/ZMesh.h"
    "../../CPPScripts/ZShader.h"
)
source_group("头文件" FILES ${})

set(
    "../../CPPScripts/CubeMap.cpp"
    "../../CPPScripts/Debug.cpp"
    "../../CPPScripts/DynamicMesh.cpp"
    "../../CPPScripts/Entry.cpp"
    "../../CPPScripts/EventManager.cpp"
    "../../CPPScripts/FBOManager.cpp"
    "../../CPPScripts/Game.cpp"
    "../../CPPScripts/GameLogicManager.cpp"
    "../../CPPScripts/GameObject.cpp"
    "../../CPPScripts/GeometryGenerator.cpp"
    "../../CPPScripts/GlobalData.cpp"
    "../../CPPScripts/LuaManager.cpp"
    "../../CPPScripts/Material.cpp"
    "../../CPPScripts/MaterialData.cpp"
    "../../CPPScripts/Math.cpp"
    "../../CPPScripts/ModelUtil.cpp"
    "../../CPPScripts/ParticleSystemManager.cpp"
    "../../CPPScripts/ProjectSetting.cpp"
    "../../CPPScripts/PublicStruct.cpp"
    "../../CPPScripts/RenderAPI.cpp"
    "../../CPPScripts/RenderAPID3D12.cpp"
    "../../CPPScripts/RenderAPIOpenGL.cpp"
    "../../CPPScripts/RenderAPIVulkan.cpp"
    "../../CPPScripts/RenderEngine.cpp"
    "../../CPPScripts/RenderEngineProperties.cpp"
    "../../CPPScripts/RenderPassAfterEffectRendering.cpp"
    "../../CPPScripts/RenderPassForwardRendering.cpp"
    "../../CPPScripts/RenderPassManager.cpp"
    "../../CPPScripts/RenderPassRayTracing.cpp"
    "../../CPPScripts/RenderPassShadowGeneration.cpp"
    "../../CPPScripts/RenderPassUIRendering.cpp"
    "../../CPPScripts/RenderQueue.cpp"
    "../../CPPScripts/RenderQueueManager.cpp"
    "../../CPPScripts/Resources.cpp"
    "../../CPPScripts/Scene.cpp"
    "../../CPPScripts/SceneManager.cpp"
    "../../CPPScripts/ShaderParser.cpp"
    "../../CPPScripts/StaticMesh.cpp"
    "../../CPPScripts/TextCharactersManager.cpp"
    "../../CPPScripts/Texture.cpp"
    "../../CPPScripts/Time.cpp"
    "../../CPPScripts/Utils.cpp"
    "../../CPPScripts/ZMesh.cpp"
    "../../CPPScripts/ZShader.cpp"
)
source_group("源文件" FILES ${})

set(ALL_FILES
    ${Animation}
    ${Audio}
    ${Component}
    ${Component__Physics}
    ${Concurrent}
    ${DirectX12}
    ${Editor}
    ${External}
    ${External__ImGui}
    ${External__Lua}
    ${Input}
    ${LuaWrap}
    ${Math}
    ${PhysZ}
    ${PhysZ__BoundingVolume}
    ${PhysZ__Force}
    ${PhysZ__Joint}
    ${Vulkan}
    ${Window}
    ${}
    ${}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ZXEngine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Build/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Build/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "assimp-vc143-mt;"
        "glfw3;"
        "opengl32;"
        "freetype;"
        "vulkan-1;"
        "irrKlang"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "assimp-vc143-mt;"
            "opengl32;"
            "glfw3;"
            "freetype"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

