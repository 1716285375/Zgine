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
	
	
	include "Zgine/vendor/glfw"
	include "Zgine/vendor/glad"
	include "Zgine/vendor/imgui"
	
project "Zgine"
    location "Zgine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    -- 目标文件输出目录：C:\C\Zgine\bin\Debug-windows-x86_64\Zgine
    targetdir ("%{rootdir}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{rootdir}/bin-int/" .. outputdir .. "/%{prj.name}")
    
	pchheader "Zgpch.h"
	pchsource "Zgine/src/Zgpch.cpp"
	
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
        
    
    filter "configurations:Debug"
        defines "ZG_DEBUG"
        symbols "on"
        
    filter "configurations:Release"
        defines "ZG_RELEASE"
        symbols "on"
        
    filter "configurations:Dist"
        defines "ZG_DIST"
        symbols "on"
        
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
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
        "Zgine/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
    }
    
    links
    {
        "Zgine",
        "ImGui"
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
