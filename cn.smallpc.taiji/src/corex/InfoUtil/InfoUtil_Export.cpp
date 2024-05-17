# pragma once

# include "./InfoUtil_Export.h"

# include "./ResPkgInfo_Ori.h"
# include "./ResPkgInfo.h"

# include "../ImageUtil/ImageInfoType.h"

namespace $TJ::$InfoUtil {
	namespace $Cnv {
		Void expCnv(Str const & srcInfoType, Str const & dstInfoType, Path const & srcInfoFile, Path const & dstInfoFile, Path const & appendInfoFile) {
			$JSON::Val srcInfo, dstInfo, appendInfo;
			$JSON::read(srcInfoFile, srcInfo);
			if (srcInfoType.cmp("ResPkgInfo_Ori")) {
				if (dstInfoType.cmp("ResPkgInfo")) {
					$OriToResPkgInfo::ParseInfo($ResPkgInfo_Ori::ResPkgInfo_Ori().read(srcInfo.getObj()), $ResPkgInfo::ResPkgInfo().getThis()).write(dstInfo.setObj().getObj());
				}
			} elif(srcInfoType.cmp("ResPkgInfo")) {
				if (dstInfoType.cmp("ResPkgInfo_Ori")) {
					$ResPkgInfoToOri::ParseInfo($ResPkgInfo::ResPkgInfo().read(srcInfo.getObj()), $ResPkgInfo_Ori::ResPkgInfo_Ori().getThis()).write(dstInfo.setObj().getObj());
				} elif(dstInfoType.cmp("RSBStructInfo")) {
					$JSON::read(appendInfoFile, appendInfo);
					$ResPkgInfoToRSBStructInfo::ParseInfo($ResPkgInfo::ResPkgInfo().read(srcInfo.getObj()), $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().getThis(), $ImageUtil::TexFmtIdxList().read(appendInfo.getArr())).write(dstInfo.setObj().getObj());
				} elif(dstInfoType.cmp("AtlasInfoList")) {
					$ResPkgInfoToAtlasInfoList::ParseInfo($ResPkgInfo::ResPkgInfo().read(srcInfo.getObj()), ListOfInfo<$ImageUtil::AtlasInfo>().setNull()).write(dstInfo.setArr().getArr());
				}
			} elif(srcInfoType.cmp("RSBResInfoList")) {
				if (dstInfoType.cmp("ResPkgInfo")) {
					//$RSBResInfoListToResPkgInfo::ParseInfo($ResPkgUtil::$RSBUtil::$ResInfoList::ResInfoList().read(srcInfo.getObj()), $ResPkgInfo::ResPkgInfo().getThis()).write(dstInfo.setObj().getObj());
				}
			} elif(srcInfoType.cmp("ResPkgInfo_SubGroup")) {
				if (dstInfoType.cmp("RSGPStructInfo")) {
					$ResPkgInfoSubGroupToRSGPStructInfo::ParseInfo($ResPkgInfo::SubGroup().read(srcInfo.getObj()), $ResPkgUtil::$RSGPUtil::$StructInfo::StructInfo().getThis()).write(dstInfo.setObj().getObj());
				}
			} elif(srcInfoType.cmp("RSBStructInfo")) {
				if (dstInfoType.cmp("AtlasInfoList")) {
					$JSON::read(appendInfoFile, appendInfo);
					$RSBStructInfoToAtlasInfoList::ParseInfo($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().read(srcInfo.getObj()), ListOfInfo<$ImageUtil::AtlasInfo>().setNull(), $ImageUtil::TexFmtIdxList().read(appendInfo.getArr())).write(dstInfo.setArr().getArr());
				}
			}
			$JSON::write(dstInfoFile, dstInfo);
			return;
		}
	}
	namespace $Cpy {
		Void expCpy(Str const & infoType, Path const & srcFile, Path const & dstFile) {
			$JSON::Val srcInfo, dstInfo;
			$JSON::read(srcFile, srcInfo);
			goif(infoType.cmp("ResPkgInfo_Ori"))
				cpy<$ResPkgInfo_Ori::ResPkgInfo_Ori>(srcInfo.getObj(), dstInfo.setObj().getObj());
			elif(infoType.cmp("ResPkgInfo"))
				cpy<$ResPkgInfo::ResPkgInfo>(srcInfo.getObj(), dstInfo.setObj().getObj());
			elif(infoType.cmp("RSGPStructInfo"))
				cpy<$ResPkgUtil::$RSGPUtil::$StructInfo::StructInfo>(srcInfo.getObj(), dstInfo.setObj().getObj());
			elif(infoType.cmp("RSBStructInfo"))
				cpy<$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo>(srcInfo.getObj(), dstInfo.setObj().getObj());
			elif(infoType.cmp("DZStructInfo"))
				cpy<$ResPkgUtil::$DZUtil::$StructInfo::StructInfo>(srcInfo.getObj(), dstInfo.setObj().getObj());
			elif(infoType.cmp("PAKStructInfo"))
				cpy<$ResPkgUtil::$PAKUtil::$StructInfo::StructInfo>(srcInfo.getObj(), dstInfo.setObj().getObj());
			elif(infoType.cmp("AtlasInfoList"))
				cpy<ListOfInfo<$ImageUtil::AtlasInfo>>(srcInfo.getArr(), dstInfo.setArr().getArr());
			else wErr("UnKnown InfoType");
			$JSON::write(dstFile, dstInfo);
			return;
		}
	}
	namespace $Spliter {
		Void expSplit(Str const & infoType, Path const & srcFile, Path const & dstDir) {
			$JSON::Obj src;
			$JSON::read(srcFile, src);
			if (infoType.cmp("ResPkgInfo"))
				split($ResPkgInfo::ResPkgInfo().read(src), dstDir);
			elif(infoType.cmp("RSBStructInfo"))
				split($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().read(src), dstDir);
			else wErr("UnKnown InfoType");
		}
		Void expCat(Str const & infoType, Path const & dstFile, Path const & srcDir) {
			$JSON::Obj dst;
			goif(infoType.cmp("ResPkgInfo"))
				cat($ResPkgInfo::ResPkgInfo().getThis(), srcDir).write(dst.setNull());
			elif(infoType.cmp("RSBStructInfo"))
				cat($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().getThis(), srcDir).write(dst.setNull());
			else wErr("UnKnown InfoType");
			$JSON::write(dstFile, dst);
		}
	}
	namespace $Lite {
		Void expLite(Str const & infoType, Path const & srcFile, Path const & dstFile, ListB<SzI> const & resIDList) {
			$JSON::Obj srcInfo, dstInfo;
			$JSON::read(srcFile, srcInfo);
			goif(infoType.cmp("ResPkgInfo"))
				Lite($ResPkgInfo::ResPkgInfo().read(srcInfo), $ResPkgInfo::ResPkgInfo().getThis(), resIDList).write(dstInfo.setNull());
			elif(infoType.cmp("RSBStructInfo"))
				Lite($ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().read(srcInfo), $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().getThis(), resIDList).write(dstInfo.setNull());
			$JSON::write(dstFile, dstInfo);
			return;
		}
	}
}
