#include "./Entry.hpp"

#include "./Validate.hpp"
#include "../Executor/Executor.hpp"
#include "../PlatformSupport.hpp"

#include "../Type.hpp"
#include "../Type/Path.hpp"
#include "../Type/JSON.hpp"
#include "../Util/File/File.hpp"
#include "../Util/Other/Other.hpp"

#include <clocale>

#if defined USE_STD_FILESYSTEM

#include <filesystem>

namespace fs = std::filesystem;

#else

#include "../../dependency/boost/filesystem.hpp"

namespace fs = boost::filesystem;

#endif

#define def_mem_var_shell_readonly(type, name) private: (##type) $##name; public: fun_getter(name);

namespace $TJ::$Entry {

	static constexpr Ch kCoreName[](NAME);
	static constexpr IU8 kCoreVerCode[]VER_CODE;
	static constexpr Ch kCoreVerCodeStr[](VER_CODE_STR);
	static constexpr Ch kCoreVerType[](VER_TYPE);

	static Str const kVerNameFmt("#%s$%s"_r);

	Str getVerName() {
		return $Str::printFmt(kVerNameFmt.$data, kCoreVerCodeStr, kCoreVerType);
	}

	static auto showCopyrightInfo() {
		log.cmt("%s by smallpc, China", kCoreName);
		log.cmt("core : %s", getVerName().$data);
		log.cmt("compile-time : %s %llu", $Util::$Time::getCompileTimeStr(__DATE__, __TIME__).$data, $Util::$Time::getCompileTimeInt(__DATE__, __TIME__));
		log.cmt("locale : %s", std::setlocale(LC_ALL, kNullPtr));
		log.cmt("%s", u8"如果你使用本程序进行数据处理，当展示你的修改或发布文件到互联网上时，都应声明使用了本程序。");
		log.cmt("%s", u8"本程序是免费、公开的。");
		log.cmt("If you use this program for data processing, you should declare that you have used this program when displaying your modifications or publishing files on the Internet.");
		log.cmt("This program is free of charge and public.");
		log.cmt("https://smallpc.cn");
		log.txtln("");
		return;
	}

	static auto redefLog(Str const & dir) {
		fs::create_directories(dir.$data);
		using namespace $Util::$Time;
		auto path = dir + $PathDelimiter::kOnNative + makeTimeStr(makeTimeStruct(getCurTimeInt()), "[%Y-%m-%d][%H-%M-%S][%z]"_r) + ".log"_r;
		log.init(path.$data, kBoT);
		return;
	}

	Void runIntro() {
		$Validate::check();
		std::setlocale(LC_ALL, ".utf8");
		$JSON::$IOMem::realloc($JSON::$IOMem::kDfltBufSz);
		$Util::$File::chgDirToRoot();
		return;
	}
	Void runOutro() {
		$JSON::$IOMem::free();
		log.free().init(nullptr, kBoT);
		return;
	}

	Void runEntry(List<Str> const & arg, Str const & logDir
			  #if defined PLATFORM_ANDROID
				  , Str const & storageDir
			  #endif
	) {
		try {
			try {
				Path initDir;
			#if defined PLATFORM_WINDOWS
				$Util::$File::chgDrive(DriveLetter('C'));
				$Util::$File::getCurDir(initDir);
			#elif defined PLATFORM_ANDROID
				initDir.parse(storageDir);
			#endif
				runIntro();
				redefLog(logDir);
				$Util::$File::chgDir(initDir);
				log.empln();
			#if defined ENABLE_SHOW_COPYRIGHT_INFO
				showCopyrightInfo();
			#endif
				if (arg.isEmpty())
					throw VAMGException("Entry", "arg list is empty");
				log.cmt("cfg : %s", arg.first().$data);
				for (auto i = kNo2; i < arg.$sz; ++i)
					log.cmt("arg-%u : %s", i, arg[i].$data);
				log.empln();
				$Executor::exec(initDir, arg.first(), List<Str>().ref(arg, kNo2));
				log.msg("finish");
				runOutro();
			} catch (fs::filesystem_error & e) {
				log.err("std:filesystem_error : %s", e.what());
			} catch (std::exception & e) {
				log.err("std:exception : %s", e.what());
			}
		#if defined PLATFORM_ANDROID
			catch ($PlatformSupport::$Android::JNIExitException & e) {
				throw;
			}
		#endif
			catch (Exception e) {
				log.err("%s", e.what().c_str());
			} catch (...) {
				log.err("unknown-exception : %s");
			}
		}
	#if defined PLATFORM_ANDROID
		catch ($PlatformSupport::$Android::JNIExitException & e) {
			log.war("%s", e.what().c_str());
			log.war("catch the JNI exit exception, force stop executor ...");
			return;
		}
	#endif
		catch (...) {
			log.war("unknown-exception");
		}
		return;
	}

}
