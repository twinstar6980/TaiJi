#pragma once

#include "../../../Type.hpp"
#include "../../../Type/List.hpp"
#include "../../../Type/Map.hpp"
#include "../../../Type/Str.hpp"
#include "../../../Type/JSON.hpp"
#include "../../../UserLiteral.hpp"

namespace $TJ::$Util::$ResPkg::$RSB {

	inline constexpr Sz kPaddingSz(0x1000);

	enum class ResType : IU8 { kNormal, kAtlas };

	namespace $BitOfResStoreMethod {

		inline constexpr IU32 kNone("00"_bin32);
		inline constexpr IU32 kCompressAtlasData("01"_bin32);
		inline constexpr IU32 kCompressNormalData("10"_bin32);

	}

	struct ResStoreMethod {

		Bool $compressNormalRes;
		Bool $compressAtlasRes;

		auto & set(Bool const & compressNormalRes, Bool const & compressAtlasRes) {
			$compressNormalRes = compressNormalRes;
			$compressAtlasRes = compressAtlasRes;
			return thiz;
		}

		auto & parseInt(IU32 const & t) {
			$compressNormalRes = t & $BitOfResStoreMethod::kCompressNormalData;
			$compressAtlasRes = t & $BitOfResStoreMethod::kCompressAtlasData;
			return thiz;
		}
		auto makeInt() const {
			auto r = $BitOfResStoreMethod::kNone;
			if ($compressNormalRes)
				r |= $BitOfResStoreMethod::kCompressNormalData;
			if ($compressAtlasRes)
				r |= $BitOfResStoreMethod::kCompressAtlasData;
			return r;
		}

		auto & read($JSON::Arr const & t) {
			t[kNo1].get($compressNormalRes);
			t[kNo2].get($compressAtlasRes);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2);
			t.add($compressNormalRes).add($compressAtlasRes);
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

		auto operator==(ResStoreMethod const & t) const {
			return $compressNormalRes == t.$compressNormalRes
				&& $compressAtlasRes == t.$compressAtlasRes;
		}
		auto operator!=(ResStoreMethod const & t) const { return !operator==(t); }

	};

	using CID128 = Ch[128];

	inline Str const kSuffixOfCompositeShell("_CompositeShell"_r);
	inline Str const kSuffixOfAutoPool("_AutoPool"_r);

	inline auto setCID128(Str const & src, CID128 & dst) {
		Range(&*dst, src.$sz).cpy(src.begin());
		Range(&*dst + src.$sz, dst + 128).set($Ch::kNull);
		return;
	}

	inline constexpr Sz kCategoryResNull(0);

	inline Str const kCategoryLocNull(""_r);

	inline constexpr FourCC kCategoryLocNullCode(makeFourCC('\0', '\0', '\0', '\0'));

	struct Category {

		Sz $res;
		Str $loc;

		auto isNullRes() const {
			return $res == kCategoryResNull;
		}
		auto isNullLoc() const {
			return $loc == kCategoryLocNull;
		}

		auto & setNull() {
			$res = kCategoryResNull;
			$loc = kCategoryLocNull;
			return thiz;
		}

		auto & parse(Sz const & res, FourCC const & loc) {
			$res = res;
			if (loc == kCategoryLocNullCode)
				$loc.alloc().setEmpty();
			else
				$loc.cpy(reinterpret_cast<Ch const *>(&crv(vRevEndian(loc))), kTSz<FourCC>);
			return thiz;
		}
		auto & parseTo(Sz & res, FourCC & loc) const {
			res = $res;
			if (isNullLoc())
				loc = kCategoryLocNullCode;
			else
				Range(reinterpret_cast<Ch *>(&loc), kTSz<FourCC>).cpy($loc.begin());
			return thiz;
		}
		auto & parseRaw(Str const & res, Str const & loc) {
			setNull();
			if (!res.isNull())
				$Str::scanFmt(res, "%u", &$res);
			if (!loc.isNull() && !loc.isEmpty())
				$loc = loc;
			return thiz;
		}
		auto & parseToRaw(Str & res, Str & loc) const {
			if (isNullRes())
				res.setNull();
			else
				res = $Str::printFmt("%u", $res);
			if (isNullLoc())
				loc.setNull();
			else
				loc = $loc;
			return thiz;
		}

		auto & read($JSON::Arr const & t) {
			t[kNo1].get($res, kCategoryResNull);
			t[kNo2].get($loc, kCategoryLocNull);
			return thiz;
		}
		auto & write($JSON::Arr & t) const {
			t.alloc(2);
			t.add($res, isNullRes()).add($loc, isNullLoc());
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setArr().getArr());
		}

	};

	inline auto cnvStdGroupID(CID128 const & src, Str & dst) {
		Str srcStr;
		refCStr(srcStr, src);
		Bool isSimple;
		if (isSimple = $Str::endWith(srcStr, kSuffixOfCompositeShell)) {
			dst.cpy(Str().ref(srcStr, kNo1, srcStr.$sz - kSuffixOfCompositeShell.$sz));
		} else {
			dst.cpy(srcStr);
		}
		srcStr.unref();
		return isSimple;
	}

	namespace $SpecialStrBlock {

		inline constexpr Sz kBlockSz = kTSz<IU32>;

		template <typename T>
		auto enc(Map<Str, T> const & t, ByteSection & data) {
			assert(data.isEmpty());
			ByteStream stm;
			stm.ref(data.$data, data.$cap).setPosBeg();
			List<Bool> strExistParent;
			List<Sz> strInheritIdx;
			List<Sz> parentStrOfs;
			{
				strExistParent.allocThenUse(t.$sz);
				strInheritIdx.allocThenUse(t.$sz);
				parentStrOfs.allocThenUse(t.$sz);
			}
			for (auto i = kNo1; i < t.$sz; ++i) {
				auto & e = t.get(i);
				{
					List<Bool> aStrExistOffspring;
					aStrExistOffspring.allocThenUse(e.$k.$sz + kSz1);
					for (auto j = static_cast<Sz>(i + kNo2); j < t.$sz; ++j) {
						if (Sz aStrInheritIdx; e.$k.range().findDiff(t.get(j).$k.range(), aStrInheritIdx)) {
							if (aStrInheritIdx >= strInheritIdx[i] && !strExistParent[j] && !aStrExistOffspring[aStrInheritIdx]) {
								strExistParent[j] = kBoT;
								strInheritIdx[j] = aStrInheritIdx;
								aStrExistOffspring[aStrInheritIdx] = kBoT;
								parentStrOfs[j] = stm.$pos / kBlockSz + (aStrInheritIdx - strInheritIdx[i]);
							}
						}
					}
					aStrExistOffspring.free();
				}
				auto chIdx = kNo1;
				if (strExistParent[i]) {
					chIdx += strInheritIdx[i];
					auto curPos = stm.$pos;
					stm.setPosBeg(parentStrOfs[i] * kBlockSz).rd<IU32>() |= (curPos / kBlockSz) << 8;
					stm.setPosBeg(curPos);
					strExistParent[i] = kBoF;
					strInheritIdx[i] = Sz();
				}
				while (chIdx < e.$k.$sz)
					stm << e.$k[chIdx++] << kByteNull << kByteNull << kByteNull;
				stm << $Ch::kNull << kByteNull << kByteNull << kByteNull;
				e.$v.write(stm);
			}
			{
				strExistParent.free();
				strInheritIdx.free();
				parentStrOfs.free();
			}
			data.setSz(stm.$pos);
			stm.unref();
			return;
		}

		template <typename T>
		auto dec(Map<Str, T> & t, ByteSection const & data) {
			ByteStream stm;
			stm.ref(data).setPosBeg();
			t.alloc(data.$sz / kBlockSz);
			Map<Bool, Str> linkInfo;
			linkInfo.allocThenUse(t.$cap);
			while (!stm.eos()) {
				auto & e = t.appendX().last();
				auto strSzInNextStm = kSzN;
				{
					auto curPos = stm.$pos;
					while (Ch(stm.rd<IU32>() & "11111111"_bin8) != $Ch::kNull)
						++strSzInNextStm;
					stm.setPosBeg(curPos);
				}
				if (linkInfo.get(stm.$pos / kBlockSz).$k) {
					e.$k.alloc(linkInfo.get(stm.$pos / kBlockSz).$v.$sz + strSzInNextStm).append(linkInfo.get(stm.$pos / kBlockSz).$v);
					linkInfo.get(stm.$pos / kBlockSz).$k = kBoF;
					linkInfo.get(stm.$pos / kBlockSz).$v.unref();
				} else {
					e.$k.alloc(strSzInNextStm);
				}
				while (kBoT) {
					auto curI32 = stm.rd<IU32>();
					Ch oneChOfStr = curI32 & "11111111"_bin8;
					if (IU32 linkStrOfs(curI32 >> 8); linkStrOfs != 0) {
						linkInfo.get(linkStrOfs).$k = kBoT;
						linkInfo.get(linkStrOfs).$v.ref(e.$k);
					}
					if (oneChOfStr == $Ch::kNull) {
						e.$v.read(stm);
						break;
					} else {
						e.$k.append(oneChOfStr);
					}
				}
			}
			linkInfo.free();
			t.lite();
			stm.unref();
			return;
		}

		extern Sz calcMinSz(List<Str> const & strList);

	}

}
