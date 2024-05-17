#pragma once

#include "../../Type/Path.hpp"

#include <ctime>

#include <bit>

namespace $TJ::$Util {

	namespace $Endian {

		inline constexpr auto isLE() {
		#if !defined PLATFORM_ANDROID
			return std::endian::native == std::endian::little;
		#else
			return kBoT;
		#endif
			//constexpr IU16 volatile val = 0x0001;
			//return reinterpret_cast<Byte volatile const *>(&val)[0] == 0x01;
		}
		inline constexpr auto isBE() {
			return !isLE();
			//constexpr volatile IU16 val = 0x0001;
			//return reinterpret_cast<Byte volatile const *>(&val)[0] == 0x00;
		}

	}

	namespace $Process {

		Void delay(Sz const & sec);
		Void pause();
		Void exit();
		Void exitWithPause(ConstCStr const & msg = kNullPtr);

	}

	namespace $Time {

		using CTimeInt = std::time_t;
		using CTimeStruct = std::tm;
		using CClockInt = std::clock_t;

		inline auto getCurTimeInt() {
			return static_cast<CTimeInt>(time(kNullPtr));
		}
		inline auto getCurClockInt() {
			return static_cast<CClockInt>(clock());
		}

		struct Clock {

			CClockInt $beg, $end;

			auto & init() {
				$beg = $end = -1;
				return thiz;
			}
			auto & start() {
				$beg = getCurClockInt();
				return thiz;
			}
			auto & stop() {
				$end = getCurClockInt();
				return thiz;
			}
			auto durationSec() {
				return static_cast<F64>($end - $beg) / static_cast<F64>(CLOCKS_PER_SEC);
			}

			Clock() :
				$beg(), $end() {
				init();
			}

		};

		CTimeStruct makeTimeStruct(Sz const & year, Sz const & month, Sz const & day,
								   Sz const & hour, Sz const & min, Sz const & sec);
		CTimeStruct makeTimeStruct(CTimeInt const & time);

		CTimeInt makeTimeInt(CTimeStruct const & time);

		inline Str const kWholeTimeFmt("%Y-%m-%d %H:%M:%S %z"_r);

		Str makeTimeStr(CTimeStruct const & time, Str const & fmt);

		SSz getTimeZone();

		CTimeStruct getCompileTimeStruct(ConstCStr const & date, ConstCStr const & time);
		CTimeInt getCompileTimeInt(ConstCStr const & date, ConstCStr const & time);
		Str getCompileTimeStr(ConstCStr const & date, ConstCStr const & time);

	}

	IU32 randomNum32();

#if defined PLATFORM_WINDOWS

	Path & getFilePathByDialog(Path & dst);
	Path & getFilePathByDialogWithValidate(Path & dst);

#endif

}
