rule("basic_settings")
on_config(function(target)
    local _, cc = target:tool("cxx")
    if is_plat("linux") then
        -- Linux should use -stdlib=libc++
        -- https://github.com/LuisaGroup/LuisaCompute/issues/58
        if (cc == "clang" or cc == "clangxx") then
            target:add("cxflags", "-stdlib=libc++", {
                force = true
            })
            target:add("syslinks", "c++")
        end
    end
    -- disable LTO
    -- if cc == "cl" then
    --     target:add("cxflags", "-GL")
    -- elseif cc == "clang" or cc == "clangxx" then
    --     target:add("cxflags", "-flto=thin")
    -- elseif cc == "gcc" or cc == "gxx" then
    --     target:add("cxflags", "-flto")
    -- end
    -- local _, ld = target:tool("ld")
    -- if ld == "link" then
    --     target:add("ldflags", "-LTCG")
    --     target:add("shflags", "-LTCG")
    -- elseif ld == "clang" or ld == "clangxx" then
    --     target:add("ldflags", "-flto=thin")
    --     target:add("shflags", "-flto=thin")
    -- elseif ld == "gcc" or ld == "gxx" then
    --     target:add("ldflags", "-flto")
    --     target:add("shflags", "-flto")
    -- end
end)
on_load(function(target)
    local _get_or = function(name, default_value)
        local v = target:values(name)
        if v == nil then
            return default_value
        end
        return v
    end
    if not is_plat("windows") then
        if project_kind == "static" then
            target:add("cxflags", "-fPIC", {
                tools = {"clang", "gcc"}
            })
        end
    end
    -- fma support
    if is_arch("x64", "x86_64") then
        target:add("cxflags", "-mfma", {
            tools = {"clang", "gcc"}
        })
    end
    local c_standard = target:values("c_standard")
    local cxx_standard = target:values("cxx_standard")
    if type(c_standard) == "string" and type(cxx_standard) == "string" then
        target:set("languages", c_standard, cxx_standard)
    else
        target:set("languages", "clatest", "cxx20")
    end

    -- MacOS执行这个会导致exception被禁用，导致try catch编译失败
    if not is_plat("macosx") then
        local enable_exception = _get_or("enable_exception", nil)
        if enable_exception then
            target:set("exceptions", "cxx")
        else
            target:set("exceptions", "no-cxx")
        end
    end

    if is_mode("debug") then
        target:set("runtimes", "MDd")
        target:set("optimize", "none")
        target:set("warnings", "none")
        target:add("cxflags", "/GS", "/Gd", {
            tools = {"clang_cl", "cl"}
        })
    elseif is_mode("releasedbg") then
        target:set("runtimes", "MD")
        target:set("optimize", "none")
        target:set("warnings", "none")
        target:add("cxflags", "/GS-", "/Gd", {
            tools = {"clang_cl", "cl"}
        })
    else
        target:set("runtimes", "MD")
        target:set("optimize", "aggressive")
        target:set("warnings", "none")
        target:add("cxflags", "/GS-", "/Gd", {
            tools = {"clang_cl", "cl"}
        })
    end
    target:add("cxflags", "/Zc:preprocessor", {
        tools = "cl"
    });
    if _get_or("use_simd", true) then
        if is_arch("arm64") then
            target:add("vectorexts", "neon")
        else
            target:add("vectorexts", "avx", "avx2")
        end
    end
    if _get_or("no_rtti", false) then
        target:add("cxflags", "/GR-", {
            tools = {"clang_cl", "cl"}
        })
        target:add("cxflags", "-fno-rtti", "-fno-rtti-data", {
            tools = {"clang"}
        })
        target:add("cxflags", "-fno-rtti", {
            tools = {"gcc"}
        })
    end
end)
rule_end()

rule("check-winsdk")
on_config(function(target)
    if not is_plat("windows") then
        return
    end
    local toolchain = get_config("toolchain")
    if not toolchain then
        if is_plat("windows") then
            toolchain = "msvc"
        elseif is_plat("linux")  then
            toolchain = "gcc"
        else
            toolchain = "clang"
        end
    end
    local sdk_version = target:toolchain(toolchain):runenvs().WindowsSDKVersion
    
    local legal_sdk = false
    if sdk_version then
        local lib = import("lib")
        local vers = lib.string_split(sdk_version, '.')
        if #vers > 0 then
            if tonumber(vers[1]) > 10 then
                legal_sdk = true
            elseif tonumber(vers[1]) == 10 then
                if #vers > 2 then
                    if tonumber(vers[3]) >= 22000 then
                        legal_sdk = true
                    end
                end
            end
        end
    end
    if not legal_sdk then
        os.raise("Illegal windows SDK version, requires 10.0.22000.0 or later")
    end
end)
