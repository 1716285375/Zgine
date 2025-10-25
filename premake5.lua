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
	
	IncludeDir = {}
	IncludeDir["GLFW"] = "Zgine/vendor/glfw/include"
	IncludeDir["GLAD"] = "Zgine/vendor/glad/include"
	IncludeDir["ImGui"] = "Zgine/vendor/imgui"
	IncludeDir["glm"] = "Zgine/vendor/glm"
	IncludeDir["stb_image"] = "Zgine/vendor/stb_image"
	IncludeDir["EnTT"] = "Zgine/vendor/entt/src"
	IncludeDir["RapidJSON"] = "Zgine/vendor/json/rapidjson/include"
	IncludeDir["NlohmannJSON"] = "Zgine/vendor/json/nlohmann/single_include"
	IncludeDir["MiniAudio"] = "Zgine/vendor/miniaudio"
	
	
	include "Zgine/vendor/glfw"
	include "Zgine/vendor/glad"
	include "Zgine/vendor/imgui"
	include "Zgine/vendor/json/rapidjson"
	include "Zgine/vendor/miniaudio"
	
project "Zgine"
    location "Zgine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    -- 目标文件输出目录：C:\C\Zgine\bin\Debug-windows-x86_64\Zgine
    targetdir ("%{rootdir}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{rootdir}/bin-int/" .. outputdir .. "/%{prj.name}")
    
	pchheader "zgpch.h"
	pchsource "Zgine/src/zgpch.cpp"
	
	-- 编译优化设置
	flags { "MultiProcessorCompile" }  -- 启用多处理器编译
	
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }
    
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.EnTT}",
        "%{IncludeDir.RapidJSON}",
        "%{IncludeDir.NlohmannJSON}",
        "%{IncludeDir.MiniAudio}",
    }
	
	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"opengl32.lib"
	}

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions "/utf-8"
        
        defines
        {
            "ZG_PLATFORM_WINDOWS",
            "ZG_BUILD_DLL",
			"ZG_ENABLE_ASSERTS",
			"GLFW_INCLUDE_NONE",
            "_CRT_SECURE_NO_WARNINGS"
        }
        
    
    -- 编译速度优化
    flags { "MultiProcessorCompile" }
    buildoptions { "/MP", "/bigobj" }
    
    -- 预编译头文件优化
    pchheader "zgpch.h"
    pchsource "Zgine/src/zgpch.cpp"
    
    -- 链接优化
    linkoptions { "/LTCG" }  -- Link Time Code Generation
    
    filter "configurations:Debug"
        defines "ZG_DEBUG"
        symbols "On"
        optimize "Off"
        -- Debug模式下的编译优化
        buildoptions { "/MP", "/bigobj" }  -- 多处理器编译，支持大对象文件
        linkoptions { "/DEBUG" }
        
    filter "configurations:Release"
        defines "ZG_RELEASE"
        symbols "On"
        optimize "Speed"
        -- Release模式下的编译优化
        buildoptions { "/MP", "/O2", "/bigobj", "/GL" }  -- 多处理器编译，优化，大对象文件，全程序优化
        linkoptions { "/LTCG", "/OPT:REF", "/OPT:ICF" }  -- 链接时代码生成，移除未引用函数，合并相同函数
        
    filter "configurations:Dist"
        defines "ZG_DIST"
        symbols "Off"
        optimize "Speed"
        buildoptions { "/MP", "/O2", "/bigobj", "/GL" }
        linkoptions { "/LTCG", "/OPT:REF", "/OPT:ICF" }
        
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    -- Sandbox编译速度优化
    flags { "MultiProcessorCompile" }
    buildoptions { "/MP", "/bigobj" }
    
    -- Sandbox独立的预编译头文件
    pchheader "sandbox_pch.h"
    pchsource "Sandbox/src/sandbox_pch.cpp"
    
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
        "Sandbox/src",
        "Zgine/src",
        "Zgine/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.EnTT}",
        "%{IncludeDir.RapidJSON}",
        "%{IncludeDir.NlohmannJSON}",
        "%{IncludeDir.MiniAudio}"
    }
    
    links
    {
        "Zgine",
        "ImGui",
        "MiniAudio"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions "/utf-8"
  
        defines { "ZG_PLATFORM_WINDOWS" }
        
    filter "configurations:Debug"
        defines "ZG_DEBUG"
        symbols "on"
        
    filter "configurations:Release"
        defines "ZG_RELEASE"
        symbols "on"
        
    filter "configurations:Dist"
        defines "ZG_DIST"
        symbols "on"
