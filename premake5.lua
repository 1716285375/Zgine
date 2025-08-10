workspace "Zgine"
    architecture "x86_64"  -- 关键：使用x86_64作为架构名，生成x86_64目录
    
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    
    -- 输出目录格式：Debug-windows-x86_64（匹配目标路径）
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- 项目根目录（根据实际情况修改，确保与你的路径一致）
    rootdir = "C:/C/Zgine"

project "Zgine"
    location "Zgine"
    kind "SharedLib"
    language "C++"
    
    -- 目标文件输出目录：C:\C\Zgine\bin\Debug-windows-x86_64\Zgine
    targetdir ("%{rootdir}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{rootdir}/bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions "/utf-8"
        
        defines
        {
            "ZG_PLATFORM_WINDOWS",
            "ZG_BUILD_DLL"
        }
        
        -- 拷贝到目标路径：C:\C\Zgine\bin\Debug-windows-x86_64\Sandbox
        postbuildcommands
        {
            -- 确保目标目录存在
            ("{MKDIR} \"%{rootdir}/bin/" .. outputdir .. "/Sandbox\""),
            -- 拷贝Zgine.dll到指定目录
            ("{COPY} \"%{cfg.buildtarget.abspath}\" \"%{rootdir}/bin/" .. outputdir .. "/Sandbox\"")
        }
    
    filter "configurations:Debug"
        defines "ZG_DEBUG"
        symbols "On"
        
    filter "configurations:Release"
        defines "ZG_RELEASE"
        symbols "On"
        
    filter "configurations:Dist"
        defines "ZG_DIST"
        symbols "On"
        
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    
    -- Sandbox输出目录：C:\C\Zgine\bin\Debug-windows-x86_64\Sandbox
    targetdir ("%{rootdir}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{rootdir}/bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "Zgine/src",
        "Zgine/vendor/spdlog/include"
    }
    
    links { "Zgine" }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions "/utf-8"
  
        defines { "ZG_PLATFORM_WINDOWS" }
        
    filter "configurations:Debug"
        defines "ZG_DEBUG"
        symbols "On"
        
    filter "configurations:Release"
        defines "ZG_RELEASE"
        symbols "On"
        
    filter "configurations:Dist"
        defines "ZG_DIST"
        symbols "On"
