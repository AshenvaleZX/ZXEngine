set(PROJECT_NAME ZXEngine)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

################################################################################
# Source groups
################################################################################
set(Animation
    "../../../CPPScripts/Animation/Animation.cpp"
    "../../../CPPScripts/Animation/Animation.h"
    "../../../CPPScripts/Animation/AnimationController.cpp"
    "../../../CPPScripts/Animation/AnimationController.h"
    "../../../CPPScripts/Animation/NodeAnimation.cpp"
    "../../../CPPScripts/Animation/NodeAnimation.h"
)
source_group("Animation" FILES ${Animation})

set(Audio
    "../../../CPPScripts/Audio/AudioClip.h"
    "../../../CPPScripts/Audio/AudioEngine.cpp"
    "../../../CPPScripts/Audio/AudioEngine.h"
    "../../../CPPScripts/Audio/AudioStream.h"
    "../../../CPPScripts/Audio/ZAudio.h"
)
source_group("Audio" FILES ${Audio})

set(Audio__DefaultImpl
    "../../../CPPScripts/Audio/DefaultImpl/AudioClip_Default.h"
    "../../../CPPScripts/Audio/DefaultImpl/AudioEngine_Default.cpp"
    "../../../CPPScripts/Audio/DefaultImpl/AudioEngine_Default.h"
    "../../../CPPScripts/Audio/DefaultImpl/AudioStream_Default.h"
)
source_group("Audio\\DefaultImpl" FILES ${Audio__DefaultImpl})

set(Common
    "../../../CPPScripts/Common/SparseNaturalSet.h"
    "../../../CPPScripts/Common/TypeList.h"
)
source_group("Common" FILES ${Common})

set(Component
    "../../../CPPScripts/Component/Animator.cpp"
    "../../../CPPScripts/Component/Animator.h"
    "../../../CPPScripts/Component/AudioListener.cpp"
    "../../../CPPScripts/Component/AudioListener.h"
    "../../../CPPScripts/Component/AudioSource.cpp"
    "../../../CPPScripts/Component/AudioSource.h"
    "../../../CPPScripts/Component/Component.cpp"
    "../../../CPPScripts/Component/Component.h"
    "../../../CPPScripts/Component/GameLogic.cpp"
    "../../../CPPScripts/Component/GameLogic.h"
    "../../../CPPScripts/Component/Light.cpp"
    "../../../CPPScripts/Component/Light.h"
    "../../../CPPScripts/Component/MeshRenderer.cpp"
    "../../../CPPScripts/Component/MeshRenderer.h"
    "../../../CPPScripts/Component/ParticleSystem.cpp"
    "../../../CPPScripts/Component/ParticleSystem.h"
    "../../../CPPScripts/Component/RectTransform.cpp"
    "../../../CPPScripts/Component/RectTransform.h"
    "../../../CPPScripts/Component/Transform.cpp"
    "../../../CPPScripts/Component/Transform.h"
    "../../../CPPScripts/Component/UIButton.cpp"
    "../../../CPPScripts/Component/UIButton.h"
    "../../../CPPScripts/Component/UITextRenderer.cpp"
    "../../../CPPScripts/Component/UITextRenderer.h"
    "../../../CPPScripts/Component/UITextureRenderer.cpp"
    "../../../CPPScripts/Component/UITextureRenderer.h"
    "../../../CPPScripts/Component/ZCamera.cpp"
    "../../../CPPScripts/Component/ZCamera.h"
)
source_group("Component" FILES ${Component})

set(Component__Physics
    "../../../CPPScripts/Component/Physics/BoxCollider.cpp"
    "../../../CPPScripts/Component/Physics/BoxCollider.h"
    "../../../CPPScripts/Component/Physics/Cloth.cpp"
    "../../../CPPScripts/Component/Physics/Cloth.h"
    "../../../CPPScripts/Component/Physics/Collider.cpp"
    "../../../CPPScripts/Component/Physics/Collider.h"
    "../../../CPPScripts/Component/Physics/PlaneCollider.cpp"
    "../../../CPPScripts/Component/Physics/PlaneCollider.h"
    "../../../CPPScripts/Component/Physics/SphereCollider.cpp"
    "../../../CPPScripts/Component/Physics/SphereCollider.h"
    "../../../CPPScripts/Component/Physics/SpringJoint.cpp"
    "../../../CPPScripts/Component/Physics/SpringJoint.h"
    "../../../CPPScripts/Component/Physics/ZDistanceJoint.cpp"
    "../../../CPPScripts/Component/Physics/ZDistanceJoint.h"
    "../../../CPPScripts/Component/Physics/ZRigidBody.cpp"
    "../../../CPPScripts/Component/Physics/ZRigidBody.h"
)
source_group("Component\\Physics" FILES ${Component__Physics})

set(Concurrent
    "../../../CPPScripts/Concurrent/Job.cpp"
    "../../../CPPScripts/Concurrent/Job.h"
    "../../../CPPScripts/Concurrent/JobSystem.cpp"
    "../../../CPPScripts/Concurrent/JobSystem.h"
    "../../../CPPScripts/Concurrent/LockFreeQueue.h"
    "../../../CPPScripts/Concurrent/Queue.h"
    "../../../CPPScripts/Concurrent/ThreadSafeData.h"
)
source_group("Concurrent" FILES ${Concurrent})

set(ECS
    "../../../CPPScripts/ECS/Command.cpp"
    "../../../CPPScripts/ECS/Command.h"
    "../../../CPPScripts/ECS/ECS.h"
    "../../../CPPScripts/ECS/ECSUtils.h"
    "../../../CPPScripts/ECS/Event.h"
    "../../../CPPScripts/ECS/Queryer.h"
    "../../../CPPScripts/ECS/World.cpp"
    "../../../CPPScripts/ECS/World.h"
)
source_group("ECS" FILES ${ECS})

set(Editor
    "../../../CPPScripts/Editor/EditorAssetPreviewer.cpp"
    "../../../CPPScripts/Editor/EditorAssetPreviewer.h"
    "../../../CPPScripts/Editor/EditorAssetPreviewPanel.cpp"
    "../../../CPPScripts/Editor/EditorAssetPreviewPanel.h"
    "../../../CPPScripts/Editor/EditorConsolePanel.cpp"
    "../../../CPPScripts/Editor/EditorConsolePanel.h"
    "../../../CPPScripts/Editor/EditorDataManager.cpp"
    "../../../CPPScripts/Editor/EditorDataManager.h"
    "../../../CPPScripts/Editor/EditorDialogBoxManager.cpp"
    "../../../CPPScripts/Editor/EditorDialogBoxManager.h"
    "../../../CPPScripts/Editor/EditorEnumStruct.cpp"
    "../../../CPPScripts/Editor/EditorEnumStruct.h"
    "../../../CPPScripts/Editor/EditorGameViewPanel.cpp"
    "../../../CPPScripts/Editor/EditorGameViewPanel.h"
    "../../../CPPScripts/Editor/EditorGUIManager.cpp"
    "../../../CPPScripts/Editor/EditorGUIManager.h"
    "../../../CPPScripts/Editor/EditorGUIManagerOpenGL.cpp"
    "../../../CPPScripts/Editor/EditorGUIManagerOpenGL.h"
    "../../../CPPScripts/Editor/EditorGUIManagerVulkan.cpp"
    "../../../CPPScripts/Editor/EditorGUIManagerVulkan.h"
    "../../../CPPScripts/Editor/EditorHierarchyPanel.cpp"
    "../../../CPPScripts/Editor/EditorHierarchyPanel.h"
    "../../../CPPScripts/Editor/EditorInputManager.cpp"
    "../../../CPPScripts/Editor/EditorInputManager.h"
    "../../../CPPScripts/Editor/EditorInspectorPanel.cpp"
    "../../../CPPScripts/Editor/EditorInspectorPanel.h"
    "../../../CPPScripts/Editor/EditorMainBarPanel.cpp"
    "../../../CPPScripts/Editor/EditorMainBarPanel.h"
    "../../../CPPScripts/Editor/EditorPanel.cpp"
    "../../../CPPScripts/Editor/EditorPanel.h"
    "../../../CPPScripts/Editor/EditorProjectPanel.cpp"
    "../../../CPPScripts/Editor/EditorProjectPanel.h"
    "../../../CPPScripts/Editor/ImGuiTextureManager.cpp"
    "../../../CPPScripts/Editor/ImGuiTextureManager.h"
    "../../../CPPScripts/Editor/ImGuiTextureManagerOpenGL.cpp"
    "../../../CPPScripts/Editor/ImGuiTextureManagerOpenGL.h"
    "../../../CPPScripts/Editor/ImGuiTextureManagerVulkan.cpp"
    "../../../CPPScripts/Editor/ImGuiTextureManagerVulkan.h"
)
if(WIN32)
    list(APPEND Editor
        "../../../CPPScripts/Editor/EditorGUIManagerDirectX12.cpp"
        "../../../CPPScripts/Editor/EditorGUIManagerDirectX12.h"
        "../../../CPPScripts/Editor/ImGuiTextureManagerD3D12.cpp"
        "../../../CPPScripts/Editor/ImGuiTextureManagerD3D12.h"
    )
endif()
source_group("Editor" FILES ${Editor})

set(External
    "../../../Vendor/Src/glad.c"
    "../../../Vendor/Src/volk.c"
)
if(WIN32)
    list(APPEND External
        "../../../Vendor/Src/d3dx12_property_format_table.cpp"
    )
endif()
source_group("External" FILES ${External})

set(External__ImGui
    "../../../CPPScripts/External/ImGui/imconfig.h"
    "../../../CPPScripts/External/ImGui/imgui.cpp"
    "../../../CPPScripts/External/ImGui/imgui.h"
    "../../../CPPScripts/External/ImGui/imgui_draw.cpp"
    "../../../CPPScripts/External/ImGui/imgui_impl_glfw.cpp"
    "../../../CPPScripts/External/ImGui/imgui_impl_glfw.h"
    "../../../CPPScripts/External/ImGui/imgui_impl_opengl3.cpp"
    "../../../CPPScripts/External/ImGui/imgui_impl_opengl3.h"
    "../../../CPPScripts/External/ImGui/imgui_impl_opengl3_loader.h"
    "../../../CPPScripts/External/ImGui/imgui_impl_vulkan.cpp"
    "../../../CPPScripts/External/ImGui/imgui_impl_vulkan.h"
    "../../../CPPScripts/External/ImGui/imgui_internal.h"
    "../../../CPPScripts/External/ImGui/imgui_tables.cpp"
    "../../../CPPScripts/External/ImGui/imgui_widgets.cpp"
    "../../../CPPScripts/External/ImGui/imstb_rectpack.h"
    "../../../CPPScripts/External/ImGui/imstb_textedit.h"
    "../../../CPPScripts/External/ImGui/imstb_truetype.h"
)
if(WIN32)
    list(APPEND External__ImGui
        "../../../CPPScripts/External/ImGui/imgui_impl_dx12.cpp"
        "../../../CPPScripts/External/ImGui/imgui_impl_dx12.h"
        "../../../CPPScripts/External/ImGui/imgui_impl_win32.cpp"
        "../../../CPPScripts/External/ImGui/imgui_impl_win32.h"
    )
endif()
source_group("External\\ImGui" FILES ${External__ImGui})

set(External__Lua
    "../../../CPPScripts/External/Lua/lapi.c"
    "../../../CPPScripts/External/Lua/lapi.h"
    "../../../CPPScripts/External/Lua/lauxlib.c"
    "../../../CPPScripts/External/Lua/lauxlib.h"
    "../../../CPPScripts/External/Lua/lbaselib.c"
    "../../../CPPScripts/External/Lua/lcode.c"
    "../../../CPPScripts/External/Lua/lcode.h"
    "../../../CPPScripts/External/Lua/lcorolib.c"
    "../../../CPPScripts/External/Lua/lctype.c"
    "../../../CPPScripts/External/Lua/lctype.h"
    "../../../CPPScripts/External/Lua/ldblib.c"
    "../../../CPPScripts/External/Lua/ldebug.c"
    "../../../CPPScripts/External/Lua/ldebug.h"
    "../../../CPPScripts/External/Lua/ldo.c"
    "../../../CPPScripts/External/Lua/ldo.h"
    "../../../CPPScripts/External/Lua/ldump.c"
    "../../../CPPScripts/External/Lua/lfunc.c"
    "../../../CPPScripts/External/Lua/lfunc.h"
    "../../../CPPScripts/External/Lua/lgc.c"
    "../../../CPPScripts/External/Lua/lgc.h"
    "../../../CPPScripts/External/Lua/linit.c"
    "../../../CPPScripts/External/Lua/liolib.c"
    "../../../CPPScripts/External/Lua/ljumptab.h"
    "../../../CPPScripts/External/Lua/llex.c"
    "../../../CPPScripts/External/Lua/llex.h"
    "../../../CPPScripts/External/Lua/llimits.h"
    "../../../CPPScripts/External/Lua/lmathlib.c"
    "../../../CPPScripts/External/Lua/lmem.c"
    "../../../CPPScripts/External/Lua/lmem.h"
    "../../../CPPScripts/External/Lua/loadlib.c"
    "../../../CPPScripts/External/Lua/lobject.c"
    "../../../CPPScripts/External/Lua/lobject.h"
    "../../../CPPScripts/External/Lua/lopcodes.c"
    "../../../CPPScripts/External/Lua/lopcodes.h"
    "../../../CPPScripts/External/Lua/lopnames.h"
    "../../../CPPScripts/External/Lua/loslib.c"
    "../../../CPPScripts/External/Lua/lparser.c"
    "../../../CPPScripts/External/Lua/lparser.h"
    "../../../CPPScripts/External/Lua/lprefix.h"
    "../../../CPPScripts/External/Lua/lstate.c"
    "../../../CPPScripts/External/Lua/lstate.h"
    "../../../CPPScripts/External/Lua/lstring.c"
    "../../../CPPScripts/External/Lua/lstring.h"
    "../../../CPPScripts/External/Lua/lstrlib.c"
    "../../../CPPScripts/External/Lua/ltable.c"
    "../../../CPPScripts/External/Lua/ltable.h"
    "../../../CPPScripts/External/Lua/ltablib.c"
    "../../../CPPScripts/External/Lua/ltm.c"
    "../../../CPPScripts/External/Lua/ltm.h"
    "../../../CPPScripts/External/Lua/lua.c"
    "../../../CPPScripts/External/Lua/lua.h"
    "../../../CPPScripts/External/Lua/lua.hpp"
    "../../../CPPScripts/External/Lua/luac.c"
    "../../../CPPScripts/External/Lua/luaconf.h"
    "../../../CPPScripts/External/Lua/lualib.h"
    "../../../CPPScripts/External/Lua/lundump.c"
    "../../../CPPScripts/External/Lua/lundump.h"
    "../../../CPPScripts/External/Lua/lutf8lib.c"
    "../../../CPPScripts/External/Lua/lvm.c"
    "../../../CPPScripts/External/Lua/lvm.h"
    "../../../CPPScripts/External/Lua/lzio.c"
    "../../../CPPScripts/External/Lua/lzio.h"
)
source_group("External\\Lua" FILES ${External__Lua})

set(Input
    "../../../CPPScripts/Input/InputManager.cpp"
    "../../../CPPScripts/Input/InputManager.h"
    "../../../CPPScripts/Input/InputManagerGLFW.cpp"
    "../../../CPPScripts/Input/InputManagerGLFW.h"
)
if(WIN32)
    list(APPEND Input
        "../../../CPPScripts/Input/InputManagerWindows.cpp"
        "../../../CPPScripts/Input/InputManagerWindows.h"
    )
endif()
source_group("Input" FILES ${Input})

set(LuaWrap
    "../../../CPPScripts/LuaWrap/Lua_Animator.h"
    "../../../CPPScripts/LuaWrap/Lua_Camera.h"
    "../../../CPPScripts/LuaWrap/Lua_Collider.h"
    "../../../CPPScripts/LuaWrap/Lua_Debug.h"
    "../../../CPPScripts/LuaWrap/Lua_GameLogic.h"
    "../../../CPPScripts/LuaWrap/Lua_GameObject.h"
    "../../../CPPScripts/LuaWrap/Lua_GlobalData.h"
    "../../../CPPScripts/LuaWrap/Lua_InputManager.h"
    "../../../CPPScripts/LuaWrap/Lua_Material.h"
    "../../../CPPScripts/LuaWrap/Lua_MeshRenderer.h"
    "../../../CPPScripts/LuaWrap/Lua_Ray.h"
    "../../../CPPScripts/LuaWrap/Lua_RectTransform.h"
    "../../../CPPScripts/LuaWrap/Lua_Resources.h"
    "../../../CPPScripts/LuaWrap/Lua_Rigidbody.h"
    "../../../CPPScripts/LuaWrap/Lua_Time.h"
    "../../../CPPScripts/LuaWrap/Lua_Transform.h"
    "../../../CPPScripts/LuaWrap/Lua_UIButton.h"
    "../../../CPPScripts/LuaWrap/Lua_UITextRenderer.h"
    "../../../CPPScripts/LuaWrap/LuaWrap.h"
)
source_group("LuaWrap" FILES ${LuaWrap})

set(Math
    "../../../CPPScripts/Math/Matrix3.cpp"
    "../../../CPPScripts/Math/Matrix3.h"
    "../../../CPPScripts/Math/Matrix4.cpp"
    "../../../CPPScripts/Math/Matrix4.h"
    "../../../CPPScripts/Math/Quaternion.cpp"
    "../../../CPPScripts/Math/Quaternion.h"
    "../../../CPPScripts/Math/Vector2.cpp"
    "../../../CPPScripts/Math/Vector2.h"
    "../../../CPPScripts/Math/Vector3.cpp"
    "../../../CPPScripts/Math/Vector3.h"
    "../../../CPPScripts/Math/Vector4.cpp"
    "../../../CPPScripts/Math/Vector4.h"
)
source_group("Math" FILES ${Math})

set(ParticleSystem
    "../../../CPPScripts/ParticleSystem/ParticleEmitter.cpp"
    "../../../CPPScripts/ParticleSystem/ParticleEmitter.h"
    "../../../CPPScripts/ParticleSystem/ParticleEvolver.cpp"
    "../../../CPPScripts/ParticleSystem/ParticleEvolver.h"
    "../../../CPPScripts/ParticleSystem/ParticleRenderer.cpp"
    "../../../CPPScripts/ParticleSystem/ParticleRenderer.h"
    "../../../CPPScripts/ParticleSystem/ParticleSystemDefinition.h"
    "../../../CPPScripts/ParticleSystem/ParticleSystemManager.cpp"
    "../../../CPPScripts/ParticleSystem/ParticleSystemManager.h"
)
source_group("ParticleSystem" FILES ${ParticleSystem})

set(PhysZ
    "../../../CPPScripts/PhysZ/BVHNode.cpp"
    "../../../CPPScripts/PhysZ/BVHNode.h"
    "../../../CPPScripts/PhysZ/CollisionData.cpp"
    "../../../CPPScripts/PhysZ/CollisionData.h"
    "../../../CPPScripts/PhysZ/CollisionDetector.cpp"
    "../../../CPPScripts/PhysZ/CollisionDetector.h"
    "../../../CPPScripts/PhysZ/CollisionPrimitive.cpp"
    "../../../CPPScripts/PhysZ/CollisionPrimitive.h"
    "../../../CPPScripts/PhysZ/Contact.cpp"
    "../../../CPPScripts/PhysZ/Contact.h"
    "../../../CPPScripts/PhysZ/ContactResolver.cpp"
    "../../../CPPScripts/PhysZ/ContactResolver.h"
    "../../../CPPScripts/PhysZ/IntersectionDetector.cpp"
    "../../../CPPScripts/PhysZ/IntersectionDetector.h"
    "../../../CPPScripts/PhysZ/PhysZ.h"
    "../../../CPPScripts/PhysZ/PhysZEnumStruct.h"
    "../../../CPPScripts/PhysZ/PointMass.cpp"
    "../../../CPPScripts/PhysZ/PointMass.h"
    "../../../CPPScripts/PhysZ/PScene.cpp"
    "../../../CPPScripts/PhysZ/PScene.h"
    "../../../CPPScripts/PhysZ/Ray.cpp"
    "../../../CPPScripts/PhysZ/Ray.h"
    "../../../CPPScripts/PhysZ/RigidBody.cpp"
    "../../../CPPScripts/PhysZ/RigidBody.h"
)
source_group("PhysZ" FILES ${PhysZ})

set(PhysZ__BoundingVolume
    "../../../CPPScripts/PhysZ/BoundingVolume/BoundingSphere.cpp"
    "../../../CPPScripts/PhysZ/BoundingVolume/BoundingSphere.h"
)
source_group("PhysZ\\BoundingVolume" FILES ${PhysZ__BoundingVolume})

set(PhysZ__Force
    "../../../CPPScripts/PhysZ/Force/FGGravity.cpp"
    "../../../CPPScripts/PhysZ/Force/FGGravity.h"
    "../../../CPPScripts/PhysZ/Force/FGSpring.cpp"
    "../../../CPPScripts/PhysZ/Force/FGSpring.h"
    "../../../CPPScripts/PhysZ/Force/ForceGenerator.h"
)
source_group("PhysZ\\Force" FILES ${PhysZ__Force})

set(PhysZ__Joint
    "../../../CPPScripts/PhysZ/Joint/DistanceJoint.cpp"
    "../../../CPPScripts/PhysZ/Joint/DistanceJoint.h"
    "../../../CPPScripts/PhysZ/Joint/Joint.cpp"
    "../../../CPPScripts/PhysZ/Joint/Joint.h"
)
source_group("PhysZ\\Joint" FILES ${PhysZ__Joint})

set(Reflection
    "../../../CPPScripts/Reflection/Argument.h"
    "../../../CPPScripts/Reflection/DynamicReflection.cpp"
    "../../../CPPScripts/Reflection/DynamicReflection.h"
    "../../../CPPScripts/Reflection/Factory.h"
    "../../../CPPScripts/Reflection/FunctionTraits.h"
    "../../../CPPScripts/Reflection/StaticReflection.h"
    "../../../CPPScripts/Reflection/TypeMap.h"
    "../../../CPPScripts/Reflection/Variable.h"
    "../../../CPPScripts/Reflection/VariableTraits.h"
)
source_group("Reflection" FILES ${Reflection})

set(Reflection__Type
    "../../../CPPScripts/Reflection/Type/TypeInfo.cpp"
    "../../../CPPScripts/Reflection/Type/TypeInfo.h"
    "../../../CPPScripts/Reflection/Type/EnumType.h"
    "../../../CPPScripts/Reflection/Type/NumericType.cpp"
    "../../../CPPScripts/Reflection/Type/NumericType.h"
    "../../../CPPScripts/Reflection/Type/VoidType.h"
)
source_group("Reflection\\Type" FILES ${Reflection__Type})

set(Test
    "../../../CPPScripts/Test/EntityComponentSystemCase.h"
    "../../../CPPScripts/Test/ReflectionAndSerializationCase.h"
)
source_group("Test" FILES ${Test})

set(Util
    "../../../CPPScripts/Util/Serialization.h"
    "../../../CPPScripts/Util/Encryption.cpp"
    "../../../CPPScripts/Util/Encryption.h"
)
source_group("Util" FILES ${Util})

set(Vulkan
    "../../../CPPScripts/Vulkan/SPIRVCompiler.cpp"
    "../../../CPPScripts/Vulkan/SPIRVCompiler.h"
    "../../../CPPScripts/Vulkan/VulkanEnumStruct.h"
)
source_group("Vulkan" FILES ${Vulkan})

set(Window
    "../../../CPPScripts/Window/WindowManager.cpp"
    "../../../CPPScripts/Window/WindowManager.h"
    "../../../CPPScripts/Window/WindowManagerGLFW.cpp"
    "../../../CPPScripts/Window/WindowManagerGLFW.h"
)
if(WIN32)
    list(APPEND Window
        "../../../CPPScripts/Window/WindowManagerWindows.cpp"
        "../../../CPPScripts/Window/WindowManagerWindows.h"
    )
endif()
source_group("Window" FILES ${Window})

set(ZXHeader
    "../../../CPPScripts/CubeMap.h"
    "../../../CPPScripts/Debug.h"
    "../../../CPPScripts/DynamicMesh.h"
    "../../../CPPScripts/EventManager.h"
    "../../../CPPScripts/FBOManager.h"
    "../../../CPPScripts/FrameBufferObject.h"
    "../../../CPPScripts/Game.h"
    "../../../CPPScripts/GameLogicManager.h"
    "../../../CPPScripts/GameObject.h"
    "../../../CPPScripts/GeometryGenerator.h"
    "../../../CPPScripts/GlobalData.h"
    "../../../CPPScripts/LuaManager.h"
    "../../../CPPScripts/Material.h"
    "../../../CPPScripts/MaterialData.h"
    "../../../CPPScripts/Math.h"
    "../../../CPPScripts/ModelUtil.h"
    "../../../CPPScripts/OpenGLEnumStruct.h"
    "../../../CPPScripts/ProjectSetting.h"
    "../../../CPPScripts/pubh.h"
    "../../../CPPScripts/PublicEnum.h"
    "../../../CPPScripts/PublicStruct.h"
    "../../../CPPScripts/RenderAPI.h"
    "../../../CPPScripts/RenderAPIOpenGL.h"
    "../../../CPPScripts/RenderAPIVulkan.h"
    "../../../CPPScripts/RenderEngine.h"
    "../../../CPPScripts/RenderEngineProperties.h"
    "../../../CPPScripts/RenderPass.h"
    "../../../CPPScripts/RenderPassAfterEffectRendering.h"
    "../../../CPPScripts/RenderPassDeferredRendering.h"
    "../../../CPPScripts/RenderPassForwardRendering.h"
    "../../../CPPScripts/RenderPassGBufferGeneration.h"
    "../../../CPPScripts/RenderPassManager.h"
    "../../../CPPScripts/RenderPassRayTracing.h"
    "../../../CPPScripts/RenderPassShadowGeneration.h"
    "../../../CPPScripts/RenderPassUIRendering.h"
    "../../../CPPScripts/RenderQueue.h"
    "../../../CPPScripts/RenderQueueManager.h"
    "../../../CPPScripts/RenderStateSetting.h"
    "../../../CPPScripts/Resources.h"
    "../../../CPPScripts/Scene.h"
    "../../../CPPScripts/SceneManager.h"
    "../../../CPPScripts/ShaderParser.h"
    "../../../CPPScripts/StaticMesh.h"
    "../../../CPPScripts/TextCharactersManager.h"
    "../../../CPPScripts/Texture.h"
    "../../../CPPScripts/Time.h"
    "../../../CPPScripts/Utils.h"
    "../../../CPPScripts/ZMesh.h"
    "../../../CPPScripts/ZShader.h"
)
if(WIN32)
    list(APPEND ZXHeader
        "../../../CPPScripts/RenderAPID3D12.h"
    )
endif()
source_group("ZXHeader" FILES ${ZXHeader})

set(ZXSource
    "../../../CPPScripts/CubeMap.cpp"
    "../../../CPPScripts/Debug.cpp"
    "../../../CPPScripts/DynamicMesh.cpp"
    "../../../CPPScripts/Entry.cpp"
    "../../../CPPScripts/EventManager.cpp"
    "../../../CPPScripts/FBOManager.cpp"
    "../../../CPPScripts/Game.cpp"
    "../../../CPPScripts/GameLogicManager.cpp"
    "../../../CPPScripts/GameObject.cpp"
    "../../../CPPScripts/GeometryGenerator.cpp"
    "../../../CPPScripts/GlobalData.cpp"
    "../../../CPPScripts/LuaManager.cpp"
    "../../../CPPScripts/Material.cpp"
    "../../../CPPScripts/MaterialData.cpp"
    "../../../CPPScripts/Math.cpp"
    "../../../CPPScripts/ModelUtil.cpp"
    "../../../CPPScripts/ProjectSetting.cpp"
    "../../../CPPScripts/PublicStruct.cpp"
    "../../../CPPScripts/RenderAPI.cpp"
    "../../../CPPScripts/RenderAPIOpenGL.cpp"
    "../../../CPPScripts/RenderAPIVulkan.cpp"
    "../../../CPPScripts/RenderEngine.cpp"
    "../../../CPPScripts/RenderEngineProperties.cpp"
    "../../../CPPScripts/RenderPassAfterEffectRendering.cpp"
    "../../../CPPScripts/RenderPassDeferredRendering.cpp"
    "../../../CPPScripts/RenderPassForwardRendering.cpp"
    "../../../CPPScripts/RenderPassGBufferGeneration.cpp"
    "../../../CPPScripts/RenderPassManager.cpp"
    "../../../CPPScripts/RenderPassRayTracing.cpp"
    "../../../CPPScripts/RenderPassShadowGeneration.cpp"
    "../../../CPPScripts/RenderPassUIRendering.cpp"
    "../../../CPPScripts/RenderQueue.cpp"
    "../../../CPPScripts/RenderQueueManager.cpp"
    "../../../CPPScripts/Resources.cpp"
    "../../../CPPScripts/Scene.cpp"
    "../../../CPPScripts/SceneManager.cpp"
    "../../../CPPScripts/ShaderParser.cpp"
    "../../../CPPScripts/StaticMesh.cpp"
    "../../../CPPScripts/TextCharactersManager.cpp"
    "../../../CPPScripts/Texture.cpp"
    "../../../CPPScripts/Time.cpp"
    "../../../CPPScripts/Utils.cpp"
    "../../../CPPScripts/ZMesh.cpp"
    "../../../CPPScripts/ZShader.cpp"
)
if(WIN32)
    list(APPEND ZXSource
        "../../../CPPScripts/RenderAPID3D12.cpp"
    )
endif()
source_group("ZXSource" FILES ${ZXSource})

if(WIN32)
    set(Audio__irrKlangImpl
        "../../../CPPScripts/Audio/irrKlangImpl/AudioClip_irrKlang.cpp"
        "../../../CPPScripts/Audio/irrKlangImpl/AudioClip_irrKlang.h"
        "../../../CPPScripts/Audio/irrKlangImpl/AudioEngine_irrKlang.cpp"
        "../../../CPPScripts/Audio/irrKlangImpl/AudioEngine_irrKlang.h"
        "../../../CPPScripts/Audio/irrKlangImpl/AudioStream_irrKlang.cpp"
        "../../../CPPScripts/Audio/irrKlangImpl/AudioStream_irrKlang.h"
    )
    source_group("Audio\\irrKlangImpl" FILES ${Audio__irrKlangImpl})

    set(DirectX12
        "../../../CPPScripts/DirectX12/D3D12EnumStruct.h"
        "../../../CPPScripts/DirectX12/ZXD3D12DescriptorAllocator.cpp"
        "../../../CPPScripts/DirectX12/ZXD3D12DescriptorAllocator.h"
        "../../../CPPScripts/DirectX12/ZXD3D12DescriptorManager.cpp"
        "../../../CPPScripts/DirectX12/ZXD3D12DescriptorManager.h"
        "../../../CPPScripts/DirectX12/ZXD3D12Util.cpp"
        "../../../CPPScripts/DirectX12/ZXD3D12Util.h"
    )
    source_group("DirectX12" FILES ${DirectX12})
endif()

set(ALL_FILES
    ${Animation}
    ${Audio}
    ${Audio__DefaultImpl}
    ${Common}
    ${Component}
    ${Component__Physics}
    ${Concurrent}
    ${ECS}
    ${Editor}
    ${External}
    ${External__ImGui}
    ${External__Lua}
    ${Input}
    ${LuaWrap}
    ${Math}
    ${ParticleSystem}
    ${PhysZ}
    ${PhysZ__BoundingVolume}
    ${PhysZ__Force}
    ${PhysZ__Joint}
    ${Reflection}
    ${Reflection__Type}
    ${Test}
    ${Util}
    ${Vulkan}
    ${Window}
    ${ZXHeader}
    ${ZXSource}
)
if(WIN32)
    list(APPEND ALL_FILES
        ${Audio__irrKlangImpl}
        ${DirectX12}
    )
endif()

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<CONFIG>"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Compile definitions
################################################################################
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

################################################################################
# Compile and link options
################################################################################
if(MSVC)
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

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Vendor/Include/Vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Vendor/Include
)
if(WIN32)
    target_link_directories(${PROJECT_NAME} PRIVATE 
        ${PROJECT_SOURCE_DIR}/../../Vendor/Library/Windows/Static
    )
elseif(LINUX)
    target_link_directories(${PROJECT_NAME} PRIVATE 
        ${PROJECT_SOURCE_DIR}/../../Vendor/Library/Linux/x86_64
    )
endif()

################################################################################
# Post build events
################################################################################
if(WIN32)
    get_filename_component(ABSOLUTE_VENDOR_DLL_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Vendor/Library/Windows/Dynamic/*.dll" ABSOLUTE)
elseif(LINUX)
    file(GLOB ABSOLUTE_VENDOR_DLL_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Vendor/Library/Linux/x86_64/*.so*")
endif()
get_filename_component(ABSOLUTE_VENDOR_DLL_DST_PATH "${CMAKE_SOURCE_DIR}/$<CONFIG>" ABSOLUTE)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND xcopy /y /d "$<SHELL_PATH:${ABSOLUTE_VENDOR_DLL_SRC_PATH}>" "$<SHELL_PATH:${ABSOLUTE_VENDOR_DLL_DST_PATH}>"
    )
elseif(LINUX)
    foreach(SO_FILE ${ABSOLUTE_VENDOR_DLL_SRC_PATH})
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SO_FILE} ${ABSOLUTE_VENDOR_DLL_DST_PATH}
        )
    endforeach()
endif()

################################################################################
# Dependencies
################################################################################
if(WIN32)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "assimp-vc143-mt;"
        "glfw3;"
        "opengl32;"
        "freetype;"
        "vulkan-1;"
        "irrKlang"
    )
elseif(LINUX)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "assimp;"
        "glfw3;"
        "GL;"
        "freetype;"
        "vulkan;"
        "z"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
