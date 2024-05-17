#pragma once

#include "./Type.hpp"

namespace $TJ {

	inline namespace $NullPtr {

		inline constexpr NullPtr kNullPtr(nullptr);

	}

	inline namespace $Bool {

		inline constexpr Bool kBoF(false);
		inline constexpr Bool kBoT(true);

	}

	inline namespace $Sz {

		inline constexpr Sz kNoX(~0);

		inline constexpr Sz kNo1(0);
		inline constexpr Sz kNo2(1);
		inline constexpr Sz kNo3(2);
		inline constexpr Sz kNo4(3);
		inline constexpr Sz kNo5(4);
		inline constexpr Sz kNo6(5);

		inline constexpr Sz kSzX(~0);
		inline constexpr Sz kSzN(0);

		inline constexpr Sz kSz0(0);
		inline constexpr Sz kSz1(1);
		inline constexpr Sz kSz2(2);
		inline constexpr Sz kSz3(3);
		inline constexpr Sz kSz4(4);
		inline constexpr Sz kSz8(8);

		inline constexpr Sz kSzBB(1 << 0);
		inline constexpr Sz kSzBK(1 << 10);
		inline constexpr Sz kSzBM(1 << 20);
		inline constexpr Sz kSzBG(1 << 30);

		inline constexpr SSz kSNoX(0x7FFFFFFF);
		inline constexpr SSz kSSzX(0x7FFFFFFF);
		inline constexpr SSz kSSzN(0);

	}

	constexpr Byte kByteNull(0);

	namespace $MaxStrLenDef {

		inline constexpr Sz kDec32(10);
		inline constexpr Sz kDec32WithSgn(10 + 1);
		inline constexpr Sz kDec64WithSgn(20 + 1);

		inline constexpr Sz kHex32(8);
		inline constexpr Sz kHex32WithPrefix(kHex32 + 2);

		inline constexpr Sz kFloat32(7 + 1); // 7-prec
		inline constexpr Sz kFloat64(16 + 1); // 16-prec

		inline constexpr Sz kPath(
		#if defined PLATFORM_WINDOWS
			260
		#elif defined PLATFORM_LINUX
			2048
		#endif
		);

	#if defined PLATFORM_WINDOWS

		inline constexpr Sz kDriveLetter(2);

	#endif

	}

	namespace $Ch {

		inline constexpr Ch kNull('\0');

		inline constexpr Ch kSpace(' ');
		inline constexpr Ch kOctothorpe('#');
		inline constexpr Ch kStarKey('*');
		inline constexpr Ch kTabulator('\t');
		inline constexpr Ch kLF('\n');
		inline constexpr Ch kCR('\r');
		inline constexpr Ch kAt('@');
		inline constexpr Ch kOpenParenthesis('(');
		inline constexpr Ch kCloseParenthesis(')');
		inline constexpr Ch kOpenBracket('[');
		inline constexpr Ch kCloseBracket(']');
		inline constexpr Ch kOpenBrace('{');
		inline constexpr Ch kCloseBrace('}');
		inline constexpr Ch kColon(':');
		inline constexpr Ch kQuestionMark('?');
		inline constexpr Ch kSingleQuotationMark('\'');
		inline constexpr Ch kDoubleQuotationMark('\"');
		inline constexpr Ch kPeriod('.');
		inline constexpr Ch kComma(',');

		inline constexpr Ch kForwardSlash('/');
		inline constexpr Ch kBackSlash('\\');

		namespace $Esc {

			inline constexpr Ch kNull('\0');
			inline constexpr Ch kBell('\a');
			inline constexpr Ch kBackSpace('\b');
			inline constexpr Ch kFormFeed('\f');
			inline constexpr Ch kLF('\n');
			inline constexpr Ch kCR('\r');
			inline constexpr Ch kHTab('\t');
			inline constexpr Ch kVTab('\v');

			inline constexpr Ch kBackSlash('\\');
			inline constexpr Ch kSingleQuotationMark('\'');
			inline constexpr Ch kDoubleQuotationMark('\"');

		}

		inline constexpr Ch kAlternativeChOfSpaceWhenPathEnd('#');

	}

	namespace $PathDelimiter {

		inline constexpr Ch kOnLinux('/');
		inline constexpr Ch kOnWindows('\\');

		inline constexpr Ch kOnNative(
		#if defined PLATFORM_WINDOWS
			kOnWindows
		#elif defined PLATFORM_LINUX
			kOnLinux
		#endif
		);

		inline constexpr Ch kSet[]{ kOnLinux, kOnWindows };

	}

	namespace $ChType {

		inline constexpr auto isNull(Ch const & t) {
			return t == $Ch::kNull;
		}

		inline constexpr IU8 kCaseDiff('a' - 'A');

		inline constexpr auto isAlphaLower(Ch const & t) {
			return t >= 'a' && t <= 'z';
		}
		inline constexpr auto isAlphaUpper(Ch const & t) {
			return t >= 'A' && t <= 'Z';
		}
		inline constexpr auto isAlpha(Ch const & t) {
			return isAlphaLower(t) || isAlphaUpper(t);
		}

		inline constexpr auto isHexAlphaLower(Ch const & t) {
			return t >= 'a' && t <= 'f';
		}
		inline constexpr auto isHexAlphaUpper(Ch const & t) {
			return t >= 'A' && t <= 'F';
		}
		inline constexpr auto isHexAlpha(Ch const & t) {
			return isHexAlphaLower(t) || isHexAlphaUpper(t);
		}

		inline constexpr auto isBinNum(Ch const & t) {
			return t >= '0' && t <= '1';
		}
		inline constexpr auto isOctNum(Ch const & t) {
			return t >= '0' && t <= '7';
		}
		inline constexpr auto isDecNum(Ch const & t) {
			return t >= '0' && t <= '9';
		}

		inline constexpr auto isHexNumLower(Ch const & t) {
			return isDecNum(t) || isHexAlphaLower(t);
		}
		inline constexpr auto isHexNumUpper(Ch const & t) {
			return isDecNum(t) || isHexAlphaUpper(t);
		}
		inline constexpr auto isHexNum(Ch const & t) {
			return isDecNum(t) || isHexAlphaLower(t) || isHexAlphaUpper(t);
		}

		inline constexpr auto isPathDelimiter(Ch const & t) {
			return t == $PathDelimiter::kOnWindows || t == $PathDelimiter::kOnLinux;
		}

		inline constexpr auto fromHexNum(Ch const & t) -> IU8 {
			if (isHexAlphaLower(t))
				return t - 'a' + 0xa;
			else if (isHexAlphaUpper(t))
				return t - 'A' + 0xA;
			else if (isDecNum(t))
				return t - '0';
			return t; // error
		}
		inline constexpr auto toHexNumChLower(IU8 const & t) -> Ch {
			if (t >= 0xa)
				return 'a' + t - 0xa;
			else
				return '0' + t;
		}
		inline constexpr auto toHexNumChUpper(IU8 const & t) -> Ch {
			if (t >= 0xA)
				return 'A' + t - 0xA;
			else
				return '0' + t;
		}

		inline constexpr auto fromHexNumDouble(Ch const & first, Ch const & second) -> IU8 {
			return fromHexNum(first) * 0x10 + fromHexNum(second);
		}
		inline constexpr auto toHexNumDoubleChLower(IU8 const & num, Ch & first, Ch & second) -> Void {
			first = toHexNumChUpper(num / 0x10);
			second = toHexNumChUpper(num % 0x10);
			return;
		}

	}

	inline constexpr F64 kFloatZero(0.0);
	inline F64 const kFloatNan(0.0 / 0.0);

	template <typename T, Sz n = kSz1>
	inline constexpr Sz kTSz = sizeof(T) * n;

	template <typename T>
	inline constexpr auto szOf(Sz const & n) {
		return static_cast<Sz>(kTSz<T> * n);
	}

	template <typename T1, typename T2>
	inline constexpr Bool kTSame = std::is_same<T1, T2>::value;

	template <typename T>
	inline constexpr Bool kTIsBool = kTSame<T, Bool>;
	template <typename T>
	inline constexpr Bool kTIsIntU = kTSame<T, IU8> || kTSame<T, IU16> || kTSame<T, IU32> || kTSame<T, IU64>;
	template <typename T>
	inline constexpr Bool kTIsIntS = kTSame<T, IS8> || kTSame<T, IS16> || kTSame<T, IS32> || kTSame<T, IS64>;
	template <typename T>
	inline constexpr Bool kTIsInt = kTIsIntU<T> || kTIsIntS<T>;
	template <typename T>
	inline constexpr Bool kTIsFloat = kTSame<T, F32> || kTSame<T, F64>;

}
