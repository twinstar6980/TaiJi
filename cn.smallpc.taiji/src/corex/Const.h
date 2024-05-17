# pragma once

# include "./Type.h"
# include "./WorkLog/WorkLog.h"

namespace $TJ {
	namespace $SzDef {
		constexpr SzI kThNull(~0);
		constexpr SzI kThNone(0);

		constexpr SzI kThFirst(0);
		constexpr SzI kThSecond(1);
		constexpr SzI kThThird(2);
		constexpr SzI kThFourth(3);
		constexpr SzI kThFifth(4);
		constexpr SzI kThSixth(5);

		constexpr SzI kSzNull(~0);
		constexpr SzI kSzNone(0);

		constexpr SzI kSzZero(0);
		constexpr SzI kSzOne(1);
		constexpr SzI kSzTwo(2);
		constexpr SzI kSzThree(3);
		constexpr SzI kSzFour(4);
		constexpr SzI kSzEight(8);

		constexpr SzI kSzBinSingle(1 << 0);
		constexpr SzI kSzBinKilo(1 << 10);
		constexpr SzI kSzBinMega(1 << 20);
		constexpr SzI kSzBinGiga(1 << 30);

		constexpr SzSI kSzSNull(0x7FFFFFFF);
		constexpr SzSI kSzSNone(0);

		constexpr SzSI kSzSZero(0);
		constexpr SzSI kSzSOne(1);
	}
	using namespace $SzDef;

	tmpl_st(T) constexpr auto szOf() { return static_cast<SzI>(sizeof(T)); }
	tmpl_st(T) inline auto szOf(SzI const & cnt) { return static_cast<SzI>(static_cast<SzI>(sizeof(T)) * cnt); }

	namespace $MaxStrLenDef {
		constexpr SzI kHex32Str(8);
		constexpr SzI kFormatHex32Str(10);
		constexpr SzI kInt32Str(11); // 1(sgd) + 10(num)
		constexpr SzI kInt64Str(21); // 1(sgd) + 20(num)
		constexpr SzI kFloat32Str(8); // 1(sgd) + 7(num-prec)
		constexpr SzI kFloat64Str(17); // 1(sgd) + 16(num-prec)
	#if defined PLATFORM_WINDOWS
		constexpr SzI kPathStr(260);
		constexpr SzI kDriveLetterStr(2);
	#elif defined PLATFORM_LINUX
		constexpr SzI kPathStr(2048);
	#endif
	}

	constexpr Bool kFalse(false);
	constexpr Bool kTrue(true);

	constexpr Byte kNullByte(0);

	namespace $Char {
		constexpr Ch kNull('\0');
		constexpr Ch kSpace(' ');
		constexpr Ch kOctothorpe('#');
		constexpr Ch kStarKey('*');
		constexpr Ch kTabulator('\t');
		constexpr Ch kLineFeed('\n');
		constexpr Ch kCarriageReturn('\r');
		constexpr Ch kAt('@');
		constexpr Ch kOpenParenthesis('(');
		constexpr Ch kCloseParenthesis(')');
		constexpr Ch kOpenBracket('[');
		constexpr Ch kCloseBracket(']');
		constexpr Ch kOpenBrace('{');
		constexpr Ch kCloseBrace('}');
		constexpr Ch kColon(':');
		constexpr Ch kQuestionMark('?');
		constexpr Ch kSingleQuotationMark('\'');
		constexpr Ch kDoubleQuotationMark('\"');
		constexpr Ch kPeriod('.');
		constexpr Ch kComma(',');
		constexpr Ch kForwardSlash('/');
		constexpr Ch kBackSlash('\\');
		namespace $Esc {
			constexpr Ch kNull('\0');
			constexpr Ch kBell('\a');
			constexpr Ch kBackSpace('\b');
			constexpr Ch kFormFeed('\f');
			constexpr Ch kLineFeed('\n');
			constexpr Ch kCarriageReturn('\r');
			constexpr Ch kHTab('\t');
			constexpr Ch kVTab('\v');
			constexpr Ch kBackSlash('\\');
			constexpr Ch kSingleQuotationMark('\'');
			constexpr Ch kDoubleQuotationMark('\"');
		}
	}
	namespace $PathDelimiter {
		constexpr Ch kWindowsStyle('\\');
		constexpr Ch kLinuxStyle('/');
		constexpr Ch kNativeStyle(
		#if defined PLATFORM_WINDOWS
			kWindowsStyle
		#elif defined PLATFORM_LINUX
			kLinuxStyle
		#endif
		);
	}

	constexpr F64 kZeroFloat64(0.0);
	F64 const kErrFloat64(0.0 / 0.0);

}
