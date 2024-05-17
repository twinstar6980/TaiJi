# include "./SomethingElse.h"

# include "./UtilKit/DevUtil.h"

# include <iostream>
# include <cstdlib>

# if defined PLATFORM_WINDOWS

# include <windows.h>

# elif defined PLATFORM_LINUX

# include <unistd.h>

# endif

namespace $TJ {
	Void Delay(SzI const & sec) {
	# if defined PLATFORM_WINDOWS
		Sleep(sec * 1000);
	# elif defined PLATFORM_LINUX
		sleep(sec);
	# endif
		return;
	}
	Void Pause(Ch const * const & msg) {
		if (msg != kNullPtr)
			wLogLn("%s", msg);
		system("pause");
		return;
	}
	Void Exit() {
		exit(0);
		return;
	}
	Void ExitWithPause(Ch const * const & msg) {
		if (msg != kNullPtr)
			wLogLn("%s", msg);
		wLogLn("Now Exit");
		system("pause");
		Exit();
		return;
	}

	TimeStruct MakeTimeStruct(SzI const & year, SzI const & month, SzI const & day, SzI const & hour, SzI const & min, SzI const & sec) {
		TimeStruct time;
		time.tm_year = year - 1900;
		time.tm_mon = month - 1;
		time.tm_mday = day;
		time.tm_hour = hour;
		time.tm_min = min;
		time.tm_sec = sec;
		return time;
	}
	TimeInt MakeTimeInt(TimeStruct const & time) {
		return mktime(const_cast<TimeStruct *>(&time));
	}
	TimeInt MakeTimeInt(SzI const & year, SzI const & month, SzI const & day, SzI const & hour, SzI const & min, SzI const & sec) {
		return MakeTimeInt(MakeTimeStruct(year, month, day, hour, min, sec));
	}
	Str MakeTimeStr(TimeStruct const & time) {
		static Ch buf[64] = { '\0' };
		strftime(buf, 64, "%Y-%m-%d %H:%M:%S", &time);
		return Str(buf);
	}
	SzSI GetTimeZone() {
		static Ch buf[64] = { '\0' };
		auto timeInt = getCurTimeInt();
		auto time = localtime(&timeInt);
		strftime(buf, 64, "%z", time);
		Ch ch; SzSI result;
		Str(buf).scanFmt("%c%2d", &ch, &result);
		if (ch == '+')
			result = result;
		elif(ch == '-')
			result = -result;
		else
			result = kSzSNull;
		return result;
	}

	TimeStruct GetCompileTimeStruct() {
		static SzI const kMonthSum = 12;
		static Ch const kMonthStrMap[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		Ch monthStr[4] = { '\0' };
		SzI year, month, day, hour, min, sec;
		sscanf(__DATE__, "%s %u %u", monthStr, &day, &year);
		sscanf(__TIME__, "%u:%u:%u", &hour, &min, &sec);
		for_cnt_ev(month, kThFirst, kMonthSum)
			if (strncmp(monthStr, kMonthStrMap[month], 3) == 0)
				break;
		++month;
		return MakeTimeStruct(year, month, day, hour, min, sec);
	}
	TimeInt GetCompileTimeInt() {
		return MakeTimeInt(GetCompileTimeStruct());
	}
	Str GetCompileTimeStr() {
		return MakeTimeStr(GetCompileTimeStruct());
	}

	namespace $ChSet {
	# if defined PLATFORM_WINDOWS
		Ch * WChToCh(WCh const * const & src, UINT const dstCP) {
			auto len = WideCharToMultiByte(dstCP, 0, src, -1, kNullPtr, 0, kNullPtr, kNullPtr);
			if (len <= kSzZero) {
				return kNullPtr;
			} else {
				Ch * dst(kNullPtr);
				newPtr(dst, len + kSzOne);
				WideCharToMultiByte(dstCP, 0, src, -1, dst, len, kNullPtr, kNullPtr);
				dst[len - kThSecond] = $Char::kNull;
				return dst;
			}
		}
		WCh * ChToWCh(Ch const * const & src, UINT const srcCP) {
			auto len = MultiByteToWideChar(srcCP, 0, src, -1, kNullPtr, 0);
			if (len <= kSzZero) {
				return kNullPtr;
			} else {
				WCh * dst(kNullPtr);
				newPtr(dst, len + kSzOne);
				MultiByteToWideChar(srcCP, 0, src, -1, dst, len);
				dst[len - kThSecond] = $Char::kNull;
				return dst;
			}
		}
		Ch * UTF16ToUTF8(WCh const * const & src) { return WChToCh(src, CP_UTF8); }
		WCh * UTF8ToUTF16(Ch const * const & src) { return ChToWCh(src, CP_UTF8); }

		Ch * ChCPCnv(Ch const * const & src, UINT const srcCP, UINT const dstCP) {
			auto tmp = ChToWCh(src, srcCP);
			auto dst = WChToCh(tmp, dstCP);
			delPtrArr(tmp);
			return dst;
		}
		Ch * UTF8ToANSI(Ch const * const & src) { return ChCPCnv(src, CP_UTF8, CP_ACP); }
		Ch * ANSIToUTF8(Ch const * const & src) { return ChCPCnv(src, CP_ACP, CP_UTF8); }
	# endif
	}

# if defined PLATFORM_WINDOWS
	Ch * getPathStrByWindows(Ch const * const & srcDir) {
		OPENFILENAMEW openFileName;
		WCHAR szFile[300];
		ZeroMemory(&openFileName, sizeof(openFileName));
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = kNullPtr;
		openFileName.lpstrFile = szFile;
		openFileName.lpstrFile[0] = $Char::kNull;
		openFileName.nFilterIndex = 1;
		openFileName.nMaxFile = sizeof(szFile);
		openFileName.lpstrFilter = L"*\0*.*\0";
		openFileName.lpstrFileTitle = kNullPtr;
		openFileName.nMaxFileTitle = 0;
		auto tmpInitDir = $ChSet::UTF8ToUTF16(srcDir);
		openFileName.lpstrInitialDir = tmpInitDir;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (GetOpenFileNameW(&openFileName)) {
			delPtrArr(tmpInitDir);
			return $ChSet::UTF16ToUTF8(openFileName.lpstrFile);
		} else {
			delPtrArr(tmpInitDir);
			return kNullPtr;
		}
	}
	Str & getPathStrByWindows(Str & dst, Path const & srcDir) {
		auto tmp = getPathStrByWindows((srcDir._sz == kSzZero) ? kRootPathStr._str : srcDir.getFullPath(Str().getThis())._str);
		dst.cpy(getPathStr(tmp));
		dst.eraseSub(srcDir.getFullPath(Str().getThis()).cat($PathDelimiter::kWindowsStyle));
		delPtrArr(tmp);
		return dst;
	}
	Path & getFilePathByWindow(Path & dst, Path const & srcDir) {
		Path curDir; $DevUtil::getCurWorkDir(curDir);
		dst.read(getPathStrByWindows(Str().getThis(), srcDir)._str);
		$DevUtil::chgDirThenFree(curDir);
		return dst;
	}
	Path & getFilePathByWindow(Path & dst) {
		Path curDir; $DevUtil::getCurWorkDir(curDir);
		wLogLn("1=%s, %d, %s", curDir.getFullPath()._str, curDir._sz, curDir.getFullPath(Str().getThis())._str);
		dst.read(getPathStrByWindows(Str().getThis(), curDir)._str);
		$DevUtil::chgDirThenFree(curDir);
		wLogLn("2=%s", $DevUtil::getCurWorkDir(curDir).getFullPath()._str);
		return dst;
	}
	Path & getFilePathWithValidateByWindow(Path & dst, Path const & srcDir) {
		while_nonstop{
			getFilePathByWindow(dst, srcDir);
			if ($DevUtil::existFile(Path().cpy(srcDir).add(dst))) {
				break;
			} else {
				wLogLn("File Is Not exist -> %s", Path().cpy(srcDir).add(dst).getFullPath()._str);
				dst.tfree();
			}
		}
		return dst;
	}
	Path & getFilePathWithValidateByWindow(Path & dst) {
		while_nonstop{
			getFilePathByWindow(dst);
			if ($DevUtil::existFile($DevUtil::getCurWorkDir(Path().getThis()).add(dst))) {
				break;
			} else {
				wLogLn("File Is Not exist -> %s", $DevUtil::getCurWorkDir(Path().getThis()).add(dst).getFullPath()._str);
				dst.tfree();
			}
		}
		return dst;
	}
# endif
}
