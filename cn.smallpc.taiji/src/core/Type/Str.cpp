#include "./Str.hpp"

#include "../ReDef.hpp"

namespace $TJ {

	namespace $Str {

		int xCmp(Str const & s1, Str const & s2) {
			return frd_strcmp(s1.$data, s2.$data);
		}

		Bool cmpIgnoreCase(Str const & s1, Str const & s2) {
			return (s1.isNull() || s2.isNull()) ? (s1.isNull() && s2.isNull()) : frd_stricmp(s1.$data, s2.$data) == 0;
		}

		Sz calcEscCharSz(Ch const & src) {
			Sz len;
			switch (src) {
				default:
					len = kSz3;
					break;
				case '\0':
				case '\a':
				case '\b':
				case '\f':
				case '\n':
				case '\r':
				case '\t':
				case '\v':
				case '\\':
				case '\'':
				case '\"':
					len = kSz1;
					break;
			}
			return len;
		}
		Sz readEscChar(ConstCStr const & src, Ch & dst) {
			auto len = kSzN;
			switch (src[len++]) {
				default:
					len = kSzX; break;
				case '0':
					if ($ChType::isOctNum(src[len])) {
						dst = $Ch::kNull;
					} else {
						--len;
						dst = src[len++] * 8 * 8;
						dst += src[len++] * 8;
						dst += src[len++];
					}
					break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					--len;
					dst = src[len++] * 8 * 8;
					dst += src[len++] * 8;
					dst += src[len++];
					break;
				case 'x':
					dst = $ChType::fromHexNum(src[len++]) * 0x10;
					dst += $ChType::fromHexNum(src[len++]);
					break;
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
			return len;
		}
		Sz writeEscChar(Ch const & src, CStr const & dst) {
			auto len = kSzN;
			switch (src) {
				default:
					dst[len++] = 'x';
					dst[len++] = $ChType::toHexNumChUpper(static_cast<IU8>(src) / 0x10);
					dst[len++] = $ChType::toHexNumChUpper(static_cast<IU8>(src) % 0x10);
					break;
				case '\0': dst[len++] = '0'; break;
				case '\a': dst[len++] = 'a'; break;
				case '\b': dst[len++] = 'b'; break;
				case '\f': dst[len++] = 'f'; break;
				case '\n': dst[len++] = 'n'; break;
				case '\r': dst[len++] = 'r'; break;
				case '\t': dst[len++] = 't'; break;
				case '\v': dst[len++] = 'v'; break;
				case '\\':
				case '\'':
				case '\"': dst[len++] = src; break;
			}
			return len;
		}

		Sz putChN(Ch const & c, Sz const & n, CStr const & dst) {
			for (auto i = kNo1; i < n; ++i)
				dst[i] = c;
			return n;
		}

		static Ch gTmpForCnv[128 + 1];

		Sz numToStr(IU64 src, CStr const & dst) {
			auto pos = kNo1;
			if (src == 0) {
				dst[pos++] = '0';
			} else {
				auto len = kSzN;
				while (src > 0) {
					gTmpForCnv[len++] = '0' + src % 10;
					src /= 10;
				}
				while (len > kSzN) {
					dst[pos++] = gTmpForCnv[--len];
				}
			}
			return pos;
		}
		Sz numToStr(IS64 src, CStr const & dst, Bool const & outSgnAlways) {
			auto pos = kNo1;
			if (src == 0) {
				dst[pos++] = '0';
			} else {
				if (src < 0) {
					dst[pos++] = '-';
					src = -src;
				} else if (outSgnAlways) {
					dst[pos++] = '+';
				}
				pos += numToStr(static_cast<IU64>(src), dst + pos);
			}
			return pos;
		}
		Sz numToStr(F64 src, CStr const & dst, Bool const & outSgnAlways) {
			auto pos = kNo1;
			if (src == 0.0) {
				dst[pos++] = '0';
				dst[pos++] = '.';
				dst[pos++] = '0';
			} else {
				if (src < 0.0) {
					dst[pos++] = '-';
					src = -src;
				} else if (outSgnAlways) {
					dst[pos++] = '+';
				}
				auto partInt = static_cast<IU64>(src);
				src -= partInt;
				auto intLen = numToStr(partInt, dst + pos);
				pos += intLen;
				dst[pos++] = '.';
				if (src == 0.0) {
					dst[pos++] = '0';
				} else {
					auto len = kSzN;
					while (src > 0.0 && len + intLen < 16u) {
						src *= 10.0;
						partInt = static_cast<IU8>(src + 0.00000001);
						if (partInt == 10u)
							partInt = 9u;
						dst[pos++] = '0' + partInt;
						src -= partInt;
						++len;
					}
					while (len > kSz1) {
						if (dst[pos - kNo2] == '0')
							dst[--pos] = '\0';
						--len;
					}
				}
			}
			return pos;
		}

		Sz strToNum(ConstCStr const & src, Void * const & dst, NumType & type) {
			auto pos = kNo1;
			auto & vInt = *reinterpret_cast<IU64 *>(dst);
			auto isNeg = kBoF;
			{
				auto ch = src[pos++];
				if (ch == '+') {
					type = NumType::kSInt;
					vInt = 0u;
				} else if (ch == '-') {
					type = NumType::kSInt;
					vInt = 0u;
					isNeg = kBoT;
				} else {
					type = NumType::kUInt;
					vInt = ch - '0';
				}
			}
			while (kBoT) {
				auto ch = src[pos++];
				if ($ChType::isDecNum(ch)) {
					vInt *= 10u;
					vInt += ch - '0';
				} else {
					if (ch == '.') {
						type = NumType::kFloat;
						auto & vFloat = *reinterpret_cast<F64 *>(dst);
						vFloat = static_cast<F64>(vInt);
						F64 vBase = 0.1;
						while (kBoT) {
							ch = src[pos++];
							if ($ChType::isDecNum(ch)) {
								vFloat += (ch - '0') * vBase;
								vBase /= 10.0;
							} else
								break;
						}
					}
					--pos;
					break;
				}
			}
			if (isNeg) {
				if (type == NumType::kSInt)
					(*reinterpret_cast<IS64 *>(dst)) = -*reinterpret_cast<IS64 *>(dst);
				else if (type == NumType::kFloat)
					(*reinterpret_cast<F64 *>(dst)) = -*reinterpret_cast<F64 *>(dst);
			}
			return pos;
		}

		Sz cntCmtLen(ConstCStr const & src) {
			auto len = kSzN;
			auto style = src[len++];
			//log.txtln("comment style : %c at %X", style, pos);
			if (style == '/')
				while (readUTF8Ch(src, len) != '\n')
					;
			else if (style == '*')
				while (readUTF8Ch(src, len) != '*' || readUTF8Ch(src, len) != '/')
					;
			else
				throw VAMGException("$Str::cntCmtLen()", "error comment style : %c at %X", style, len);
			//log.txtln("comment end : %c at %X", style, pos);
			return len;
		}

		static Ch gTmpStrBuf[65536 * 2];

		Sz readUTF8Str(ConstCStr const & src, Str & dst) {
			auto pos = kNo1;
			auto len = kSzN;
			while (kBoT) {
				auto ch = src[pos++];
				auto chLen = getUTF8ChLen(ch);
				//log.txt("%x len %u\n", ch, chLen);
				if (chLen == kSz1) {
					if (ch == '\\') {
						pos += readEscChar(src + pos, gTmpStrBuf[len++]);
					} else if (ch == '\"') {
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
			dst.cpy(Str().ref(&*gTmpStrBuf, len));
			return pos;
		}
		Sz writeUTF8Str(Str const & src, CStr const & dst) {
			auto pos = kNo1;
			for (auto i = kNo1; i < src.$sz; ++i) {
				auto chLen = getUTF8ChLen(src[i]);
				if (chLen == kSz1) {
					if (static_cast<IU8>(src[i]) < 0x20 ||
						static_cast<IU8>(src[i]) == 0x7Fu ||
						src[i] == '\"' ||
						src[i] == '\\') {
						dst[pos++] = '\\';
						pos += writeEscChar(src[i], dst + pos);
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
			return pos;
		}

		static Str kStrBuf(kSzBK * 16);

		static Str printFmt(ConstCStr const fmt, std::va_list const arg) {
			auto result = frd_vsprintf(kStrBuf.$data, kStrBuf.$cap, fmt, arg);
			kStrBuf.$sz = getCStrLen(kStrBuf.$data);
			return kStrBuf;
		}

		Str printFmt(ConstCStr const fmt, ...) {
			std::va_list arg;
			va_start(arg, fmt);
			auto result = frd_vsprintf(kStrBuf.$data, kStrBuf.$cap, fmt, arg);
			va_end(arg);
			kStrBuf.$sz = getCStrLen(kStrBuf.$data);
			return kStrBuf;
		}
		Void scanFmt(Str const & str, ConstCStr const fmt, ...) {
			std::va_list arg;
			va_start(arg, fmt);
			frd_vsscanf(str.$data, fmt, arg);
			va_end(arg);
			return;
		}

		Sz cnvByteSzStr(Str const & str) {
			Sz base; Ch unit;
			scanFmt(str, "%u%c"_r.$data, &base, &unit);
			switch (unit) {
				default:
					throw VAMGException("$Str::cnvByteSzStr()", "unknown byte-unit : %c", unit);
					break;
				case 'B':
					base *= kSzBB;
					break;
				case 'K':
					base *= kSzBK;
					break;
				case 'M':
					base *= kSzBM;
					break;
				case 'G':
					base *= kSzBG;
					break;
			}
			return base;
		}

		Str & writeHexStr(ByteSection const & data, Str & str, Bool const & insSpace) {
			str.alloc(insSpace ? (data.$sz * 3u - 1u) : data.$sz * 2u);
			for (auto const & e : data.range(kNo1, data.$sz - kNo2)) {
				auto & first = str.appendX().last();
				auto & second = str.appendX().last();
				$ChType::toHexNumDoubleChLower(e, first, second);
				if (insSpace)
					str.append($Ch::kSpace);
			}
			auto & first = str.appendX().last();
			auto & second = str.appendX().last();
			$ChType::toHexNumDoubleChLower(data.last(), first, second);
			return str;
		}
		Void readHexStr(Str const & str, ByteSection & data, Bool const & insSpace) {
			data.alloc(calcByteSzFromHexStr(str.$sz, insSpace));
			for (auto i = kNo1; i < str.$sz;) {
				data.append($ChType::fromHexNumDouble(str[i], str[i + kNo2]));
				i += kSz2;
				if (insSpace)
					++i;
			}
			return;
		}

	}

	VAMGException::VAMGException(ConstCStr const & tag, ConstCStr const fmt, ...) : Exception() {
		std::va_list arg;
		va_start(arg, fmt);
		auto msg = $Str::printFmt(fmt, arg);
		va_end(arg);
		$tag = tag;
		$msg = msg.$data;
	}

}
