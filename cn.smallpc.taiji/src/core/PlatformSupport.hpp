#pragma once

#include "./Ver.hpp"
#include "./Type.hpp"
#include "./Type/Str.hpp"
#include "./Type/Path.hpp"

#include "./Util/Other/Other.hpp"

#if defined PLATFORM_WINDOWS

#include <windows.h>

#elif defined PLATFORM_ANDROID

#include <jni.h>

extern ::JNIEnv * gJNIEnv;

#endif

namespace $TJ::$PlatformSupport {

#if defined PLATFORM_WINDOWS

	namespace $Windows {

		Void pause();
		Void exit();

		namespace $Reg {

			Bool existKey(::HKEY const & keyType, ConstCStr const & keyPath);
			Bool createKey(::HKEY const & keyType, ConstCStr const & keyPath);
			Bool getKeyVal(::HKEY const & keyType, ConstCStr const & keyPath, ConstCStr const & valName, ::DWORD valType, Void * const & valVal, ::DWORD valMaxSz);
			Bool setKeyVal(::HKEY const & keyType, ConstCStr const & keyPath, ConstCStr const & valName, ::DWORD const & valType, Void const * const & valVal, ::DWORD const & valSz);

		}

	}

#elif defined PLATFORM_ANDROID

	namespace $Android {

		inline constexpr Ch kABIName[](ABI_NAME);

		inline Str getABIName() {
			return Str(ABI_NAME, getCStrLen(ABI_NAME));
		}

		Str jstr2u8s(::JNIEnv & env, ::jstring jstr);

		Void exit(::JNIEnv & env, Str const & msg);

		namespace $JavaFunc {

			namespace $CppUserInput {

				Void pause(::JNIEnv & env, Str const & msg);

				Bool inputBool(::JNIEnv & env, Str const & msg);
				IS32 inputInt(::JNIEnv & env, Str const & msg);
				Str inputStr(::JNIEnv & env, Str const & msg);
				Path inputPath(::JNIEnv & env, Str const & msg);

			}
		}

		struct JNIExitException : Exception {

			JNIExitException(ConstCStr const & msg) : Exception("JNI-Exit", msg) {}

		};

	}

#endif

	Void pause(Str const & msg = Str());
	Void exit(Str const & msg = Str());

	template <typename T>
	static auto getUserInput(Str const & msg = Str()) {
		if (!msg.isNull())
			log.txtln("> %s ?", msg.$data);
		T r;
	#if defined PLATFORM_WINDOWS
		if constexpr (kTIsBool<T>) {
			std::cin >> r;
		} else if constexpr (kTIsInt<T>) {
			std::cin >> r;
		} else if constexpr (kTIsStr<T>) {
			std::string s;
			std::cin >> s;
			r.cpy(s.c_str(), s.length());
		} else if constexpr (kTSame<T, Path>) {
			$Util::getFilePathByDialogWithValidate(r);
		} else {
			static_assert_failed("error type");
		}
	#elif defined PLATFORM_ANDROID
		if constexpr (kTIsBool<T>) {
			r = $Android::$JavaFunc::$CppUserInput::inputBool(*gJNIEnv, msg);
		} else if constexpr (kTIsInt<T>) {
			r = $Android::$JavaFunc::$CppUserInput::inputInt(*gJNIEnv, msg);
		} else if constexpr (kTIsStr<T>) {
			r = $Android::$JavaFunc::$CppUserInput::inputStr(*gJNIEnv, msg);
		} else if constexpr (kTSame<T, Path>) {
			r = $Android::$JavaFunc::$CppUserInput::inputPath(*gJNIEnv, msg);
		} else {
			static_assert_failed("error type");
		}
	#endif
		return r;
	}

}

namespace $TJ {

	using $PlatformSupport::getUserInput;

}
