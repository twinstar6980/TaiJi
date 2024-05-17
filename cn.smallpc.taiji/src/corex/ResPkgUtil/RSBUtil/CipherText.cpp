# include "./CipherText.h"

# include "../../Type.h"
# include "../../MyType/Str.h"
# include "../../MyType/StrList.h"
# include "../../MyType/ListP.h"

# include "../../UtilKit/MemoryUtil.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$CipherText {
	SzI CalcCipherTextMinSize(StrList const & strList) {
		SzI sz;
		SzI chIdx;
		ListB<Bool> strExistParent;
		ListB<Bool> aStrExistOffspring;
		ListB<SzI> strInheritIdx;
		SzI aStrInheritIdx;
		{
			strExistParent.allocThenNew(strList._sz);
			strInheritIdx.allocThenNew(strList._sz);
		}
		sz = kSzZero;
		for_cnt(idx, kThFirst, strList._sz) {
			aStrExistOffspring.allocThenNew(strList[idx].getSz());
			for_cnt(idx2, static_cast<SzI>(idx + kThSecond), strList._sz)
				if (strList[idx].findDiff(strList[idx2], aStrInheritIdx))
					if ((aStrInheritIdx >= strInheritIdx[idx]) && (!strExistParent[idx2]) && (!aStrExistOffspring[aStrInheritIdx])) {
						strExistParent[idx2] = kTrue;
						strInheritIdx[idx2] = aStrInheritIdx;
						aStrExistOffspring[aStrInheritIdx] = kTrue;
					}
			aStrExistOffspring.tfree();
			if (strExistParent[idx]) {
				chIdx = strInheritIdx[idx];
				strExistParent[idx] = kFalse;
				strInheritIdx.reset(idx, kSzOne);
			} else {
				chIdx = kThFirst;
			}
			sz += strList[idx].getSz() - chIdx;
		}
		{
			strExistParent.tfree();
			strInheritIdx.tfree();
		}
		return szOf<UI32>(sz);
	}
}
