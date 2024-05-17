#pragma once

#include "../../Type/Str.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/File.hpp"
#include "../../Type/InfoType.hpp"

#include "./RSBInfoX.hpp"
#include "./RSBInfoXOri.hpp"
#include "../ResPkg/RSB/Info.hpp"
#include "../ResPkg/RSB/RSGP/Info.hpp"
#include "../ResPkg/DZ/Info.hpp"
#include "../ResPkg/PAK/Info.hpp"
#include "../Image/InfoType.hpp"

namespace $TJ::$Util::$Info {

	namespace $Purify {

		template <typename T>
		inline auto purify($JSON::Val const & src, $JSON::Val & dst) {
			T().read(src).write(dst);
			return;
		}

		inline Void purifyExport(Str const & type, Path const & srcFile, Path const & dstFile) {
			$JSON::Val srcInfo, dstInfo;
			$JSON::read(srcFile, srcInfo);
			switch (type.hashByPtr(operator""_sh)) {
				default:
					throw VAMGException("Info::purify", "unknown info-type");
					break;
				case "RSBInfoXOri"_sh:
					purify<$RSBInfoXOri::Info>(srcInfo, dstInfo);
					break;
				case "RSBInfoX"_sh:
					purify<$RSBInfoX::Info>(srcInfo, dstInfo);
					break;
				case "RSBInfo"_sh:
					purify<$ResPkg::$RSB::$Info::Info>(srcInfo, dstInfo);
					break;
				case "RSGPInfo"_sh:
					purify<$ResPkg::$RSB::$RSGP::$Info::Info>(srcInfo, dstInfo);
					break;
				case "AtlasInfoList"_sh:
					purify<InfoList<$Image::AtlasInfo>>(srcInfo, dstInfo);
					break;
			}
			$JSON::write(dstFile, dstInfo);
			return;
		}

	}

	namespace $Spliter {

		template <typename T>
		inline constexpr auto isDivisible() {
			return kTSame<T, $RSBInfoX::Info>
				|| kTSame<T, $RSBInfoX::Group>
				|| kTSame<T, $RSBInfoX::SubGroup>
				|| kTSame<T, $ResPkg::$RSB::$Info::Info>
				|| kTSame<T, $ResPkg::$RSB::$Info::Group>;
		}

		template <typename T>
		inline auto & readBase($JSON::Obj const & src, T & dst) {
			if constexpr (kTSame<TDecay<T>, $RSBInfoX::Info>) {
			} else if constexpr (kTSame<TDecay<T>, $RSBInfoX::Group>) {
				src["composite"_r].get(dst.$composite);
			} else if constexpr (kTSame<TDecay<T>, $RSBInfoX::SubGroup>) {
				dst.$category.read(src["category"_r]);
			} else if constexpr (kTSame<TDecay<T>, $ResPkg::$RSB::$Info::Info>) {
				src["headerType"_r].get(dst.$headerType);
				src["enableEmbeddedResInfo"_r].get(dst.$enableEmbeddedResInfo);
				src["enableAtlasInfoExpand"_r].get(dst.$enableAtlasInfoExpand);
			} else if constexpr (kTSame<TDecay<T>, $ResPkg::$RSB::$Info::Group>) {
				src["composite"_r].get(dst.$composite);
			} else {
				static_assert_failed("error type");
			}
			return dst;
		}
		template <typename T>
		inline auto & writeBase(T const & src, $JSON::Obj & dst) {
			dst.alloc();
			if constexpr (kTSame<TDecay<T>, $RSBInfoX::Info>) {
			} else if constexpr (kTSame<TDecay<T>, $RSBInfoX::Group>) {
				dst("composite"_r).set(src.$composite);
			} else if constexpr (kTSame<TDecay<T>, $RSBInfoX::SubGroup>) {
				src.$category.write(dst("category"_r));
			} else if constexpr (kTSame<TDecay<T>, $ResPkg::$RSB::$Info::Info>) {
				dst("headerType"_r).set(src.$headerType);
				dst("enableEmbeddedResInfo"_r).set(src.$enableEmbeddedResInfo);
				dst("enableAtlasInfoExpand"_r).set(src.$enableAtlasInfoExpand);
			} else if constexpr (kTSame<TDecay<T>, $ResPkg::$RSB::$Info::Group>) {
				dst("composite"_r).set(src.$composite);
			} else {
				static_assert_failed("error type");
			}
			return dst;
		}

		template <typename T>
		inline auto & getSub(T & t) {
			if constexpr (kTSame<TDecay<T>, $RSBInfoX::Info>) {
				return t.$group;
			} else if constexpr (kTSame<TDecay<T>, $RSBInfoX::Group>) {
				return t.$subGroup;
			} else if constexpr (kTSame<TDecay<T>, $RSBInfoX::SubGroup>) {
				return t.$res;
			} else if constexpr (kTSame<TDecay<T>, $ResPkg::$RSB::$Info::Info>) {
				return t.$group;
			} else if constexpr (kTSame<TDecay<T>, $ResPkg::$RSB::$Info::Group>) {
				return t.$subGroup;
			} else {
				static_assert_failed("error type");
			}
		}

		template <typename T>
		inline auto & readBase($JSON::Val const & src, T & dst) {
			return readBase(src.getObj(), dst);
		}
		template <typename T>
		inline auto & writeBase(T const & src, $JSON::Val & dst) {
			return writeBase(src, dst.setObj().getObj());
		}

		template <typename TUntil, typename T>
		Void split(T const & src, Path const & dstDir) {
			if constexpr (kTSame<TUntil, T>) {
				$JSON::write(Path(dstDir).setExtension("json"_r), src.write(crv($JSON::Val())));
			} else {
				if constexpr (!isDivisible<T>()) {
					static_assert_failed("!divisible");
				}
				List<Str> subIDList(getSub(src).$sz);
				for (auto const & e : getSub(src)) {
					subIDList.append(e.$k);
					split<TUntil>(e.$v, dstDir + "inc"_r + e.$k);
				}
				{
					$JSON::Obj data;
					data.alloc(kSz2);
					writeBase(src, data("base"_r));
					$JSON::setListToArr(subIDList, data("inc"_r));
					$JSON::write(dstDir + "info.json"_r, data);
				}
				subIDList.free();
			}
			return;
		}
		template <typename TUntil, typename T>
		T & cat(T & dst, Path const & srcDir) {
			if constexpr (kTSame<TUntil, T>) {
				dst.read($JSON::read(Path(srcDir).setExtension("json"_r), crv($JSON::Val())));
			} else {
				if constexpr (!isDivisible<T>()) {
					static_assert_failed("!divisible");
				}
				List<Str> subIDList;
				{
					$JSON::Obj data;
					$JSON::read(srcDir + "info.json"_r, data);
					readBase(data["base"_r], dst);
					$JSON::getListFromArr(data["inc"_r], subIDList);
				}
				getSub(dst).alloc(subIDList.$sz);
				for (auto const & e : subIDList)
					cat<TUntil>(getSub(dst)(e), srcDir + "inc"_r + e);
			}
			return dst;
		}

		inline Void splitExport(Str const & type, Str const & until, Path const & srcFile, Path const & dstDir) {
			$JSON::Obj src;
			$JSON::read(srcFile, src);
			switch ((type + "|"_r + until).hashByPtr(operator""_sh)) {
				default:
					throw VAMGException("Info::Spliter::cat", "unknown info-type");
					break;
				case "RSBInfoX|Group"_sh:
					split<$RSBInfoX::Group>($RSBInfoX::Info().read(src), dstDir);
					break;
				case "RSBInfoX|SubGroup"_sh:
					split<$RSBInfoX::SubGroup>($RSBInfoX::Info().read(src), dstDir);
					break;
				case "RSBInfoX|Res"_sh:
					split<$RSBInfoX::Res>($RSBInfoX::Info().read(src), dstDir);
					break;
				case "RSBInfo|Group"_sh:
					split<$ResPkg::$RSB::$Info::Group>($ResPkg::$RSB::$Info::Info().read(src), dstDir);
					break;
				case "RSBInfo|SubGroup"_sh:
					split<$ResPkg::$RSB::$Info::SubGroup>($ResPkg::$RSB::$Info::Info().read(src), dstDir);
					break;
			}
			return;
		}
		inline Void catExport(Str const & type, Str const & untilType, Path const & srcDir, Path const & dstFile) {
			$JSON::Obj dst;
			switch ((type + "|"_r + untilType).hashByPtr(operator""_sh)) {
				default:
					throw VAMGException("Info::Spliter::cat", "unknown info-type");
					break;
				case "RSBInfoX|Group"_sh:
					cat<$RSBInfoX::Group>(crv($RSBInfoX::Info()), srcDir).write(dst);
					break;
				case "RSBInfoX|SubGroup"_sh:
					cat<$RSBInfoX::SubGroup>(crv($RSBInfoX::Info()), srcDir).write(dst);
					break;
				case "RSBInfoX|Res"_sh:
					cat<$RSBInfoX::Res>(crv($RSBInfoX::Info()), srcDir).write(dst);
					break;
				case "RSBInfo|Group"_sh:
					cat<$ResPkg::$RSB::$Info::Group>(crv($ResPkg::$RSB::$Info::Info()), srcDir).write(dst);
					break;
				case "RSBInfo|SubGroup"_sh:
					cat<$ResPkg::$RSB::$Info::SubGroup>(crv($ResPkg::$RSB::$Info::Info()), srcDir).write(dst);
					break;
			}
			$JSON::write(dstFile, dst);
			return;
		}

	}

	namespace $Cnv {

		$RSBInfoX::Info & cnv($RSBInfoXOri::Info const & src, $RSBInfoX::Info & dst);
		$RSBInfoXOri::Info & cnv($RSBInfoX::Info const & src, $RSBInfoXOri::Info & dst, Sz const & slotBegNum);
		$ResPkg::$RSB::$Info::Info & cnv($RSBInfoX::Info const & src, $ResPkg::$RSB::$Info::Info & dst, $Image::TexFmtIdxMap const & texFmtIdx);
		$ResPkg::$RSB::$RSGP::$Info::Info & cnv($RSBInfoX::SubGroup const & src, $ResPkg::$RSB::$RSGP::$Info::Info & dst);
		InfoList<$Image::AtlasInfo> & cnv($RSBInfoX::Info const & src, InfoList<$Image::AtlasInfo> & dst);
		InfoList<$Image::AtlasInfo> & cnv($ResPkg::$RSB::$Info::Info const & src, InfoList<$Image::AtlasInfo> & dst, $Image::TexFmtIdxMap const & texFmtIdx);

		inline Void cnvExport(Str const & srcType, Str const & dstType, Path const & srcFile, Path const & dstFile, Path const & extraFile) {
			$JSON::Val src, dst, extra;
			$JSON::read(srcFile, src);
			switch ((srcType + "|"_r + dstType).hashByPtr(operator""_sh)) {
				default:
					break;
				case "RSBInfoXOri|RSBInfoX"_sh:
					cnv($RSBInfoXOri::Info().read(src), crv($RSBInfoX::Info())).write(dst);
					break;
				case "RSBInfoX|RSBInfoXOri"_sh:
					cnv($RSBInfoX::Info().read(src), crv($RSBInfoXOri::Info()), kNo1).write(dst);
					break;
				case "RSBInfoX|RSBInfo"_sh:
					$JSON::read(extraFile, extra);
					cnv($RSBInfoX::Info().read(src), crv($ResPkg::$RSB::$Info::Info()), $Image::TexFmtIdxMap().read(extra)).write(dst);
					break;
				case "RSBInfoX|AtlasInfoList"_sh:
					cnv($RSBInfoX::Info().read(src), crv(InfoList<$Image::AtlasInfo>())).write(dst);
					break;
				case "RSBInfo|AtlasInfoList"_sh:
					$JSON::read(extraFile, extra);
					cnv($ResPkg::$RSB::$Info::Info().read(src), crv(InfoList<$Image::AtlasInfo>()), $Image::TexFmtIdxMap().read(extra)).write(dst);
					break;
				case "RSBInfoX:SubGroup|RSGPInfo"_sh:
					cnv($RSBInfoX::SubGroup().read(src), crv($ResPkg::$RSB::$RSGP::$Info::Info())).write(dst);
					break;
			}
			$JSON::write(dstFile, dst);
			return;
		}

	}

}
