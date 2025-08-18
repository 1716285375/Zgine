-- 1. 项目定义
project "GLAD"
    kind "StaticLib"       -- 与 add_library(... STATIC)
    language "C"
    cdialect "C99"
    staticruntime "On"
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir   ("%{wks.location}/bin-int/".. outputdir .. "/%{prj.name}")

-- 2. 通用源文件
    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c",
    }
	
	includedirs
	{
		"include"
	}

-- 3. 平台分支
    filter "system:windows"
        systemversion "latest"
		buildoptions { "-std=+c11", "-lgdi32" }
		staticruntime "On"

-- 4. 通用编译选项
    filter "configurations:Debug"
        symbols "On"
		buildoptions "/MT"
        runtime "Debug"

    filter "configurations:Release"
        optimize "On"
		buildoptions "/MT"
        runtime "Release"