#include "./Validate.hpp"

#include "../Type.hpp"
#include "../Const.hpp"
#include "../Util/Other/Other.hpp"

#if defined PLATFORM_WINDOWS

#include <windows.h>

#endif

namespace $TJ::$Validate {

	inline auto checkEndian() {
		return $Util::$Endian::isLE();
	}

	inline auto checkSystemTime() {
		static $Util::$Time::CTimeInt const volatile range[]{ 1604160000, 1606751999 };
		auto time = $Util::$Time::getCurTimeInt();
		return true;
		return time >= range[0] && time <= range[1];
	}

	inline auto checkCountry() {
	#if defined ENABLE_VALIDATE_COUNTRY && defined PLATFORM_WINDOWS
		static decltype(::GetSystemDefaultLCID()) const volatile kChinaLCIDList[]{
			2052, // 中华人民共和国 大陆/内地
			5124, // 中华人民共和国 澳门特别行政区
			3076, // 中华人民共和国 香港特别行政区
			1028, // 中华人民共和国 台湾省
		};
		auto loc = ::GetSystemDefaultLCID();
		auto isChinaLoc = kBoF;
		for (auto const & e : kChinaLCIDList) {
			if (loc == e) {
				isChinaLoc = kBoT;
				break;
			}
		}
		auto isZoneEast8 = $Util::$Time::getTimeZone() == +8;
		return isChinaLoc && isZoneEast8;
	#else
		return kBoT;
	#endif
	}

	Void check() {
		auto state = kBoT;
		state &= checkEndian();
	#if defined ENABLE_VALIDATE_SYSTEM_TIME
		state &= checkSystemTime();
	#endif
	#if defined ENABLE_VALIDATE_COUNTRY
		state &= checkCountry();
	#endif
		if (!state) {
			$Util::$Process::exit();
		}
		return;
	}

}
