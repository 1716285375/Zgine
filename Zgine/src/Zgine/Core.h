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

#ifdef ZG_ENABLE_ASSERTS
	#define ZG_ASSERT(x, ...) { if(!(x)) { ZG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define ZG_CORE_ASSERT(x, ...) { if (!(x)) { ZG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define ZG_ASSERT(x, ...)
	#define ZG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)