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
    target:add("linkdirs", path.join(os.projectdir(), "../../Vendor/Libs"), {
        public = true
    })
    target:add("links", "vulkan-1", {
        public = true
    })
    target:add("includedirs", path.join(os.projectdir(), "../../Vendor/Include/Vulkan"), {
        public = true
    })
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
add_includedirs("../../Vendor/Include")
add_rules("basic_settings", "check-winsdk")
add_deps("vulkan-sdk")
add_linkdirs(path.join(os.projectdir(), "../../Vendor/Libs"))
add_links("assimp-vc143-mt", "freetype", "glfw3", "irrKlang", "opengl32", "Shell32")
set_pcxxheader("../../CPPScripts/pubh.h")
add_defines("UNICODE")
-- copy dll
after_build(function(target)
    local bin_dir = path.join(os.projectdir(), get_config("mode"))
    local src_dir = path.join(os.projectdir(), "../../Vendor/DyLibs")
    os.cp(path.join(src_dir, "*.dll"), bin_dir)
end)
target_end()
