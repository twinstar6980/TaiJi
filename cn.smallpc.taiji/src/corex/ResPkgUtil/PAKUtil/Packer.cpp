# include "./Packer.h"

# include "./StructInfo.h"
# include "./Struct.h"

# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

namespace $TJ::$ResPkgUtil::$PAKUtil::$Packer {
	namespace $Pack {
		Void Pack(Byte * const & data, SzI & sz, $StructInfo::StructInfo const & structInfo, Path const & resDir) {
			$Struct::$Header::Member header;
			ListP<$Struct::ResInfo> resInfoList;
			sz = kSzZero;
			{
				header._magic = structInfo._magic;
				header._ver = structInfo._ver;
				sz += $DataBlockUtil::write(data + kThFirst, header).getSz();
			}
			{
				resInfoList.alloc(structInfo._res._sz);
				for_criter(res, structInfo._res) {
					auto & resInfo = resInfoList.add().getLast();
					res._path.getFullPath(resInfo._path._str);
					resInfo._info._time = res._time;
					sz += szOf<Byte>() + resInfo.getSz();
				}
			}
			sz += szOf<Byte>();
			{
				Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, resDir);
				for_cnt(idx, kThFirst, structInfo._res._sz) {
					$DevUtil::ReadData(structInfo._res[idx]._path, data + sz, resInfoList[idx]._info._sz);
					sz += resInfoList[idx]._info._sz;
				}
				$DevUtil::chgDirThenFree(parentDir);
			}
			{
				auto pos = header.getSz();
				for_criter(resInfo, resInfoList) {
					data[pos++] = $Struct::$EndFlag::kNext;
					pos += resInfo.write(data + pos).getSz();
				}
				data[pos++] = $Struct::$EndFlag::kStop;
			}
			return;
		}
	}
	namespace $UnPack {
		Void UnPack(Byte const * const & data, SzI const & sz, Path const & structInfoFile, Path const & resDir) {
			$StructInfo::StructInfo structInfo;
			$Struct::$Header::Member header;
			ListP<$Struct::ResInfo> resInfoList;
			auto pos = kThFirst;
			pos += $DataBlockUtil::read(data + pos, header).getSz();
			resInfoList.setNull();
			while_nonstop{
				auto shouldBreak = kFalse;
				switch (data[pos++]) {
					default: wErr("UnKnown Flag"); break;
					case $Struct::$EndFlag::kStop: shouldBreak = kTrue; break;
					case $Struct::$EndFlag::kNext: pos += resInfoList.add().getLast().read(data + pos).getSz(); break;
				}
				if (shouldBreak) break;
			}
			{
				structInfo._magic = header._magic;
				structInfo._ver = header._ver;
				structInfo._res.alloc(resInfoList._sz);
			}
			{
				Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, resDir);
				for_criter(resInfo, resInfoList) {
					auto & Res = structInfo._res.add().getLast();
					Res._path.read(resInfo._path._str);
					Res._time = resInfo._info._time;
					$DevUtil::WriteData(Res._path, data + pos, resInfo._info._sz);
					pos += resInfo._info._sz;
				}
				$DevUtil::chgDirThenFree(parentDir);
			}
			$JSON::write(structInfoFile, structInfo.write($JSON::Obj().setNull()));
			return;
		}
	}
}
