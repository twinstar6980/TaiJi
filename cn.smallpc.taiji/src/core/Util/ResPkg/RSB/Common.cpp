#include "./Common.hpp"

namespace $TJ::$Util::$ResPkg::$RSB {

	namespace $SpecialStrBlock {

		extern Sz calcMinSz(List<Str> const & strList) {
			List<Bool> strExistParent;
			List<Sz> strInheritIdx;
			{
				strExistParent.allocThenUse(strList.$sz);
				strInheritIdx.allocThenUse(strList.$sz);
			}
			auto sz = kSzN;
			for (auto i = kNo1; i < strList.$sz; ++i) {
				auto const & e = strList[i];
				{
					List<Bool> aStrExistOffspring;
					aStrExistOffspring.allocThenUse(e.$sz + kSz1);
					for (auto j = static_cast<Sz>(i + kNo2); j < strList.$sz; ++j) {
						if (Sz aStrInheritIdx; e.range().findDiff(strList[j].range(), aStrInheritIdx)) {
							if (aStrInheritIdx >= strInheritIdx[i] && !strExistParent[j] && !aStrExistOffspring[aStrInheritIdx]) {
								strExistParent[j] = kBoT;
								strInheritIdx[j] = aStrInheritIdx;
								aStrExistOffspring[aStrInheritIdx] = kBoT;
							}
						}
					}
					aStrExistOffspring.free();
				}
				auto chIdx = kNo1;
				if (strExistParent[i]) {
					chIdx += strInheritIdx[i];
					strExistParent[i] = kBoF;
					strInheritIdx[i] = Sz();
				}
				sz += strList[i].$sz + kSz1 - chIdx;
			}
			{
				strExistParent.free();
				strInheritIdx.free();
			}
			return sz * kBlockSz;
		}

	}

}
