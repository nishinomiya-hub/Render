#pragma once

#if defined(_WIN32) || defined(_WIN64)
#   undef  PLATFORM_WINDOWS
#   define PLATFORM_WINDOWS
#elif defined(__ANDROID__)
#	undef  PLATFORM_ANDROID
#	define PLATFORM_ANDROID 1
#elif  defined(__linux__)
#	undef  PLATFORM_LINUX
#	define PLATFORM_LINUX 1
#elif  defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#	undef  PLATFORM_IOS
#	define PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#	undef  PLATFORM_OSX
#	define PLATFORM_OSX 1
#elif defined(__EMSCRIPTEN__)
#	undef  PLATFORM_EMSCRIPTEN
#	define PLATFORM_EMSCRIPTEN 1
#endif


/*
​​Windows​​：检测 _WIN32或 _WIN64宏，定义 PLATFORM_WINDOWS（未显式赋值，等效于 #define PLATFORM_WINDOWS 1）。
​​Android​​：检测 __ANDROID__，定义 PLATFORM_ANDROID 1。
​​Linux​​：检测 __linux__（且非Android），定义 PLATFORM_LINUX 1。
​​iOS​​：检测 __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__，定义 PLATFORM_IOS 1。
​​macOS​​：检测 __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__，定义 PLATFORM_OSX 1。
​​Emscripten​​：检测 __EMSCRIPTEN__（WebAssembly），定义 PLATFORM_EMSCRIPTEN 1。
*/