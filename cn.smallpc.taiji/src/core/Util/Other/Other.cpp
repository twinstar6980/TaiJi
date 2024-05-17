#include "./Other.hpp"

#include "../File/File.hpp"
#include "../CharSet/CharSet.hpp"
#include "../../PlatformSupport.hpp"

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>

#if defined PLATFORM_WINDOWS

#include <windows.h>

#elif defined PLATFORM_LINUX

#include <unistd.h>

#endif

namespace $TJ::$Util {

	namespace $Process {

		Void delay(Sz const & sec) {
		#if defined PLATFORM_WINDOWS
			::Sleep(sec * 1000);
		#elif defined PLATFORM_LINUX
			::sleep(sec);
		#endif
			return;
		}
		Void pause() {
			return $PlatformSupport::pause();
		}
		Void exit() {
			return $PlatformSupport::exit();
		}
		Void exitWithPause(ConstCStr const & msg) {
			if (msg != kNullPtr)
				log.txtln("%s", msg);
			pause();
			exit();
			return;
		}

	}

	namespace $Time {

		CTimeStruct makeTimeStruct(Sz const & year, Sz const & month, Sz const & day,
								   Sz const & hour, Sz const & min, Sz const & sec) {
			CTimeStruct time;
			time.tm_year = year - 1900;
			time.tm_mon = month - 1;
			time.tm_mday = day;
			time.tm_hour = hour;
			time.tm_min = min;
			time.tm_sec = sec;
			return time;
		}
		CTimeStruct makeTimeStruct(CTimeInt const & time) {
			return *std::localtime(&time);
		}

		CTimeInt makeTimeInt(CTimeStruct const & time) {
			return std::mktime(const_cast<CTimeStruct *>(&time));
		}

		Str makeTimeStr(CTimeStruct const & time, Str const & fmt) {
			Str r;
			r.alloc(64);
			std::strftime(r.$data, r.$cap, fmt.$data, &time);
			r.$sz = getCStrLen(r.$data);
			return r;
		}

		SSz getTimeZone() {
			auto time = std::localtime(&crv(getCurTimeInt()));
			auto s = makeTimeStr(*time, "%z"_r);
			Ch sgn; SSz zone;
			$Str::scanFmt(s, "%c%2d", &sgn, &zone);
			if (sgn == '+')
				zone = zone;
			else if (sgn == '-')
				zone = -zone;
			else
				zone = 0;
			return zone;
		}

		CTimeStruct getCompileTimeStruct(ConstCStr const & date, ConstCStr const & time) {
			Ch monthName[4] = { '\0' };
			Sz year, month, day, hour, min, sec;
			std::sscanf(date, "%s %u %u", monthName, &day, &year);
			std::sscanf(time, "%u:%u:%u", &hour, &min, &sec);
			month = kNo1;
			for (auto const & e : { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" }) {
				if (std::strncmp(monthName, e, 3) == 0)
					break;
				++month;
			}
			++month;
			return makeTimeStruct(year, month, day, hour, min, sec);
		}
		CTimeInt getCompileTimeInt(ConstCStr const & date, ConstCStr const & time) {
			return makeTimeInt(getCompileTimeStruct(date, time));
		}
		Str getCompileTimeStr(ConstCStr const & date, ConstCStr const & time) {
			return makeTimeStr(getCompileTimeStruct(date, time), kWholeTimeFmt);
		}

	}

	IU32 randomNum32() {
		std::mt19937_64 gen(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<IU32> dis(0u, 0xFFFFFFFFu);
		return dis(gen);
	}

#if defined PLATFORM_WINDOWS

	static Path & getFilePathByDialogProto(Path & dst) {
		::OPENFILENAMEW ofn;
		::WCHAR szFile[300];
		::ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = kNullPtr;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = $Ch::kNull;
		ofn.nFilterIndex = 1;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"*\0*.*\0";
		ofn.lpstrFileTitle = kNullPtr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = L"\\";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_FORCESHOWHIDDEN;
		dst.setNull();
		if (::GetOpenFileNameW(&ofn)) {
			auto tmp = $CharSet::cnvUTF16ToUTF8(ofn.lpstrFile);
			dst.parse(refCStr(crv(Str()), tmp));
			pFreeN(tmp);
		}
		return dst;
	}
	Path & getFilePathByDialog(Path & dst) {
		Path curDir; $File::getCurDir(curDir);
		getFilePathByDialogProto(dst);
		$File::chgDirThenFree(curDir);
		return dst;
	}
	Path & getFilePathByDialogWithValidate(Path & dst) {
		while (kBoT) {
			getFilePathByDialog(dst);
			if ($File::existFile(dst)) {
				break;
			} else {
				log.msg("file not exist : %s", dst.isNull() ? "" : dst.fullName().$data);
				dst.free();
			}
		}
		return dst;
	}

#endif

}
