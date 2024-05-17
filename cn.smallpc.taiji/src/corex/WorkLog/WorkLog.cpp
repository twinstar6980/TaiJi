# include "./WorkLog.h"

#include "../Macro.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#if defined PLATFORM_ANDROID
#include <android/log.h>
#endif

namespace $TJ::$WorkLog {
	/*

	struct TJException : std::exception {
		Str _msg;

		TJException(Ch const * const & msg) {
			_msg.cpy(msg);
			return;
		}

		Ch const * what() const {
			return _msg._str;
		}
	};
*/

	static auto wPrint(char const * fmt, va_list args) {
	# if defined PLATFORM_ANDROID
		__android_log_vprint(ANDROID_LOG_DEBUG, "cn.smallpc.taiji-ndk", fmt, args);
	# else
		vfprintf(stdout, fmt, args);
	#endif
		return;
	}
	static auto wPrintLn(char const * fmt, va_list args) {
	# if defined PLATFORM_ANDROID
		__android_log_vprint(ANDROID_LOG_DEBUG, "cn.smallpc.taiji-ndk", fmt, args);
	# else
		vfprintf(stdout, fmt, args);
		printf("\n");
	#endif
		return;
}

	void wLog(char const * fmt, ...) {
		va_list args;
		va_start(args, fmt);
		wPrint(fmt, args);
		va_end(args);
		fflush(stdout);
		return;
	}
	void wLogLn(char const * fmt, ...) {
		va_list args;
		va_start(args, fmt);
		wPrintLn(fmt, args);
		va_end(args);
		fflush(stdout);
		return;
	}

	void wCmt(char const * fmt, ...) {
		va_list args;
		va_start(args, fmt);
		wPrint("# ", args);
		wPrintLn(fmt, args);
		va_end(args);
		fflush(stdout);
		return;
	}
	void wMsg(char const * fmt, ...) {
		va_list args;
		va_start(args, fmt);
		wPrint("$ ", args);
		wPrintLn(fmt, args);
		va_end(args);
		fflush(stdout);
		return;
	}
	void wWarn(char const * fmt, ...) {
		va_list args;
		va_start(args, fmt);
		wPrint("* ", args);
		wPrintLn(fmt, args);
		va_end(args);
		fflush(stdout);
		return;
	}
	void wErr(char const * fmt, ...) {
		va_list args;
		va_start(args, fmt);
		wPrint("! ", args);
		wPrintLn(fmt, args);
		va_end(args);
		system("pause");
		exit(0);
		fflush(stdout);
		return;
	}

}
