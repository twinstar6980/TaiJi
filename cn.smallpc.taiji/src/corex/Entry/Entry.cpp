# include "./Entry.h"

# include "../Type.h"
# include "../MyType/Path.h"
# include "../MyType/ListOfOther.h"
# include "../MyType/Map.h"
# include "../MyType/JSON.h"

# include "../UtilKit/DevUtil.h"
# include "../UtilKit/JSONUtil.h"
# include "../SomethingElse.h"

# include "../RTONUtil/Encoder.h"

# include "../Executor/Executor.h"

# include "./Validate.h"

# include "../NonStdFunc.h"
# include "../TmpCode.h"
# include "../DataUtil/Hash/Hash.h"

# include <clocale>
# include <cstdio>
# include <stdexcept>

namespace $TJ::$Entry {

	static constexpr UI8 kCoreVerCode[]CORE_VER_CODE;
	static constexpr Ch kCoreVerName[](CORE_VER_NAME);
	static constexpr Ch kCoreVerType[](CORE_VER_TYPE);

	Str getCoreInfo() {
		return Str().printFmt("#%s$%s", kCoreVerName, kCoreVerType);
	}

	static auto printCopyrightInfo() {
		wCmt("%s By Small-PC(一路上海外), China", CORE_NAME);
		wCmt("Core : %s", getCoreInfo()._str);
		wCmt("Compile Time : % s % lld", GetCompileTimeStr()._str, GetCompileTimeInt());
		wCmt("如果你使用本程序进行数据处理，当展示你的修改或发布文件到互联网上时，都应声明使用了本程序。");
		wCmt("本程序是免费、公开的。");
		wCmt("If you use this program for data processing, you should declare that you have used this program when displaying your modifications or publishing files on the Internet.");
		wCmt("This program is free of charge and public.");
		wCmt("About This Program : https://www.smallpc.cn");
		wLogLn("");
		return;
	}

	static auto runInrto() {
		$Validate::ifBigEndian();
		setlocale(LC_ALL, ".utf8");
		wMsg("locale : %s", setlocale(LC_ALL, nullptr));
		$DevUtil::chgDirToRoot();
		$JSON::$IOBuf::modBufSz($JSON::$IOBuf::kDfltBufSz);
		return;
	}
	static auto runOutro() {
		$JSON::$IOBuf::freeBuf();
		return;
	}

	static auto xtest() {
		//testx();
		//auto result = setlocale(LC_ALL, ".utf8");
		//wLogLn("%s", result);
		/*Ch buf[1000] = "121.999999999 "; SzI bufPos = 0;
		Byte val[8];
		auto type = $StrUtil::strToNum(buf, bufPos, val);
		printf("%u, %u, %.15lf\n", (UI8)type, bufPos, *(F64 *)val);
		bufPos = 50;
		$StrUtil::numToStr(*(F64 *)val, buf, bufPos);
		printf("%u , %s\n", bufPos - 50, buf + 50);
		Str s; SzI pos = 0;
		$StrUtil::readUTF8Str("text\n\xE9\xBB\x91=3 2\" ", pos, s);
		wLogLn("%u, %s", s._len, s._str);
		Str s; SzI pos = 0;
		s.cpy("黑");
		//$StrUtil::readUTF8Str("text\n\xE9\xBB\x91=3 2\" ", pos, s);
		for_criter(e, s) {
			wCmt("%.2x", (UI8)e);
		}*/
		return;
	}

	Void runEntry(StrList const & arg
	# if defined PLATFORM_WINDOWS
		, Ch const & drive
	# endif
	) {
		try {
		# if defined PLATFORM_WINDOWS
			$DevUtil::chgDrive((drive == '?') ? $DevUtil::getCurDrive() : drive);
			wCmt("drive : %c", $DevUtil::getCurDrive());
		# endif
			//xtest();
			runInrto();
		# if ! defined VER_ULTIMATE
			printCopyrightInfo();
		# endif
			if (arg._sz == kSzZero) wErr("error cmd");
			wCmt("cfg : %s", arg[kThFirst]._str);
			for_cnt(i, kThSecond, arg._sz)
				wCmt("arg-%u : %s", i - kThSecond + kThSecond, arg[i]._str);
			wLogLn("");
			$Executor::exec(arg[kThFirst], StrList(arg, kThSecond));
			wMsg("finish");
			runOutro();
		} catch (Exception e) {
			wErr("%s", e.what());
		}
		return;
	}

}
