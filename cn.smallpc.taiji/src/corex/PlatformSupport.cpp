# include "./PlatformSupport.h"

# include "./Const.h"
# include "./UtilKit/PtrUtil.h"
# include "./UtilKit/DevUtil.h"
# include "./SomethingElse.h"

# if defined PLATFORM_WINDOWS

# include <windows.h>

# elif defined PLATFORM_ANDROID

# include <jni.h>

# endif

namespace $TJ::$PlatformSupport {

#if defined PLATFORM_WINDOWS

	namespace $WinReg {
		Bool existKey(HKEY const & keyType, Ch const * const & keyPath) {
			auto unicKeyPath = $ChSet::UTF8ToUTF16(keyPath);
			HKEY hKey;
			auto openResult = ::RegOpenKeyExW(keyType, unicKeyPath, 0, KEY_READ, &hKey);
			delPtrArr(unicKeyPath);
			if (openResult == ERROR_PATH_NOT_FOUND || openResult == ERROR_FILE_NOT_FOUND) {
				return kFalse;
			} else {
				::RegCloseKey(hKey);
				return kTrue;
			}
		}
		Bool createKey(HKEY const & keyType, Ch const * const & keyPath) {
			auto unicKeyPath = $ChSet::UTF8ToUTF16(keyPath);
			auto isOK = kFalse;
			HKEY hKey;
			if (ERROR_SUCCESS == ::RegCreateKeyW(keyType, unicKeyPath, &hKey)) {
				isOK = kTrue;
				::RegCloseKey(hKey);
			}
			delPtrArr(unicKeyPath);
			return isOK;
		}
		Bool queryKeyVal(HKEY const & keyType, Ch const * const & keyPath, Ch const * const & valName, DWORD valType, Void * const & valVal, DWORD valMaxSz) {
			auto unicKeyPath = $ChSet::UTF8ToUTF16(keyPath);
			auto unicValName = $ChSet::UTF8ToUTF16(valName);
			auto isOK = kFalse;
			HKEY hKey;
			if (ERROR_SUCCESS == ::RegOpenKeyExW(keyType, unicKeyPath, 0, KEY_READ, &hKey)) {
				if (ERROR_SUCCESS == ::RegQueryValueExW(hKey, unicValName, 0, &valType, (LPBYTE)valVal, &valMaxSz))
					isOK = kTrue;
				::RegCloseKey(hKey);
			}
			delPtrArr(unicValName);
			delPtrArr(unicKeyPath);
			return isOK;
		}
		Bool setKeyVal(HKEY const & keyType, Ch const * const & keyPath, Ch const * const & valName, DWORD const & valType, Void const * const & valVal, DWORD const & valSz) {
			auto unicKeyPath = $ChSet::UTF8ToUTF16(keyPath);
			auto unicValName = $ChSet::UTF8ToUTF16(valName);
			auto isOK = kFalse;
			HKEY hKey;
			if (ERROR_SUCCESS == ::RegOpenKeyExW(keyType, unicKeyPath, 0, KEY_WRITE, &hKey)) {
				if (ERROR_SUCCESS == ::RegSetValueExW(hKey, unicValName, 0, valType, (LPBYTE)valVal, valSz))
					isOK = kTrue;
				::RegCloseKey(hKey);
			}
			delPtrArr(unicValName);
			delPtrArr(unicKeyPath);
			return isOK;
		}
	}

# elif defined PLATFORM_ANDROID

	Ch * jstr2u8s(JNIEnv * jniEnv, jstring jStr) {
		Ch * result = kNullPtr;
		auto jByteArr = (jbyteArray)((*jniEnv).CallObjectMethod(
			jStr,
			(*jniEnv).GetMethodID(
				(*jniEnv).FindClass("java/lang/String"),
				"getBytes", "(Ljava/lang/String;)[B"),
			(*jniEnv).NewStringUTF("UTF-8")
		));
		auto jByteArrLen = (*jniEnv).GetArrayLength(jByteArr);
		auto cByteArr = (*jniEnv).GetByteArrayElements(jByteArr, JNI_FALSE);
		if (jByteArrLen > kSzZero) {
			newPtr(result, jByteArrLen + kSzOne);
			memCpy(result, cByteArr, jByteArrLen);
			result[jByteArrLen] = '\0';
		}
		(*jniEnv).ReleaseByteArrayElements(jByteArr, cByteArr, 0);
		return result;
	}

# endif

}
