#pragma once

#if defined _WIN32

#define PLATFORM_WINDOWS

#else

#define PLATFORM_LINUX

#define PLATFORM_ANDROID

#if defined __arm__

#define ABI_ARM
#define ABI_NAME "arm"

#elif defined __aarch64__

#define ABI_ARM64
#define ABI_NAME "arm64"

#elif defined __i386__

#define ABI_X86
#define ABI_NAME "x86"

#elif defined __x86_64__

#define ABI_X64
#define ABI_NAME "x64"

#else

#define ABI_UNKNOWN
#define ABI_NAME "unknown"

#endif

#endif
