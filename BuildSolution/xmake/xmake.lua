set_xmakever("2.8.7")
add_rules("mode.release", "mode.debug")
if is_mode("debug") then
    set_targetdir("debug")
else
    set_targetdir("release")
end

includes("helper/xmake_func.lua")

target("vulkan-sdk")
set_kind("phony")
on_load(function(target)
    -- 使用工程自带的Vulkan SDK
    if is_plat("macosx") then
        target:add("linkdirs", path.join(os.projectdir(), "../../Vendor/Library/MacOS"), { public = true })
        target:add("links", "vulkan.1.3.280", "MoltenVK", { public = true })
    elseif is_plat("linux") then
        target:add("linkdirs", path.join(os.projectdir(), "../../Vendor/Library/Linux/x86_64"), { public = true })
        target:add("links", "vulkan", { public = true })
    elseif is_plat("windows") then
        target:add("linkdirs", path.join(os.projectdir(), "../../Vendor/Library/Windows/Static"), { public = true })
        target:add("links", "vulkan-1", { public = true })
    end
    target:add("includedirs", path.join(os.projectdir(), "../../Vendor/Include/Vulkan"), { public = true })

    -- 使用系统环境的Vulkan SDK，暂时弃用，因为可能会有版本不一致的问题导致编译失败
    -- local vk_path = os.getenv("VULKAN_SDK")
    -- if not vk_path then
    --     vk_path = os.getenv("VK_SDK_PATH")
    -- end
    -- if vk_path then
    --     if is_plat("linux", "macosx") then
    --         target:add("linkdirs", path.join(vk_path, "lib"), {
    --             public = true
    --         })
    --         target:add("links", "vulkan", {
    --             public = true
    --         })
    --         target:add("includedirs", path.join(vk_path, "include"), {
    --             public = true
    --         })
    --     else
    --         target:add("linkdirs", path.join(vk_path, "Lib"), {
    --             public = true
    --         })
    --         target:add("links", "vulkan-1", {
    --             public = true
    --         })
    --         target:add("includedirs", path.join(vk_path, "Include"), {
    --             public = true
    --         })
    --     end
    -- else
    --     target:add("linkdirs", path.join(os.projectdir(), "../../Vendor/Libs"), {
    --         public = true
    --     })
    --     target:add("links", "vulkan-1", {
    --         public = true
    --     })
    --     target:add("includedirs", path.join(os.projectdir(), "../../Vendor/Include/Vulkan"), {
    --         public = true
    --     })
    -- end
end)
target_end()

target("zxengine")
set_kind("binary")

add_files("../../CPPScripts/**.cpp", "../../CPPScripts/**.c", "../../Vendor/Src/**.c")
if is_plat("macosx", "linux") then
    remove_files("../../CPPScripts/DirectX12/**.cpp")
    remove_files("../../CPPScripts/Input/InputManagerWindows.cpp")
    remove_files("../../CPPScripts/Window/WindowManagerWindows.cpp")
    remove_files("../../CPPScripts/RenderAPID3D12.cpp")
    remove_files("../../CPPScripts/External/ImGui/imgui_impl_dx12.cpp")
    remove_files("../../CPPScripts/External/ImGui/imgui_impl_win32.cpp")
    remove_files("../../CPPScripts/Editor/EditorGUIManagerDirectX12.cpp")
    remove_files("../../CPPScripts/Editor/ImGuiTextureManagerD3D12.cpp")
    remove_files("../../CPPScripts/Audio/irrKlangImpl/**.cpp")
end

add_includedirs("../../Vendor/Include")
add_rules("basic_settings", "check-winsdk")
add_deps("vulkan-sdk")

if is_plat("macosx") then
    add_linkdirs(path.join(os.projectdir(), "../../Vendor/Library/MacOS"))
    add_links("assimp.5.4.0", "freetype", "glfw3", "bz2")
    add_frameworks("CoreFoundation", "CoreGraphics", "IOKit", "AppKit", "OpenGL")
    if is_mode("debug") then
        add_rpathdirs(path.join(os.projectdir(), "debug"))
    else
        add_rpathdirs(path.join(os.projectdir(), "release"))
    end
elseif is_plat("linux") then
    add_linkdirs(path.join(os.projectdir(), "../../Vendor/Library/Linux/x86_64"))
    add_links("assimp", "freetype", "glfw3", "GL", "z")
    if is_mode("debug") then
        add_rpathdirs(path.join(os.projectdir(), "debug"))
    else
        add_rpathdirs(path.join(os.projectdir(), "release"))
    end
elseif is_plat("windows") then
    add_linkdirs(path.join(os.projectdir(), "../../Vendor/Library/Windows/Static"))
    add_links("assimp-vc143-mt", "freetype", "glfw3", "irrKlang", "opengl32", "Shell32")
end

set_pcxxheader("../../CPPScripts/pubh.h")
add_defines("UNICODE")
-- copy dynamic library
after_build(function(target)
    local bin_dir = path.join(os.projectdir(), get_config("mode"))
    if is_plat("macosx") then
        local src_dir = path.join(os.projectdir(), "../../Vendor/Library/MacOS")
        os.cp(path.join(src_dir, "*.dylib"), bin_dir)
    elseif is_plat("linux") then
        local src_dir = path.join(os.projectdir(), "../../Vendor/Library/Linux/x86_64")
        os.cp(path.join(src_dir, "*.so*"), bin_dir)
    elseif is_plat("windows") then
        local src_dir = path.join(os.projectdir(), "../../Vendor/Library/Windows/Dynamic")
        os.cp(path.join(src_dir, "*.dll"), bin_dir)
    end
end)
target_end()
