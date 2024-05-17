#include "./PlatformSupport.hpp"

#include "./Const.hpp"
#include "./TypeUtil.hpp"
#include "./Ptr.hpp"
#include "./Util/CharSet/CharSet.hpp"

#include <cstdlib>

#if defined PLATFORM_WINDOWS

#include <windows.h>

#elif defined PLATFORM_ANDROID

#include <jni.h>
#include <stdexcept>

#endif

namespace $TJ::$PlatformSupport {

#if defined PLATFORM_WINDOWS

	namespace $Windows {

		Void pause() {
			std::system("pause");
			return;
		}
		Void exit() {
			std::exit(-1);
			return;
		}

		namespace $Reg {

			Bool existKey(::HKEY const & keyType, ConstCStr const & keyPath) {
				auto uKeyPath = $Util::$CharSet::cnvUTF8ToUTF16(keyPath);
				::HKEY hKey;
				auto openResult = ::RegOpenKeyExW(keyType, uKeyPath, 0, KEY_READ, &hKey);
				pFreeN(uKeyPath);
				if (openResult == ERROR_PATH_NOT_FOUND || openResult == ERROR_FILE_NOT_FOUND) {
					return kBoF;
				} else {
					::RegCloseKey(hKey);
					return kBoT;
				}
			}
			Bool createKey(::HKEY const & keyType, ConstCStr const & keyPath) {
				auto uKeyPath = $Util::$CharSet::cnvUTF8ToUTF16(keyPath);
				auto isOK = kBoF;
				::HKEY hKey;
				if (::RegCreateKeyW(keyType, uKeyPath, &hKey) == ERROR_SUCCESS) {
					isOK = kBoT;
					::RegCloseKey(hKey);
				}
				pFreeN(uKeyPath);
				return isOK;
			}
			Bool getKeyVal(::HKEY const & keyType, ConstCStr const & keyPath, ConstCStr const & valName, ::DWORD valType, Void * const & valVal, ::DWORD valMaxSz) {
				auto uKeyPath = $Util::$CharSet::cnvUTF8ToUTF16(keyPath);
				auto uValName = $Util::$CharSet::cnvUTF8ToUTF16(valName);
				auto isOK = kBoF;
				::HKEY hKey;
				if (::RegOpenKeyExW(keyType, uKeyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
					if (::RegQueryValueExW(hKey, uValName, 0, &valType, (LPBYTE)valVal, &valMaxSz) == ERROR_SUCCESS)
						isOK = kBoT;
					::RegCloseKey(hKey);
				}
				pFreeN(uValName);
				pFreeN(uKeyPath);
				return isOK;
			}
			Bool setKeyVal(::HKEY const & keyType, ConstCStr const & keyPath, ConstCStr const & valName, ::DWORD const & valType, Void const * const & valVal, ::DWORD const & valSz) {
				auto uKeyPath = $Util::$CharSet::cnvUTF8ToUTF16(keyPath);
				auto uValName = $Util::$CharSet::cnvUTF8ToUTF16(valName);
				auto isOK = kBoF;
				::HKEY hKey;
				if (::RegOpenKeyExW(keyType, uKeyPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
					if (::RegSetValueExW(hKey, uValName, 0, valType, (LPBYTE)valVal, valSz) == ERROR_SUCCESS)
						isOK = kBoT;
					::RegCloseKey(hKey);
				}
				pFreeN(uValName);
				pFreeN(uKeyPath);
				return isOK;
			}

		}

	}

#elif defined PLATFORM_ANDROID

	namespace $Android {

		Str jstr2u8s(::JNIEnv & env, ::jstring jstr) {
			auto cls = env.FindClass("java/lang/String");
			auto mtd = env.GetMethodID(cls, "getBytes", "(Ljava/lang/String;)[B");
			auto jByteArr = (::jbyteArray)env.CallObjectMethod(jstr, mtd, env.NewStringUTF("UTF-8"));
			auto jByteArrLen = env.GetArrayLength(jByteArr);
			auto cByteArr = env.GetByteArrayElements(jByteArr, JNI_FALSE);
			Str result;
			if (jByteArrLen > kSzN) {
				result.allocThenUse(jByteArrLen);
				std::memcpy(result.$data, cByteArr, jByteArrLen);
			}
			env.ReleaseByteArrayElements(jByteArr, cByteArr, 0);
			return result;
		}

		Void exit(::JNIEnv & env, Str const & msg) {
			auto cls = env.FindClass("java/lang/Exception");
			env.ThrowNew(cls, msg.$data);
			throw JNIExitException(msg.isNull() ? "" : msg.$data);
			return;
		}

		namespace $JavaFunc {

			namespace $CppUserInput {

				Void pause(::JNIEnv & env, Str const & msg) {
					auto cls = env.FindClass("cn/smallpc/taiji/app/android/CppUserInput");
					auto mtd = env.GetStaticMethodID(cls, "pause", "(Ljava/lang/String;)V");
					auto arg_msg = env.NewStringUTF(msg.$data);
					env.CallStaticVoidMethod(cls, mtd, arg_msg);
					return;
				}

				Bool inputBool(::JNIEnv & env, Str const & msg) {
					auto cls = env.FindClass("cn/smallpc/taiji/app/android/CppUserInput");
					auto mtd = env.GetStaticMethodID(cls, "inputBool", "(Ljava/lang/String;)Z");
					auto arg_msg = env.NewStringUTF(msg.$data);
					auto result = env.CallStaticBooleanMethod(cls, mtd, arg_msg);
					return result;
				}

				IS32 inputInt(::JNIEnv & env, Str const & msg) {
					auto cls = env.FindClass("cn/smallpc/taiji/app/android/CppUserInput");
					auto mtd = env.GetStaticMethodID(cls, "inputInt", "(Ljava/lang/String;)I");
					auto arg_msg = env.NewStringUTF(msg.$data);
					auto result = env.CallStaticIntMethod(cls, mtd, arg_msg);
					return result;
				}

				Str inputStr(::JNIEnv & env, Str const & msg) {
					auto cls = env.FindClass("cn/smallpc/taiji/app/android/CppUserInput");
					auto mtd = env.GetStaticMethodID(cls, "inputStr", "(Ljava/lang/String;)Ljava/lang/String;");
					auto arg_msg = env.NewStringUTF(msg.$data);
					auto result = jstr2u8s(env, (::jstring)env.CallStaticObjectMethod(cls, mtd, arg_msg));
					return result;
				}

				Path inputPath(::JNIEnv & env, Str const & msg) {
					auto cls = env.FindClass("cn/smallpc/taiji/app/android/CppUserInput");
					auto mtd = env.GetStaticMethodID(cls, "inputPath", "(Ljava/lang/String;)Ljava/lang/String;");
					auto arg_msg = env.NewStringUTF(msg.$data);
					auto result = jstr2u8s(env, (::jstring)env.CallStaticObjectMethod(cls, mtd, arg_msg));
					return Path(result);
				}

			}

		}

	}

#endif

	Void pause(Str const & msg) {
		if (!msg.isNull())
			log.msg("%s", msg.$data);
	#if defined PLATFORM_WINDOWS
		$Windows::pause();
	#elif defined PLATFORM_ANDROID
		$Android::$JavaFunc::$CppUserInput::pause(*gJNIEnv, msg);
	#endif
		return;
	}
	Void exit(Str const & msg) {
		if (!msg.isNull())
			log.msg("%s", msg.$data);
	#if defined PLATFORM_WINDOWS
		$Windows::exit();
	#elif defined PLATFORM_ANDROID
		$Android::exit(*gJNIEnv, msg);
	#endif
		return;
	}

}
