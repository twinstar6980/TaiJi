# include "./LawnStrTextUtil.h"

# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

# include <codecvt>

namespace $TJ::$SpecialMod::$PvZ_2 {
	namespace $LawnStrTextUtil {
		Str & CnvJSONToText($JSON::Obj const & src, Str & dst) {
			dst.setNull();
			for (auto const & item : src) {
				dst.cat($Char::kOpenBracket).cat(item._key.get()).cat($Char::kCloseBracket).cat($Char::kLineFeed)
					.cat(item._val.getStr()).cat($Char::kLineFeed).cat($Char::kLineFeed);
			}
			dst.cat($Char::kNull);
			return dst;
		}
		Byte * const & CnvJSONToText_RipeUTF16($JSON::Obj const & src, Byte * const & dst, SzI & dstSz) {
		# if defined PLATFORM_WINDOWS
			Str rawText;
			CnvJSONToText(src, rawText);
			auto tmpStr = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(rawText._str);
			rawText.tfree();
			dst[kThFirst] = 0xFF;
			dst[kThSecond] = 0xFE;
			dstSz = kSzTwo + szOf<Ch16>(tmpStr.length());
			memCpy(dst + kSzTwo, tmpStr.c_str(), dstSz);
			tmpStr.clear();
		# endif
			return dst;
		}
		Void CnvJSONToText_ByFile(Path const & srcFile, Path const & dstFile) {
			Byte * textData(kNullPtr); SzI textSz;
			newPtr(textData, szOf<Ch16>($DevUtil::getFileSize(srcFile)) * kSzTwo);
			CnvJSONToText_RipeUTF16($JSON::read(srcFile, $JSON::Obj().setNull()), textData, textSz);
			$DevUtil::WriteData(dstFile, textData, textSz);
			delPtrArr(textData);
			return;
		}

		$JSON::Obj & CnvTextToJSON(Ch const * const & src, SzI const & srcSz, $JSON::Obj & dst) {
			dst.setNull();
			for (auto pos = kThFirst; pos < srcSz;) {
				if (src[pos++] == $Char::kOpenBracket) {
					StrRef keyStr;
					Str valStr;
					SzI strLen;
					for (strLen = kSzZero; src[pos + strLen] != $Char::kCloseBracket; ++strLen);
					keyStr.set(src + pos, strLen);
					pos += strLen + kThSecond;
					for (; (src[pos] == $Char::kLineFeed) || (src[pos] == $Char::kCarriageReturn); ++pos);
					if (src[pos] == $Char::kOpenBracket) {
						--pos;
						valStr.setNull();
					} else {
						for (strLen = kSzZero;;) {
							if ((src[pos + strLen] == $Char::kLineFeed) || (src[pos + strLen] == $Char::kCarriageReturn)) {
								if ((pos + strLen + kSzOne) >= srcSz) {
									--strLen;
									break;
								} else {
									if (src[pos + strLen + kSzOne] == $Char::kOpenBracket) {
										--strLen;
										break;
									}
								}
							}
							++strLen;
						}
						valStr.cpy(src + pos, strLen);
						pos += strLen + kThSecond;
						for (auto idx = valStr._len; idx > kThFirst; --idx) {
							if (((valStr[idx - kThSecond] != $Char::kLineFeed) && (valStr[idx - kThSecond] != $Char::kCarriageReturn)) || (idx == kThSecond)) {
								valStr.eraseAfter(idx);
								break;
							}
						}
					}
					dst.add(keyStr, valStr);
					keyStr.clear();
					valStr.tfree();
				}
			}
			return dst;
		}
		$JSON::Obj & CnvTextToJSON_RipeUTF16(Ch16 const * const & src, SzI const & srcSz, $JSON::Obj & dst) {
		# if defined PLATFORM_WINDOWS
			std::u16string tmpStr16(const_cast<Ch16 *>(src + kSzOne), srcSz - kSzOne);
			auto tmpStr = std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t>().to_bytes(reinterpret_cast<const int16_t *>(tmpStr16.c_str()));
			//std::string tmpStr = std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t>().to_bytes(reinterpret_cast<const int16_t *>(src + 1));
			CnvTextToJSON(reinterpret_cast<Ch const *>(tmpStr.c_str()), tmpStr.length(), dst);
			tmpStr.clear();
		# endif
			return dst;
		}
		Void CnvTextToJSON_ByFile(Path const & srcFile, Path const & dstFile) {
			Byte * textData(kNullPtr); SzI textSz;
			$DevUtil::ForceReadData(srcFile, textData, textSz);
			$JSON::write(dstFile, CnvTextToJSON_RipeUTF16(reinterpret_cast<Ch16 const *>(textData), textSz / szOf<Ch16>(), $JSON::Obj().setNull()));
			delPtrArr(textData);
			return;
		}
	}
}
