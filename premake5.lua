workspace "Zgine"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
project "Zgine"
	location "Zgine"
	kind "SharedLib"
	language "C++"
	
	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" ..outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/vendor/spdlog/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.26100.0"
		buildoptions "/utf-8"  -- 添加UTF-8编码支持
		
		defines
		{
			"ZG_PLATFORM_WINDOWS",
			"ZG_BUILD_DLL"
		}
	
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "ZG_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "ZG_RELEASE"
		optimize "On"
		
	filter "configurations:Dist"
		defines "ZG_DIST"
		optimize "On"
		
	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"
		
		
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir .. "/%{prj.name}")
	
	links
	{
		"Zgine"
	}
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"Zgine/vendor/spdlog/include",
		"Zgine/src"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.26100.0"
		buildoptions "/utf-8"  -- 添加UTF-8编码支持
		
		defines
		{
			"ZG_PLATFORM_WINDOWS"		
		}
		
	filter "configurations:Debug"
		defines "ZG_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "ZG_RELEASE"
		optimize "On"
		
	filter "configurations:Dist"
		defines "ZG_DIST"
		optimize "On"
		
	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"