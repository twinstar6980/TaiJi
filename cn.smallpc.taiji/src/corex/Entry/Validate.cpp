# include "./Validate.h"

# include "../Type.h"
# include "../TypeOfTime.h"
# include "../MyType/Str.h"
# include "../UtilKit/JSONUtil.h"
# include "../SomethingElse.h"

# if defined PLATFORM_WINDOWS

# include <windows.h>

# endif

namespace $TJ::$Validate {
# if defined ENABLE_COUNTRY_VALIDATE
	static SzI const kChinaLCIDSum(4);
	static decltype(GetSystemDefaultLCID()) const kChinaLCIDList[kChinaLCIDSum]{
		2052, // 中华人民共和国 大陆(内地)
		5124, // 中华人民共和国 澳门特别行政区
		3076, // 中华人民共和国 香港特别行政区
		1028, // 中华人民共和国 台湾省
	};

	static auto isChina() { // 本程序只支持中国地区
		auto lang = GetSystemDefaultLCID();
		auto isChinaLoc = kFalse;
		for_cnt(i, kThFirst, kChinaLCIDSum) {
			if (lang == kChinaLCIDList[i]) {
				isChinaLoc = kTrue;
				break;
			}
		}
		auto isZoneEast8 = GetTimeZone() == +8; // 东八区
		return isChinaLoc && isZoneEast8;
	}
# endif
	Void ifNotOK() {
	# if defined ENABLE_COUNTRY_VALIDATE
		if (!isChina()) {
			// do something
			//Exit();
		}
	# endif
		return;
	}
	Void ifBigEndian() {
		if (!isLE()) wErr("Your Device Is Big-Endian ! Now Exit !");
		return;
	}
}
