#pragma once

# include "./Macro.h"
# include "./Type.h"
# include "./MyType/Str.h"

# if defined PLATFORM_WINDOWS

# include <windows.h>

# elif defined PLATFORM_ANDROID

# include <jni.h>

# endif

namespace $TJ::$PlatformSupport {

# if defined PLATFORM_WINDOWS

	namespace $WinReg {
		Bool existKey(HKEY const & keyType, Ch const * const & keyPath);
		Bool createKey(HKEY const & keyType, Ch const * const & keyPath);
		Bool queryKeyVal(HKEY const & keyType, Ch const * const & keyPath, Ch const * const & valName, DWORD valType, Void * const & valVal, DWORD valMaxSz);
		Bool setKeyVal(HKEY const & keyType, Ch const * const & keyPath, Ch const * const & valName, DWORD const & valType, Void const * const & valVal, DWORD const & valSz);
	}

# elif defined PLATFORM_ANDROID

	constexpr Ch kABIName[](ABI_NAME);

	inline Str getABIName() { return Str(ABI_NAME); }

	Ch * jstr2u8s(JNIEnv * jniEnv, jstring jStr);

# endif

}
