#pragma once

#include "../Platform.hpp"

#if defined PLATFORM_ANDROID

#include <jni.h>

#endif

namespace $TJ::$Shell {

	char const * getVerName();

	void runEntry(unsigned const argCnt, char const * const * const argVal, char const * const logDir
			  #if defined PLATFORM_ANDROID
				  , char const * const storageDir
			  #endif
	);

#if defined PLATFORM_WINDOWS

	void runEntryBySpecialForWindows(unsigned const argCnt, char const * const * const argVal);

#elif defined PLATFORM_ANDROID

	char const * getABIName();

	void runEntryBySpecialForAndroid(::JNIEnv & env, ::jobjectArray const & argArr);

#endif

}
