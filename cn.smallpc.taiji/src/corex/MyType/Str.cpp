# pragma once

# include <cstdio>
# include <cstdarg>
# include <cstring>
# include <cmath>
# include "./Str.h"

#if defined PLATFORM_ANDROID
# include <strings.h>
#endif

uint32_t f_strlen(char const * const & s) { return static_cast<uint32_t>(strlen(s)); }
int f_strcmp(char const * const & s1, char const * const & s2) { return strcmp(s1, s2); }
int f_stricmp(char const * const & s1, char const * const & s2) {
#if defined PLATFORM_WINDOWS
	return stricmp(s1, s2);
#elif defined PLATFORM_ANDROID
	return strcasecmp(s1, s2);
#endif
}

inline auto f_vsnprintf(char * _Buf, size_t _BufCnt, char const * _Fmt, va_list _ArgList) {
	return
	# if defined PLATFORM_WINDOWS
		_vsnprintf
	# else
		vsnprintf
	# endif
		(_Buf, _BufCnt, _Fmt, _ArgList);
}

namespace $TJ {

	SzI calcEscCharSize(Ch const & src) {
		SzI len;
		switch (src) {
			default: len = kSzThree; break;
			case $Char::$Esc::kNull:
			case $Char::$Esc::kBell:
			case $Char::$Esc::kBackSpace:
			case $Char::$Esc::kFormFeed:
			case $Char::$Esc::kLineFeed:
			case $Char::$Esc::kCarriageReturn:
			case $Char::$Esc::kHTab:
			case $Char::$Esc::kVTab:
			case $Char::$Esc::kBackSlash:
			case $Char::$Esc::kSingleQuotationMark:
			case $Char::$Esc::kDoubleQuotationMark: len = kSzOne; break;
		}
		return len;
	}
	SzI readEscChar(Ch const * const & src, Ch & dst) {
		auto len = kSzZero;
		switch (src[len++]) {
			default: len = kSzNull; break;
			case '0':
				if (!kCharIntervalOfOctNumber.btw(src[len])) dst = $Char::kNull;
				else { sscanf(src + --len, "%03hho", &dst); len += kSzThree; }
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7': sscanf(src + --len, "%03hho", &dst); len += kSzThree; break;
			case 'x': sscanf(src + len, "%02hhX", &dst); len += kSzTwo; break;
			case 'a': dst = $Char::$Esc::kBell; break;
			case 'b': dst = $Char::$Esc::kBackSpace; break;
			case 'f': dst = $Char::$Esc::kFormFeed; break;
			case 'n': dst = $Char::$Esc::kLineFeed; break;
			case 'r': dst = $Char::$Esc::kCarriageReturn; break;
			case 't': dst = $Char::$Esc::kHTab; break;
			case 'v': dst = $Char::$Esc::kVTab; break;
			case '\\': dst = $Char::$Esc::kBackSlash; break;
			case '\'': dst = $Char::$Esc::kSingleQuotationMark; break;
			case '\"': dst = $Char::$Esc::kDoubleQuotationMark; break;
		}
		return len;
	}
	SzI writeEscChar(Ch const & src, Ch * const & dst) {
		auto len = kSzZero;
		switch (src) {
			default: dst[len++] = 'x'; snprintf(dst + len, kSzTwo + kSzOne, "%02hhX", src); len += kSzTwo; break;
			case $Char::$Esc::kNull: dst[len++] = '0'; break;
			case $Char::$Esc::kBell: dst[len++] = 'a'; break;
			case $Char::$Esc::kBackSpace: dst[len++] = 'b'; break;
			case $Char::$Esc::kFormFeed: dst[len++] = 'f'; break;
			case $Char::$Esc::kLineFeed: dst[len++] = 'n'; break;
			case $Char::$Esc::kCarriageReturn: dst[len++] = 'r'; break;
			case $Char::$Esc::kHTab: dst[len++] = 't'; break;
			case $Char::$Esc::kVTab: dst[len++] = 'v'; break;
			case $Char::$Esc::kBackSlash:
			case $Char::$Esc::kSingleQuotationMark:
			case $Char::$Esc::kDoubleQuotationMark: dst[len++] = src; break;
		}
		return len;
	}

	using CVarArgList = va_list;

	static constexpr Interval<SzI> kStrBufLenInterval(kSzZero, static_cast<SzI>(16u * kSzBinKilo), kTrue, kFalse);
	static constexpr Interval<int> kPrintFmtResultInterval(static_cast<int>(kStrBufLenInterval._min), static_cast<int>(kStrBufLenInterval._max), kTrue, kFalse);

	static Ch kStrBuf[kStrBufLenInterval._max];

	Str & Str::printFmt(Ch const * const fmt, ...) {
		CVarArgList argList;
		va_start(argList, fmt);
		auto result = f_vsnprintf(kStrBuf, kStrBufLenInterval._max, fmt, argList);
		va_end(argList);
		return kPrintFmtResultInterval.btw(result) ? cpy(kStrBuf, static_cast<SzI>(result)) : setNull();
	}
	Str & Str::printFmtByAppend(Ch const * const fmt, ...) {
		CVarArgList argList;
		va_start(argList, fmt);
		auto result = f_vsnprintf(kStrBuf, kStrBufLenInterval._max, fmt, argList);
		va_end(argList);
		return kPrintFmtResultInterval.btw(result) ? append(kStrBuf, static_cast<SzI>(result)) : *this;
	}
	Str const & Str::scanFmt(Ch const * const fmt, ...) const {
		CVarArgList argList;
		va_start(argList, fmt);
		vsscanf(_str, fmt, argList);
		va_end(argList);
		return *this;
	}
	Str const & Str::scanFmt(SzI const & beg, Ch const * const fmt, ...) const {
		CVarArgList argList;
		va_start(argList, fmt);
		vsscanf(_str + beg, fmt, argList);
		va_end(argList);
		return *this;
	}

	SzI cnvByteSizeStr(Str const & str) {
		SzI val; Ch unit;
		str.scanFmt("%u%c", &val, &unit);
		switch (unit) {
			default: wErr("UnKnown Byte Unit"); break;
			case 'B': val *= kSzBinSingle; break;
			case 'K': val *= kSzBinKilo; break;
			case 'M': val *= kSzBinMega; break;
			case 'G': val *= kSzBinGiga; break;
		}
		return val;
	}

	Str & writeHexStr(Byte const * const & src, SzI const & sz, Str & dst, Bool const & insSpace) {
		dst.alloc(insSpace ? (sz * 3u - 1u) : sz * 2u);
		for_cnt(i, kThFirst, sz) {
			dst.printFmtByAppend("%02hhX", src[i]);
			if (insSpace && i < (sz - kThSecond))
				dst.append($Char::kSpace);
		}
		return dst;
	}
	Void readHexStr(Str const & src, Byte * const & dst, SzI & sz, Bool const & insSpace) {
		sz = kSzNone;
		for (auto i = kThFirst; i < src._len;) {
			src.scanFmt(i, "%02hhX", &dst[sz++]);
			i += kSzTwo;
			if (insSpace && i < (src._len - kThSecond))
				++i;
		}
		return;
	}

	namespace $StrUtil {

		static Ch gTmpStrBuf[65536 * 2];

		SzI const kUTF8ChLenTable[]{
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

		Void skipCmt(Ch const * const & src, SzI & pos) {
			auto style = src[pos++];
			//wLogLn("comment style : %c at %X", style, pos);
			if (style == '/')
				while (readUTF8Ch(src, pos) != '\n');
			elif(style == '*')
				while (readUTF8Ch(src, pos) != '*' || readUTF8Ch(src, pos) != '/');
			else
				wErr("error comment style : %c at %X", style, pos);
			//wLogLn("comment end : %c at %X", style, pos);
			return;
		}

		Void readEscChar(Ch const * const & src, SzI & pos, Ch & dst) {
			Ch ch;
			switch (ch = src[pos++]) {
				default: wErr("err esc-char at %X", pos); break;
				case '0': dst = '\0'; break;
				case 'x': {
					dst = '\0';
					ch = src[pos++];
					if (ch >= '0' && ch <= '9')
						dst += (ch - '0') * 0x10;
					elif(ch >= 'A' && ch <= 'F')
						dst += (ch - 'A' + 0xA) * 0x10;
					elif(ch >= 'a' && ch <= 'f')
						dst += (ch - 'a' + 0xa) * 0x10;
					else wErr("unk parse-hex-char first");
					ch = src[pos++];
					if (ch >= '0' && ch <= '9')
						dst += ch - '0';
					elif(ch >= 'A' && ch <= 'F')
						dst += ch - 'A' + 0xA;
					elif(ch >= 'a' && ch <= 'f')
						dst += ch - 'a' + 0xa;
					else wErr("unk parse-hex-char second");
					break;
				}
				case 'a': dst = '\a'; break;
				case 'b': dst = '\b'; break;
				case 'f': dst = '\f'; break;
				case 'n': dst = '\n'; break;
				case 'r': dst = '\r'; break;
				case 't': dst = '\t'; break;
				case 'v': dst = '\v'; break;
				case '\\': dst = '\\'; break;
				case '\'': dst = '\''; break;
				case '\"': dst = '\"'; break;
			}
			return;
		}
		Void writeEscChar(Ch const & src, Ch * const & dst, SzI & pos) {
			switch (src) {
				default: {
					dst[pos++] = 'x';
					auto first = static_cast<UI8>((src & 0xF0) >> 4);
					auto second = static_cast<UI8>((src & 0x0F) >> 0);
					//if (first < 0xA)
					dst[pos++] = first + (first < 0xA) ? '0' : ('A' - 0xA);
					dst[pos++] = second + (second < 0xA) ? '0' : ('A' - 0xA);
					break;
				}
				case '\0': dst[pos++] = '0'; break;
				case '\a': dst[pos++] = 'a'; break;
				case '\b': dst[pos++] = 'b'; break;
				case '\f': dst[pos++] = 'f'; break;
				case '\n': dst[pos++] = 'n'; break;
				case '\r': dst[pos++] = 'r'; break;
				case '\t': dst[pos++] = 't'; break;
				case '\v': dst[pos++] = 'v'; break;
				case '\\': case '\'': case '\"': dst[pos++] = src; break;
			}
			return;
		}

		Str & readUTF8Str(Ch const * const & src, SzI & pos, Str & dst) {
			auto len = kSzZero;
			while_nonstop{
				auto ch = src[pos++];
				auto chLen = getUTF8ChLen(ch);
				//wLog("%x len %u\n", ch, chLen);
				if (chLen == kSzOne) {
					if (ch == '\\') {
						readEscChar(src, pos, gTmpStrBuf[len++]);
					} elif(ch == '\"') {
						break;
					} else {
						gTmpStrBuf[len++] = ch;
					}
				} else {
					gTmpStrBuf[len++] = ch;
					if (chLen >= 2)
						gTmpStrBuf[len++] = src[pos++];
					if (chLen >= 3)
						gTmpStrBuf[len++] = src[pos++];
					if (chLen >= 4)
						gTmpStrBuf[len++] = src[pos++];
				}
			}
			dst.cpy(gTmpStrBuf, len);
			return dst;
		}
		Void writeUTF8Str(Str const & src, Ch * const & dst, SzI & pos) {
			for_cnt(i, kThFirst, src._len) {
				auto chLen = getUTF8ChLen(src[i]);
				if (chLen == kSzOne) {
					if (static_cast<UI8>(src[i]) < 0x20 || src[i] == '\"' || src[i] == '\\' || static_cast<UI8>(src[i]) == 0x7Fu) {
						dst[pos++] = '\\';
						writeEscChar(src[i], dst, pos);
					} else {
						dst[pos++] = src[i];
					}
				} else {
					dst[pos++] = src[i];
					if (chLen >= 2)
						dst[pos++] = src[++i];
					if (chLen >= 3)
						dst[pos++] = src[++i];
					if (chLen >= 4)
						dst[pos++] = src[++i];
				}
			}
			return;
		}

		static Ch gTmpForCnv[1024 + 1];

		SzI putChN(Ch const & ch, SzI const & cnt, Ch * const & dst) {
			for_cnt(i, kThFirst, cnt)
				dst[i] = ch;
			return cnt;
		}

		SzI numToStr(UI64 src, Ch * const & dst, SzI & pos) {
			auto oriPos = pos;
			if (src == 0) {
				dst[pos++] = '0';
			} else {
				auto len = kSzNone;
				while (src > 0) {
					gTmpForCnv[len++] = '0' + src % 10;
					src /= 10;
				}
				while (len > kSzNone) {
					dst[pos++] = gTmpForCnv[--len];
				}
			}
			return pos - oriPos;
		}
		SzI numToStr(SI64 src, Ch * const & dst, SzI & pos, Bool const & outSgnAlways) {
			auto oriPos = pos;
			if (src == 0) {
				dst[pos++] = '0';
			} else {
				if (src < 0) {
					dst[pos++] = '-';
					src = -src;
				} elif(outSgnAlways) {
					dst[pos++] = '+';
				}
				numToStr(static_cast<UI64>(src), dst, pos);
			}
			return pos - oriPos;
		}
		SzI numToStr(F64 src, Ch * const & dst, SzI & pos, Bool const & outSgnAlways) {
			auto oriPos = pos;
			if (src == 0.0) {
				dst[pos++] = '0';
				dst[pos++] = '.';
				dst[pos++] = '0';
			} else {
				if (src < 0.0) {
					dst[pos++] = '-';
					src = -src;
				} elif(outSgnAlways) {
					dst[pos++] = '+';
				}
				auto partInt = static_cast<UI64>(src);
				src -= partInt;
				auto intLen = numToStr(partInt, dst, pos);
				dst[pos++] = '.';
				if (src == 0.0) {
					dst[pos++] = '0';
				} else {
					auto len = kSzNone;
					while (src > 0.0 && len + intLen < 16u) {
						src *= 10.0;
						partInt = static_cast<UI8>(src + 0.00000001);
						if (partInt == 10u)
							partInt = 9u;
						dst[pos++] = '0' + partInt;
						src -= partInt;
						++len;
					}
					while (len > kSzOne) {
						if (dst[pos - kThSecond] == '0')
							dst[--pos] = '\0';
						--len;
					}
				}
			}
			return pos - oriPos;
		}

		NumType strToNum(Ch const * const & src, SzI & pos, Void * const & dst) {
			auto & vInt = *reinterpret_cast<UI64 *>(dst);
			auto ch = src[pos++];
			auto isNeg = kFalse;
			NumType type;
			if (ch == '+') {
				type = NumType::kSInt;
				vInt = 0u;
			} elif(ch == '-') {
				type = NumType::kSInt;
				vInt = 0u;
				isNeg = kTrue;
			} else {
				type = NumType::kUInt;
				vInt = ch - '0';
			}
			while_nonstop{
				ch = src[pos++];
				if (ch >= '0' && ch <= '9') {
					vInt *= 10u;
					vInt += ch - '0';
				} else {
					if (ch == '.') {
						type = NumType::kFloat;
						auto & vFloat = *reinterpret_cast<F64 *>(dst);
						vFloat = static_cast<F64>(vInt);
						F64 vBase = 0.1;
						while_nonstop{
							ch = src[pos++];
							if (ch >= '0' && ch <= '9') {
								vFloat += (ch - '0') * vBase;
								vBase /= 10.0;
							} else break;
						}
					}
					--pos;
					break;
				}
			}
				if (isNeg) {
					if (type == NumType::kSInt)
						(*reinterpret_cast<SI64 *>(dst)) = -*reinterpret_cast<SI64 *>(dst);
					elif(type == NumType::kFloat)
						(*reinterpret_cast<F64 *>(dst)) = -*reinterpret_cast<F64 *>(dst);
				}
			return type;
		}
	}
}
