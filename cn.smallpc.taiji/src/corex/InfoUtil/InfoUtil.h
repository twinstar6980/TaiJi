# pragma once

# include "../MyType/Str.h"
# include "../MyType/StrList.h"
# include "../UtilKit/JSONUtil.h"
# include "../MyType/ListOfOther.h"
# include "../MyType/ListP.h"
# include "../MyType/ListB.h"
# include "../MyType/File.h"

# include "./ResPkgInfo_Ori.h"
# include "./ResPkgInfo.h"
# include "../ResPkgUtil/RSBUtil/StructInfo.h"
# include "../ResPkgUtil/RSGPUtil/StructInfo.h"
# include "../ResPkgUtil/DZUtil/StructInfo.h"
# include "../ResPkgUtil/PAKUtil/StructInfo.h"
# include "../ImageUtil/ImageInfoType.h"

namespace $TJ::$InfoUtil {
	namespace $Cnv {
		namespace $OriToResPkgInfo {
			$ResPkgInfo::ResPkgInfo & ParseInfo($ResPkgInfo_Ori::ResPkgInfo_Ori const & src, $ResPkgInfo::ResPkgInfo & dst);
		}
		namespace $ResPkgInfoToOri {
			$ResPkgInfo_Ori::ResPkgInfo_Ori & ParseInfo($ResPkgInfo::ResPkgInfo const & src, $ResPkgInfo_Ori::ResPkgInfo_Ori & dst, SzI const & slotBeginNum = kThFirst);
		}
		namespace $RSBResInfoListToResPkgInfo {
			$ResPkgInfo::ResPkgInfo & ParseInfo($ResPkgUtil::$RSBUtil::$ResInfoList::ResInfoList const & src, $ResPkgInfo::ResPkgInfo & dst);
		}
		namespace $ResPkgInfoToRSBStructInfo {
			$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & ParseInfo($ResPkgInfo::ResPkgInfo const & src, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & dst, $ImageUtil::TexFmtIdxList const & texFmtIdxList);
		}
		namespace $ResPkgInfoSubGroupToRSGPStructInfo {
			$ResPkgUtil::$RSGPUtil::$StructInfo::StructInfo & ParseInfo($ResPkgInfo::SubGroup const & src, $ResPkgUtil::$RSGPUtil::$StructInfo::StructInfo & dst);
		}
		namespace $ResPkgInfoToAtlasInfoList {
			ListOfInfo<$ImageUtil::AtlasInfo> & ParseInfo($ResPkgInfo::ResPkgInfo const & src, ListOfInfo<$ImageUtil::AtlasInfo> & dst);
		}
		namespace $RSBStructInfoToAtlasInfoList {
			ListOfInfo<$ImageUtil::AtlasInfo> & ParseInfo($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & src, ListOfInfo<$ImageUtil::AtlasInfo> & dst, $ImageUtil::TexFmtIdxList const & texFmtList);
		}
	}
	namespace $Cpy {
		tmpl_st(T) inline auto cpy($JSON::Obj const & src, $JSON::Obj & dst) {
			T().read(src).write(dst.setNull());
			return;
		}
		tmpl_st(T) inline auto cpy($JSON::Arr const & src, $JSON::Arr & dst) {
			T().read(src).write(dst.setNull());
			return;
		}
	}
	namespace $Spliter {
		Path const kStructFilePath("Struct.json");
		Str const kBaseInfoStr("BaseInfo");
		Str const kIncludeDirStr("Include");

		tmpl_st(T) constexpr auto IsDivisibleInfo() {
			return cmpT<T, $ResPkgInfo::ResPkgInfo>()
				|| cmpT<T, $ResPkgInfo::Group>()
				|| cmpT<T, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo>()
				|| cmpT<T, $ResPkgUtil::$RSBUtil::$StructInfo::Group>();
		}
		tmpl_st(T) constexpr auto IsMapItemSub() {
			return cmpT<T, MapItem<Str, $ResPkgInfo::Group>>()
				|| cmpT<T, MapItem<Str, $ResPkgInfo::SubGroup>>()
				|| cmpT<T, MapItem<Str, $ResPkgUtil::$RSBUtil::$StructInfo::Group>>()
				|| cmpT<T, MapItem<Str, $ResPkgUtil::$RSBUtil::$StructInfo::SubGroup>>();
		}

		inline auto & ReadBase($ResPkgInfo::ResPkgInfo & dst, $JSON::Obj const & src) {
			return dst;
		}
		inline auto & WriteBase($ResPkgInfo::ResPkgInfo const & src, $JSON::Obj & dst) {
			return dst;
		}
		inline auto & GetSub($ResPkgInfo::ResPkgInfo & src) { return src._group; }
		inline auto & GetSub($ResPkgInfo::ResPkgInfo const & src) { return src._group; }

		inline auto & ReadBase($ResPkgInfo::Group & dst, $JSON::Obj const & src) {
			src["IsComposite"].getX(dst._isComposite);
			return dst;
		}
		inline auto & WriteBase($ResPkgInfo::Group const & src, $JSON::Obj & dst) {
			dst.add("IsComposite", src._isComposite);
			return dst;
		}
		inline auto & GetSub($ResPkgInfo::Group & src) { return src._subGroup; }
		inline auto & GetSub($ResPkgInfo::Group const & src) { return src._subGroup; }

		inline auto & ReadBase($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & dst, $JSON::Obj const & src) {
			src["HeaderType"].getX(dst._headerType);
			src["SetResListInHeader"].getX(dst._setResInfoListInHeader);
			src["UseMoreAtlasInfo"].getX(dst._useMoreAtlasInfo);
			return dst;
		}
		inline auto & WriteBase($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & src, $JSON::Obj & dst) {
			dst.add("HeaderType", src._headerType);
			dst.add("SetResListInHeader", src._setResInfoListInHeader);
			dst.add("UseMoreAtlasInfo", src._useMoreAtlasInfo);
			return dst;
		}
		inline auto & GetSub($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & src) { return src._group; }
		inline auto & GetSub($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & src) { return src._group; }

		inline auto & ReadBase($ResPkgUtil::$RSBUtil::$StructInfo::Group & dst, $JSON::Obj const & src) {
			src["IsComposite"].getX(dst._isComposite);
			return dst;
		}
		inline auto & WriteBase($ResPkgUtil::$RSBUtil::$StructInfo::Group const & src, $JSON::Obj & dst) {
			dst.add("IsComposite", src._isComposite);
			return dst;
		}
		inline auto & GetSub($ResPkgUtil::$RSBUtil::$StructInfo::Group & src) { return src._subGroup; }
		inline auto & GetSub($ResPkgUtil::$RSBUtil::$StructInfo::Group const & src) { return src._subGroup; }

		tmpl_st(T) Void split(T const & src, Path const & dstDir) {
			ifcexp(!IsDivisibleInfo<T>()) $JSON::write(Path(dstDir).setSuffix("json"), src.write($JSON::Obj().setNull()));
			else {
			StrList subIDList;
			for_criter(e, GetSub(src)) {
				subIDList.add(e._key);
				split(e._val, Path(dstDir).add(kIncludeDirStr).add(e._key));
			}
			{
				$JSON::Obj data;
				WriteBase(src, data.addObj(kBaseInfoStr).last()._val.getObj());
				$JSON::setArrStr(subIDList, data.addArr(kIncludeDirStr).last()._val.getArr());
				$JSON::write(Path(dstDir).add(kStructFilePath), data);
			}
			subIDList.tfree();
			}
			return;
		}
		tmpl_st(T) T & cat(T & dst, Path const & srcDir) {
			ifcexp(!IsDivisibleInfo<T>()) dst.read($JSON::read(Path(srcDir).setSuffix("json"), $JSON::Obj().setNull()));
			else {
			StrList subIDList;
			{
				$JSON::Obj data;
				$JSON::read(Path(srcDir).add(kStructFilePath), data);
				ReadBase(dst, data[kBaseInfoStr].getObj());
				$JSON::getArrStr(data[kIncludeDirStr].getArr(), subIDList);
			}
			for_criter(e, subIDList)
				cat(GetSub(dst).addByKey(e).getLastVal(), Path(srcDir).add(kIncludeDirStr).add(e));
			}
			return dst;
		}
	}
	namespace $Lite {
		$ResPkgInfo::ResPkgInfo & Lite($ResPkgInfo::ResPkgInfo const & src, $ResPkgInfo::ResPkgInfo & dst, ListB<SzI> const & resIDList);
		$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & Lite($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo const & src, $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo & dst, ListB<SzI> const & resIDList);
	}
}
