#pragma once


#ifdef ZG_PLATFORM_WINDOWS
	#ifdef ZG_BUILD_DLL
		#define ZG_API __declspec(dllexport)
	#else
		#define ZG_API __declspec(dllimport)
	#endif
#else
	#error Zgine only support Windows!

#endif