# include "./Packer.h"

# include "./StructInfo.h"
# include "./Struct.h"
# include "../../MyType/StrList.h"

# include "../../Const.h"
# include "../../UtilKit/PtrUtil.h"
# include "../../UtilKit/MemoryUtil.h"
# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"
# include "../../DataUtil/Compressor/LZMA.h"

namespace $TJ::$ResPkgUtil::$DZUtil::$Packer {
	namespace $Pack {
		Void Pack(Byte * const & data, SzI & sz, $StructInfo::StructInfo const & structInfo, Path const & resDir) {
			ListOfBlock<$Struct::$ResPathInfo::Member> resPathInfoList;
			ListOfBlock<$Struct::$ResDataInfo::Member> resDataInfoList;
			StrList resDirList, resNameList;
			{
				resDirList.alloc(kSzOne + structInfo._res._sz).add(kEmptyStr);
				resNameList.alloc(structInfo._res._sz);
				resPathInfoList.alloc(structInfo._res._sz);
				for_criter(res, structInfo._res) {
					auto & resPathInfo = resPathInfoList.add().getLast();
					SzI resDirIdx;
					if (resDirList.find(res._path.getDirPath(Str().getThis()), resDirIdx)) {
						resPathInfo._dirIdx = static_cast<UI16>(resDirIdx);
					} else {
						resDirList.add(res._path.getDirPath(Str().getThis()));
						resPathInfo._dirIdx = static_cast<UI16>(resDirList.lastIdx());
					}
					resNameList.add(res._path.getFilePath());
					resPathInfo._nameIdx = static_cast<UI16>(resNameList.lastIdx());
					resPathInfo._end = $Struct::$ResPathInfo::$MemberConst::kEnd;
				}
				resDirList.eraseSurplusCap();
			}
			sz = kSzZero;
			{
				$Struct::$Header::Member header;
				header._id = $Struct::$Header::$MemberConst::kID;
				header._dirSum = static_cast<UI16>(resDirList._sz);
				header._nameSum = static_cast<UI16>(resNameList._sz);
				sz += $DataBlockUtil::write(data + sz, header).getSz();
			}
			{
				memSet(data + sz, kSzOne);
				sz += kSzOne;
			}
			sz += static_cast<SzI>(resNameList.write(data + sz).getStrSz());
			sz += static_cast<SzI>(resDirList.write(data + sz, kThSecond).getStrSz(kThSecond));
			sz += $DataBlockUtil::writeArr(data + sz, resPathInfoList).getSz();
			{
				$Struct::$UnKnownBlock::Member unKnownBlock;
				unKnownBlock._unKnown_1 = $Struct::$UnKnownBlock::$MemberConst::kUnKnown_1;
				unKnownBlock._unKnown_e = static_cast<UI16>(structInfo._res._sz);
				sz += $DataBlockUtil::write(data + sz, unKnownBlock).getSz();
			}
			{
				Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, resDir);
				SzI resDataPos = sz + ($Struct::$ResDataInfo::kBlockSize * resPathInfoList._sz); // maybe use list.getsize() ?
				resDataInfoList.alloc(structInfo._res._sz);
				for_criter(res, structInfo._res) {
					auto & resDataInfo = resDataInfoList.add().getLast();
					resDataInfo._flag = 0x100; // TODO force set flag = 0x100
					switch (resDataInfo._flag) {
						default: wErr("UnKnown Format"); break;
						case 0x100:
							resDataInfo._ofs = resDataPos;
							$DevUtil::ReadData(res._path, data + resDataPos, resDataInfo._sz);
							resDataInfo._sz2 = resDataInfo._sz;
							resDataPos += resDataInfo._sz;
							break;
						case 0x200: // TODO lzma compress
							break;
						case 0x8: // TODO unknown format
							break;
					}
				}
				$DataBlockUtil::writeArr(data + sz, resDataInfoList);
				sz = resDataPos;
				$DevUtil::chgDirThenFree(parentDir);
			}
			resPathInfoList.tfree(); resDataInfoList.tfree();
			resDirList.tfree(); resNameList.tfree();
			return;
		}
	}
	namespace $UnPack {
		static auto OutPutResFile(Byte const * const & resData, ListP<$Struct::$ResPathInfo::Member> const & resPathInfoList, ListP<$Struct::$ResDataInfo::Member> const & resDataInfoList, StrList const & resDirList, StrList const & resNameList, Path const & dstDir) {
			Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, dstDir);
			for_cnt(idx, kThFirst, resPathInfoList._sz) {
				auto & resPathInfo = resPathInfoList[idx];
				auto & resDataInfo = resDataInfoList[idx];
				Path resPath;
				if (!resDirList[resPathInfo._dirIdx].isEmpty())
					resPath.read(resDirList[resPathInfo._dirIdx]);
				resPath.add(resNameList[resPathInfo._nameIdx]);
				switch (resDataInfo._flag) {
					default: wErr("UnKnown Format = 0x%08X", resDataInfo._flag); break;
					case 0x100:
						$DevUtil::WriteData(resPath, resData + resDataInfo._ofs, resDataInfo._sz);
						break;
					case 0x200: {
						$Struct::$CompressHeader_512::Member compressHeader;
						Byte * tmpData(kNullPtr);
						newPtr(tmpData, resDataInfo._sz);
						$DataBlockUtil::read(resData + resDataInfo._ofs + kThFirst, compressHeader);
						$DataUtil::$Compressor::$LZMA::UnCompress(compressHeader._props, resData + resDataInfo._ofs + compressHeader.getSz(), $DataUtil::$Compressor::$LZMA::kSrcSz_IgnoreActualSz, tmpData, resDataInfo._sz);
						$DevUtil::WriteData(resPath, tmpData, resDataInfo._sz);
						delPtrArr(tmpData);
						break;
					}
					case 0x8: {
						wWarn("Format Is 0x8");
						// unknown flag with LZMA, found on ver.cn, non-found props
						$Struct::$CompressHeader_8::Member compressHeader;
						Byte * tmpData(kNullPtr);
						newPtr(tmpData, resDataInfo._sz);
						$DataBlockUtil::read(resData + resDataInfo._ofs + kThFirst, compressHeader);
						//$DataUtil::$LZMACompressor::UnCompress(compressHeader._props, resData + resDataInfo._ofs + compressHeader.getSz(), $DataUtil::$LZMACompressor::kSrcSize_IgnoreActualSize, tmpData, resDataInfo._sz);
						$DevUtil::WriteData(resPath, tmpData, resDataInfo._sz);
						delPtrArr(tmpData);
						break;
					}
				}
				resPath.tfree();
			}
			$DevUtil::chgDirThenFree(parentDir);
			return;
		}
		static auto & ParseInfo($StructInfo::StructInfo & structInfo, ListP<$Struct::$ResPathInfo::Member> const & resPathInfoList, StrList const & resDirList, StrList const & resNameList) {
			structInfo._res.alloc(resPathInfoList._sz);
			for_criter(resPathInfo, resPathInfoList) {
				auto & res = structInfo._res.add().getLast();
				if (!resDirList[resPathInfo._dirIdx].isEmpty())
					res._path.read(resDirList[resPathInfo._dirIdx]);
				res._path.add(resNameList[resPathInfo._nameIdx]);
			}
			return structInfo;
		}
		static auto Analysis(Byte const * const & data, $Struct::$Header::Member & header, ListOfBlock<$Struct::$ResPathInfo::Member> & resPathInfoList, ListOfBlock<$Struct::$ResDataInfo::Member> & resDataInfoList, StrList & resDirList, StrList & resNameList) {
			auto pos = kThFirst;
			pos += $DataBlockUtil::read(data + pos, header).getSz();
			pos += kThSecond;
			pos += resNameList.alloc(header._nameSum).read(data + pos).getStrSz();
			pos += resDirList.alloc(header._dirSum).add(kEmptyStr).read(data + pos).getStrSz(kThSecond);
			pos += $DataBlockUtil::readArr(data + pos, resPathInfoList.allocThenNew(header._nameSum)).getSz();
			pos += kSzFour;
			pos += $DataBlockUtil::readArr(data + pos, resDataInfoList.allocThenNew(header._nameSum)).getSz();
			return;
		}
		Void UnPack(Byte const * const & data, SzI const & sz, Path const & structInfoFile, Path const & resDir) {
			ListOfBlock<$Struct::$ResPathInfo::Member> resPathInfoList;
			ListOfBlock<$Struct::$ResDataInfo::Member> resDataInfoList;
			StrList resDirList, resNameList;
			{
				$Struct::$Header::Member header;
				Analysis(data + kThFirst, header, resPathInfoList, resDataInfoList, resDirList, resNameList);
				$JSON::write(structInfoFile, ParseInfo($StructInfo::StructInfo().getThis(), resPathInfoList, resDirList, resNameList).write($JSON::Obj().setNull()));
				OutPutResFile(data + kThFirst, resPathInfoList, resDataInfoList, resDirList, resNameList, resDir);
			}
			resPathInfoList.tfree(); resDataInfoList.tfree();
			resDirList.tfree(); resNameList.tfree();
			return;
		}
	}
}
