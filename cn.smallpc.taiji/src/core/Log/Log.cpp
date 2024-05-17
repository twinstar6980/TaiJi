#include "./Log.hpp"

#include "../Platform.hpp"
#include "../PlatformSupport.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#if defined PLATFORM_ANDROID

#include <android/log.h>
#include <jni.h>

extern ::JNIEnv * gJNIEnv;

#endif

namespace $TJ::$Log {

	namespace {

		inline auto pvfprintf(FILE * const & fp, char const * fmt, std::va_list const arg) {
			if (fp != nullptr) {
				std::vfprintf(fp, fmt, arg);
				std::fflush(fp);
			}
			return;
		}
		inline auto pvfprintf(void * const & fp, char const * fmt, std::va_list const arg) {
			return pvfprintf(static_cast<FILE *>(fp), fmt, arg);
		}

	#if defined PLATFORM_ANDROID
		inline auto printAndroidLog(char const * const fmt, std::va_list const arg) {
			::__android_log_vprint(::ANDROID_LOG_DEBUG, "cn.smallpc.taiji-ndk", fmt, arg);
			return;
		}
	#endif

	}

	Log log(nullptr, true);

	Log & Log::init(char const * const & path, bool const & syncShow) {
		if (path != nullptr)
			$f = std::fopen(path, "ab");
		else
			$f = nullptr;
		$syncShow = syncShow;
		return thiz;
	}
	Log & Log::free() {
		if ($f != nullptr) {
			std::fclose(static_cast<FILE *>($f));
		}
		$syncShow = false;
		return thiz;
	}

	Log const & vtxt(Log const & log, char const * const fmt, std::va_list const arg) {
		pvfprintf(log.$f, fmt, arg);
		if (log.$syncShow) {
		#if defined PLATFORM_ANDROID
			printAndroidLog(fmt, arg);
		#else
			pvfprintf(stdout, fmt, arg);
		#endif
		}
		return log;
	}
	Log const & vtxtln(Log const & log, char const * const fmt, std::va_list const arg) {
		vtxt(log, fmt, arg);
		pvfprintf(log.$f, "\n", std::va_list());
	#if ! defined PLATFORM_ANDROID
		if (log.$syncShow)
			pvfprintf(stdout, "\n", nullptr);
	#endif
		return log;
	}
	Log const & Log::txt(char const * const fmt, ...) const {
		std::va_list arg;
		va_start(arg, fmt);
		vtxt(log, fmt, arg);
		va_end(arg);
		return thiz;
	}
	Log const & Log::txtln(char const * const fmt, ...) const {
		std::va_list arg;
		va_start(arg, fmt);
		vtxtln(log, fmt, arg);
		va_end(arg);
		return thiz;
	}
	Log const & Log::cmt(char const * const fmt, ...) const {
		std::va_list arg;
		va_start(arg, fmt);
		txt("# ");
		vtxtln(log, fmt, arg);
		va_end(arg);
		return thiz;
	}
	Log const & Log::msg(char const * const fmt, ...) const {
		std::va_list arg;
		va_start(arg, fmt);
		txt("$ ");
		vtxtln(log, fmt, arg);
		va_end(arg);
		return thiz;
	}
	Log const & Log::war(char const * const fmt, ...) const {
		std::va_list arg;
		va_start(arg, fmt);
		txt("* ");
		vtxtln(log, fmt, arg);
		va_end(arg);
		return thiz;
	}
	Log const & Log::err(char const * const fmt, ...) const {
		std::va_list arg;
		va_start(arg, fmt);
		txt("! ");
		vtxtln(log, fmt, arg);
		va_end(arg);
		$PlatformSupport::pause();
		$PlatformSupport::exit(refCStr(crv(Str()), fmt));
		return thiz;
	}

}
