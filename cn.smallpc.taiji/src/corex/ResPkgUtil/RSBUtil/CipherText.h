# pragma once

# include "../../Type.h"
# include "../../MyType/Str.h"
# include "../../MyType/StrList.h"
# include "../../MyType/ListP.h"
# include "../../MyType/Map.h"

# include "../../UtilKit/MemoryUtil.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$CipherText {
	tmpl_st(T) struct CipherText : Map<Str, T> {
		auto & sort() {
			(*this).sortUpByKey();
			return *this;
		}
		auto & AllStrUpperCase() {
			for_riter(item, *this)
				item._key.upperCase();
			return *this;
		}

		auto & EnCrypt(Byte * const & dataOri, SzI & szOri) {
			auto data = reinterpret_cast<UI32 *>(dataOri);
			SzI sz;
			SzI chIdx;
			ListB<Bool> strExistParent;
			ListB<Bool> aStrExistOffspring;
			ListB<SzI> strInheritIdx;
			ListB<SzI> parentStrOfs;
			SzI aStrInheritIdx;
			{
				strExistParent.allocThenNew((*this)._sz);
				strInheritIdx.allocThenNew((*this)._sz);
				parentStrOfs.allocThenNew((*this)._sz);
			}
			sz = kSzZero;
			for_cnt(idx, kThFirst, (*this)._sz) {
				aStrExistOffspring.allocThenNew((*this).getKey(idx).getSz());
				for_cnt(idx2, static_cast<SzI>(idx + kThSecond), (*this)._sz)
					if ((*this).getKey(idx).findDiff((*this).getKey(idx2), aStrInheritIdx))
						if ((aStrInheritIdx >= strInheritIdx[idx]) && (!strExistParent[idx2]) && (!aStrExistOffspring[aStrInheritIdx])) {
							strExistParent[idx2] = kTrue;
							strInheritIdx[idx2] = aStrInheritIdx;
							aStrExistOffspring[aStrInheritIdx] = kTrue;
							parentStrOfs[idx2] = sz + (aStrInheritIdx - strInheritIdx[idx]);
						}
				aStrExistOffspring.tfree();
				if (strExistParent[idx]) {
					chIdx = strInheritIdx[idx];
					data[parentStrOfs[idx]] |= (sz << 8);
					strExistParent[idx] = kFalse;
					strInheritIdx.reset(idx, kSzOne);
				} else {
					chIdx = kThFirst;
				}
				while (chIdx < (*this).getKey(idx)._len) {
					memSet(data + sz, kSzOne);
					data[sz++] |= (*this).getKey(idx)[chIdx++];
				}
				memSet(data + sz, kSzOne);
				data[sz++] |= $Char::kNull;
				(*this).getVal(idx).write(reinterpret_cast<Byte *>(data + sz));
				sz += static_cast<SzI>((*this).getVal(idx).getSz() / szOf<UI32>());
			}
			{
				strExistParent.tfree();
				strInheritIdx.tfree();
				parentStrOfs.tfree();
			}
			szOri = szOf<UI32>(sz);
			return *this;
		}
		auto & DeCrypt(Byte const * const & dataOri, SzI const & szOri) {
			auto data = reinterpret_cast<UI32 const *>(dataOri);
			auto sz = getEnoughSz(szOri, szOf<UI32>()) / szOf<UI32>();
			Map<Bool, Str> linkInfo;
			Ch oneChOfStr;
			UI32 linkStrOfs;
			(*this).alloc(sz);
			linkInfo.allocThenNew((*this)._cap);
			for (SzI pos = kThFirst; pos < sz;) {
				auto & textItem = (*this).add().last();
				textItem._key.alloc(1024);
				if (linkInfo.getKey(pos)) {
					linkInfo.getKey(pos) = kFalse;
					textItem._key.cat(linkInfo.getVal(pos));
					linkInfo.getVal(pos).setNull();
				}
				while_nonstop{
					oneChOfStr = data[pos] & 0xFF;
					linkStrOfs = data[pos] >> 8;
					++pos;
					if (linkStrOfs != kThFirst) {
						linkInfo[linkStrOfs].set(kTrue, textItem._key);
					}
					if (oneChOfStr == $Char::kNull) {
						textItem._key.eraseSurplusCap();
						textItem._val.read(reinterpret_cast<Byte const *>(data + pos));
						pos += getEnoughSz(textItem._val.getSz(), szOf<UI32>()) / szOf<UI32>();
						break;
					} else {
						textItem._key.cat(oneChOfStr);
					}
				}
			}
			linkInfo.tfree();
			(*this).eraseSurplusCap();
			return *this;
		}
	};
	SzI CalcCipherTextMinSize(StrList const & strList);
}
