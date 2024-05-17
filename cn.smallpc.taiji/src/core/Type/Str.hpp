#pragma once

#include "../Type.hpp"
#include "../Const.hpp"
#include "../TypeUtil.hpp"
#include "./List.hpp"
#include "./ByteStream.hpp"

#include <regex>

namespace $TJ {

	using Str = List<Ch>;

	template <typename T>
	inline constexpr Bool kTIsStr = kTSame<T, Str>;

	inline auto & bindCStr(Str & dst, ConstCStr const & src, Sz const & len = kSzX) {
		dst.bind(src, len == kSzX ? getCStrLen(src) : len);
		return dst;
	}
	inline auto & refCStr(Str & dst, ConstCStr const & src, Sz const & len = kSzX) {
		dst.ref(src, len == kSzX ? getCStrLen(src) : len);
		return dst;
	}
	inline auto & cpyCStr(Str & dst, ConstCStr const & src, Sz const & len = kSzX) {
		dst.cpy(src, len == kSzX ? getCStrLen(src) : len);
		return dst;
	}

	// convert c-style string to string-ref class
	inline auto operator""_r(char const * const s, std::size_t const len) {
		Str r;
		r.ref(s, static_cast<Sz>(len));
		return r;
	}
	// convert c-style string to string class (deep-copy)
	inline auto operator""_s(char const * const s, std::size_t const len) {
		return Str(s, static_cast<Sz>(len));
	}

	inline Str const kNullStr;
	inline Str const kEmptyStr(""_r);

	template <typename T>
	struct StrBlock {

		Str $str;

		auto sz() const {
			return static_cast<Sz>(kTSz<T> +$str.$sz);
		}

		auto & read(ByteStream & t) {
			T len;
			t >> len;
			$str.allocThenUse(len);
			t >> $str;
			return thiz;
		}
		auto & write(ByteStream & t) const {
			t << T($str.$sz) << $str;
			return thiz;
		}

	};

	namespace $Str {

		inline Str & readStrFromRaw(Str & s, ByteStream & stm) {
			auto len = kSzN;
			while (stm.rd<Ch>() != $Ch::kNull)
				++len;
			stm.posBack<Ch>().posBack<Ch>(len);
			s.allocThenUse(len);
			stm >> s;
			stm.posGo<Ch>();
			return s;
		}
		inline List<Str> & readStrFromRaw(List<Str> & s, ByteStream & stm) {
			for (auto & e : s)
				readStrFromRaw(e, stm);
			return s;
		}

		inline Void writeStrToRaw(Str const & s, ByteStream & stm) {
			stm << s << $Ch::kNull;
			return;
		}
		inline Void writeStrToRaw(List<Str> const & s, ByteStream & stm) {
			for (auto & e : s)
				writeStrToRaw(e, stm);
			return;
		}

		int xCmp(Str const & s1, Str const & s2);

		Bool cmpIgnoreCase(Str const & s1, Str const & s2);

		inline auto endWith(Str const & parent, Str const & sub) {
			return parent.$sz >= sub.$sz && parent.range(parent.$sz - sub.$sz).cmp(sub.begin());
		}

		inline auto & splitWithPrefix(Str const & src, List<Str> & dst, Ch const & ch) {
			dst.alloc(src.range().cnt(ch));
			Str * ptr(kNullPtr);
			for (auto const & c : src)
				if (c == ch)
					ptr = &dst.appendX().last().alloc(src.$sz);
				else
					(*ptr).append(c);
			return dst;
		}
		inline auto & split(Str const & src, List<Str> & dst, Ch const & ch) {
			if (src.isNull()) {
				dst.setNull();
			} else if (src.isEmpty()) {
				dst.setEmpty();
			} else {
				dst.alloc(src.range().cnt(ch) + kSz1);
				auto posBeg = kNo1, posEnd = kNo1;
				while (posEnd < src.$sz) {
					if (src[posEnd] == ch) {
						dst.appendX().last().cpy(src.$data + posBeg, posEnd - posBeg);
						posBeg = posEnd + kNo2;
					}
					++posEnd;
				}
				dst.appendX().last().cpy(src.$data + posBeg, posEnd - posBeg);
			}
			return dst;
		}
		template <typename TChRange>
		inline auto & split(Str const & src, List<Str> & dst, TChRange const & ch) {
			if (src.isNull()) {
				dst.setNull();
			} else if (src.isEmpty()) {
				dst.setEmpty();
			} else {
				auto chRange = ch.range();
				dst.alloc(src.range().cntN(chRange) + kSz1);
				auto posBeg = kNo1, posEnd = kNo1;
				while (posEnd < src.$sz) {
					if (chRange.exist(src[posEnd])) {
						dst.appendX().last().cpy(src.$data + posBeg, posEnd - posBeg);
						posBeg = posEnd + kNo2;
					}
					++posEnd;
				}
				dst.appendX().last().cpy(src.$data + posBeg, posEnd - posBeg);
			}
			return dst;
		}

		inline auto regexMatch(Str const & regex, Str const & t, Bool const & ignoreCase = kBoF) {
			auto flag = std::regex_constants::syntax_option_type::ECMAScript;
			if (ignoreCase)
				flag |= std::regex_constants::syntax_option_type::icase;
			return std::regex_match(std::string(t.$data, t.$sz), std::regex(regex.$data, regex.$sz, flag));
		}
		inline auto regexMatchSub(Str const & regex, Str const & t, Bool const & ignoreCase = kBoF) {
			auto flag = std::regex_constants::syntax_option_type::ECMAScript;
			if (ignoreCase)
				flag |= std::regex_constants::syntax_option_type::icase;
			return std::regex_search(std::string(t.$data, t.$sz), std::regex(regex.$data, regex.$sz, flag));
		}

		using Selector = Str;

		inline auto selectorIsRegex(Selector const & t) {
			return !t.isEmpty() && (t.first() == '!' || t.first() == '?');
		}

		inline auto selectorMatch(Selector const & t, Str const & s, Bool const & ignoreCase = kBoF) {
			if (selectorIsRegex(t)) {
				if (t.first() == '!')
					return $Str::regexMatch(Str().ref(t, kNo2), s, ignoreCase);
				else
					return $Str::regexMatchSub(Str().ref(t, kNo2), s, ignoreCase);
			} else {
				if (ignoreCase)
					return cmpIgnoreCase(t, s);
				else
					return t == s;
			}
		}

		inline auto & toLower(Str & s) {
			s.range().each(
				[](auto & c) {
					if ($ChType::isAlphaUpper(c))
						c += $ChType::kCaseDiff;
				}
			);
			return s;
		}
		inline auto & toUpper(Str & s) {
			s.range().each(
				[](auto & c) {
					if ($ChType::isAlphaLower(c))
						c -= $ChType::kCaseDiff;
				}
			);
			return s;
		}
		inline auto & toLower(List<Str> & t) {
			t.range().each([](auto & v) { toLower(v); });
			return t;
		}
		inline auto & toUpper(List<Str> & t) {
			t.range().each([](auto & v) { toUpper(v); });
			return t;
		}
		template <typename T>
		inline auto & mapKeyUpper(T & t) {
			t.range().each([](auto & v) { toUpper(v.$k); });
			return t;
		}

		Sz calcEscCharSz(Ch const & src);
		Sz readEscChar(ConstCStr const & src, Ch & dst);
		Sz writeEscChar(Ch const & src, CStr const & dst);

		Sz putChN(Ch const & c, Sz const & n, CStr const & dst);

		enum class NumType : IU8 { kUInt, kSInt, kFloat };

		Sz numToStr(IU64 src, CStr const & dst);
		Sz numToStr(IS64 src, CStr const & dst, Bool const & outSgnAlways = kBoF);
		Sz numToStr(F64 src, CStr const & dst, Bool const & outSgnAlways = kBoF);

		Sz strToNum(ConstCStr const & src, Void * const & dst, NumType & type);

		inline constexpr Sz kUTF8ChLenTable[]{
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
			4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
		};

		inline constexpr auto isASCII(Ch const & c) {
			return (c & 0x80u) == 0;
		}

		inline auto getUTF8ChLen(Ch const & c) {
			return kUTF8ChLenTable[static_cast<IU8>(c)];
		}

		// if not ascii , return -1
		inline auto readUTF8Ch(ConstCStr const & src, Sz & pos) {
			auto c = src[pos];
			auto len = getUTF8ChLen(c);
			pos += len;
			return len == kSz1 ? c : static_cast<Ch>(0xFF);
		}

		// ignore first '/'
		Sz cntCmtLen(ConstCStr const & src);

		Sz readUTF8Str(ConstCStr const & src, Str & dst);
		Sz writeUTF8Str(Str const & src, CStr const & dst);

		Str printFmt(ConstCStr const fmt, ...);
		Void scanFmt(Str const & str, ConstCStr const fmt, ...);

		Sz cnvByteSzStr(Str const & str);

		inline auto calcByteSzFromHexStr(Sz const & len, Bool const & insSpace = kBoT) {
			return static_cast<Sz>(insSpace ? ((len + 1) / 3) : (len / 2));
		}
		Str & writeHexStr(ByteSection const & data, Str & str, Bool const & insSpace = kBoT);
		Void readHexStr(Str const & str, ByteSection & data, Bool const & insSpace = kBoT);

	}

	inline auto operator>(Str const & a, Str const & b) {
		return $Str::xCmp(a, b) > 0;
	}
	inline auto operator<(Str const & a, Str const & b) {
		return $Str::xCmp(a, b) < 0;
	}

	struct VAMGException : Exception {

		VAMGException(ConstCStr const & tag, ConstCStr const fmt, ...);

		template <typename T>
		auto to() {
			return T();
		}

	};

}
