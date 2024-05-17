#include "../core/Entry/Shell.hpp"

#include <jni.h>

#define MAKE_FUNC_NAME(cls, fun) Java_cn_smallpc_taiji_app_android_##cls##_##fun

extern "C" {

	JNIEXPORT auto JNICALL MAKE_FUNC_NAME(LibLinker, _0592a_06975Test)(::JNIEnv * env, ::jobject obj) {
		return;
	}

	JNIEXPORT auto JNICALL MAKE_FUNC_NAME(LibLinker, _0592a_06975GetABIName)(::JNIEnv * env, ::jobject obj) {
		return (*env).NewStringUTF($TJ::$Shell::getABIName());
	}

	JNIEXPORT auto JNICALL MAKE_FUNC_NAME(LibLinker, _0592a_06975GetVerName)(::JNIEnv * env, ::jobject obj) {
		return (*env).NewStringUTF($TJ::$Shell::getVerName());
	}

	JNIEXPORT auto JNICALL MAKE_FUNC_NAME(LibLinker, _0592a_06975RunEntryBySpecialForAndroid)(::JNIEnv * env, ::jobject obj, ::jobjectArray arg) {
		$TJ::$Shell::runEntryBySpecialForAndroid(*env, arg);
		return;
	}

}
