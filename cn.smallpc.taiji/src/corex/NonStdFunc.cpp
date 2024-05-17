# include "./NonStdFunc.h"

# include "./InfoUtil/InfoUtil.h"

# include "./UtilKit/PBVarIntUtil.h"
# include "./UtilKit/JSONUtil.h"
# include "./DataUtil/Compressor/ZLIB.h"
# include "../lib/zlib/zlib.h"

# include "./RTONUtil/Encoder_Export.h"
# include "./RTONUtil/Encryptor_Export.h"
# include "./AudioUtil/WEMEncoder/WEMEncoder.h"
# include "./AnimUtil/PopAnimUtil/Encoder.h"

namespace $TJ {
	namespace $Deprecated {}
	namespace $NonStdFunc {
		Void getFileNameNoPathNoSuffixFromDir(Path const & srcDir, Path const & dstFile) {
			StrList result;
			ListP<Path> fileList;
			$DevUtil::getFilePathList(srcDir, fileList, kTrue);
			for_riter(file, fileList) {
				file.eraseSuffix();
				if (!result.exist(file.getFilePath())) {
					result.add(file.getFilePath());
				}
			}
			$JSON::write(dstFile, $JSON::setArrStr(result, $JSON::Arr().setNull()));
			return;
		}
		Void getEventIDFromPAMDir(Path const & srcDir, Path const & dstFile) {
			StrList result;
			ListP<Path> fileList;
			$DevUtil::getFilePathList(srcDir, fileList, kTrue);
			SzI cnt = kThFirst;
			for_criter(file, fileList) {
				wLogLn("\rproc : %u / %u", cnt, fileList._sz);
				if (file.cmpSuffix(StrRef("pam"))) {
					$AnimUtil::$PopAnimUtil::$Struct::Struct st;
					Byte * dat; SzI sz;
					$DevUtil::ForceReadData(file, dat, sz);
					st.read(dat);
					delPtrArr(dat);
					for_criter(frame, st._mainAnim._frame) {
						if (frame._existCmd) {
							for_criter(cmd, frame._cmd) {
								if ((f_stricmp(cmd._cmd._str._str, "PlaySample") == 0
									 || f_stricmp(cmd._cmd._str._str, "PlaySmaple") == 0
									 || f_stricmp(cmd._cmd._str._str, "play_sound") == 0) && !result.exist(cmd._arg._str)) {
									result.add(cmd._arg._str);
								}
							}
						}
					}
				}
				++cnt;
			}
			$JSON::write(dstFile, $JSON::setArrStr(result, $JSON::Arr().setNull()));
			return;
		}
		Void BatchCpyLevelFile(Path const & srcFile, Path const & dstDir, Str const & prefix, Str const & suffix, SzI const & sum) {
			for_cnt(idx, kThFirst, sum) {
				Str fileName;
				fileName.printFmt("%s%u.%s", prefix._str, idx, suffix._str);
				$DevUtil::CpyFile(srcFile, Path(dstDir).add(fileName));
				fileName.tfree();
			}
			return;
		}

		Void ReNameFileCase(Path const & oriListDataFile, Path const & tgtDir) {
			$TJ::$InfoUtil::$ResPkgInfo::ResPkgInfo OriListData;
			OriListData.read($JSON::read(oriListDataFile, $JSON::Obj().setNull()));
			ListP<Path> dirList, fileList;
			for_criter(group, OriListData._group)
				for_criter(subGroup, group._val._subGroup)
				for_criter(res, subGroup._val._res)
				if (res._val._path[kThFirst] != "!program") {
					fileList.add(res._val._path);
					if (res._val._resType == $TJ::$InfoUtil::$ResPkgInfo::ResType::kAtlas)
						fileList.last().setSuffix("ptx");
				}
			fileList.add(Path("properties/resources.rton"));
			for_criter(file, fileList) {
				Path mid;
				mid.setNull();
				for_cnt(idx, kThFirst, file._sz - kThSecond) {
					mid.add(file[idx]);
					if (!dirList.exist(mid))
						dirList.add(mid);
				}
				mid.tfree();
			}
			{
				Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, tgtDir);
				for_cnt(i, kThFirst, dirList._sz) {
					$DevUtil::ReNameDir(dirList[i], dirList[i]);
					wLog("\r>> ReName Dir  : %u / %u            ", i + kThSecond, dirList._sz);
				}
				for_cnt(i, kThFirst, fileList._sz) {
					$DevUtil::ReNameFile(fileList[i], fileList[i]);
					wLog("\r>> ReName File : %u / %u            ", i + kThSecond, fileList._sz);
				}
				$DevUtil::chgDirThenFree(parentDir);
			}
			return;
		}

		Void MixBNKSubList(Path const & srcAFile, Path const & srcBFile, Path const & dstFile) {
			$JSON::Arr srcA, srcB, dst;
			$JSON::read(srcAFile, srcA);
			$JSON::read(srcBFile, srcB);
			dst.setNull();
			ListB<SzI> idList;
			for_cnt(idx, kThFirst, srcA._sz)
				if (!idList.exist(srcA[idx]["ID"].getUInt())) {
					idList.add(srcA[idx]["ID"].getUInt());
					dst.add(srcA[idx]);
				}
			for_cnt(idx, kThFirst, srcB._sz)
				if (!idList.exist(srcB[idx]["ID"].getUInt())) {
					idList.add(srcB[idx]["ID"].getUInt());
					dst.add(srcB[idx]);
				} else {
					wLogLn("%u", srcB[idx]["ID"].getUInt());
				}
			$JSON::write(dstFile, dst);
		}

		Void CompressRawRSBSMFData(Byte const * const & src, SzI const & srcSz, Byte *& dst, SzI & dstSz) {
			newPtr(dst, srcSz + kSzEight);
			*(reinterpret_cast<UI32 *>(dst + kThFirst)) = kThFirst;
			*(reinterpret_cast<UI32 *>(dst + kSzFour)) = srcSz;
			$DataUtil::$Compressor::$ZLIB::Compress(src, srcSz, dst + kSzEight, dstSz);
			return;
		}
		Void UnCompressRawRSBSMFData(Byte const * const & src, SzI const & srcSz, Byte *& dst, SzI & dstSz) {
			dstSz = *(reinterpret_cast<UI32 const *>(src + kSzFour));
			newPtr(dst, dstSz);
			$DataUtil::$Compressor::$ZLIB::UnCompress(src + kSzEight, srcSz - kSzEight, dst, dstSz);
			return;
		}
		Void CompressRawRSBSMF(Path const & srcFile, Path const & dstFile) {
			Byte * srcData(kNullPtr); SzI srcSize;
			Byte * dstData(kNullPtr); SzI dstSize;
			$DevUtil::ForceReadData(srcFile, srcData, srcSize);
			CompressRawRSBSMFData(srcData, srcSize, dstData, dstSize);
			$DevUtil::WriteData(dstFile, dstData, dstSize);
			delPtrArr(srcData); delPtrArr(dstData);
			return;
		}
		Void UnCompressRawRSBSMF(Path const & srcFile, Path const & dstFile) {
			Byte * srcData(kNullPtr); SzI srcSize;
			Byte * dstData(kNullPtr); SzI dstSize;
			$DevUtil::ForceReadData(srcFile, srcData, srcSize);
			UnCompressRawRSBSMFData(srcData, srcSize, dstData, dstSize);
			$DevUtil::WriteData(dstFile, dstData, dstSize);
			delPtrArr(srcData); delPtrArr(dstData);
			return;
		}

		Void CnvWEM(Path const & srcFile, Path const & dstFile) {
			$AudioUtil::$WEMEncoder::WEMDeCodeToOGG(srcFile, dstFile);
			return;
		}

		Void XCatRSBResInfoByPart(Path const & rawResInfoPartDir, Path const & partIDListFile, Path const & partInfoListFile, Path const & resInfoFile) {
			StrList partIDList;
			$ResPkgUtil::$RSBUtil::$PartInfo::PartInfoList partInfoList;
			$JSON::getArrStr($JSON::read(partIDListFile, $JSON::Arr().setNull()), partIDList);
			$InfoUtil::$ResPkgInfo::ResPkgInfo fullResInfo;
			for_criter(partID, partIDList) {
				Byte * rtonData(kNullPtr); SzI rtonSz;
				$DevUtil::ForceReadData(Path(rawResInfoPartDir).add(partID).setSuffix("rton"), rtonData, rtonSz);
				$JSON::Obj ripeJSON;
				$RTONUtil::$Encoder::$DeCode::decFull(rtonData, rtonSz, ripeJSON);
				delPtrArr(rtonData);
				$InfoUtil::$ResPkgInfo_Ori::ResPkgInfo_Ori resInfoOri;
				$InfoUtil::$ResPkgInfo::ResPkgInfo resInfo;
				resInfoOri.read(ripeJSON);
				$InfoUtil::$Cnv::$OriToResPkgInfo::ParseInfo(resInfoOri, resInfo);
				fullResInfo._group.add(resInfo._group);
				{
					auto & partInfo = partInfoList.add().last();
					partInfo._id = partID;
					for_criter(group, resInfo._group)
						partInfo._group.add(group._key);
				}
			}
			$JSON::write(resInfoFile, fullResInfo.write($JSON::Obj().setNull()));
			$JSON::write(partInfoListFile, partInfoList.write($JSON::Arr().setNull()));
			return;
		}
		Void XSplitRSBResInfoByPart(Path const & rawResInfoPartDir, Path const & partInfoListFile, Path const & resInfoFile, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			$ResPkgUtil::$RSBUtil::$PartInfo::PartInfoList partInfoList;
			$InfoUtil::$ResPkgInfo::ResPkgInfo resInfo;
			partInfoList.read($JSON::read(partInfoListFile, $JSON::Arr().setNull()));
			resInfo.read($JSON::read(resInfoFile, $JSON::Obj().setNull()));
			int iofmain = 0;
			for_criter(partInfo, partInfoList) {
				$InfoUtil::$ResPkgInfo::ResPkgInfo partResInfo;
				{
					int iofmain2;
					auto iintro = partInfo._id.cmp("Main");
					iofmain2 = iofmain;
					for_criter(groupID, partInfo._group)
						for_criter(group, resInfo._group)
						if (groupID == group._key) {
							if (iintro && iofmain2 > 0) {
								iofmain2 -= 1;
								continue;
							}
							partResInfo._group.appendByKV(group._key, group._val);
							break;
						}
					iofmain += 1;
				}
				$RTONUtil::$Encoder::$EnCode::encFull($InfoUtil::$Cnv::$ResPkgInfoToOri::ParseInfo(partResInfo, $InfoUtil::$ResPkgInfo_Ori::ResPkgInfo_Ori().getThis()).write($JSON::Obj().setNull()), data, sz, kTrue, kFalse);
				$DevUtil::WriteData(Path(rawResInfoPartDir).add(partInfo._id).setSuffix("rton"), data, sz);
			}
			delPtrArr(data);
			return;
		}

		Void MixRSBResInfoToHD(Path const & baseInfoFile, Path const & hdInfoFile, Path const & dstInfoFile, SzI const & lowResID, SzI const & highResID) {
			$InfoUtil::$ResPkgInfo::ResPkgInfo stBase, stHD;
			stHD.read($JSON::read(hdInfoFile, $JSON::Obj().setNull()));
			stBase.read($JSON::read(baseInfoFile, $JSON::Obj().setNull()));
			int ds = 0;
			for_riter(groupBase, stBase._group)
				for_criter(groupHD, stHD._group)
				if (groupBase._val._isComposite && groupBase._key == groupHD._key) {
					auto hasl = false, hash = false;
					int rs1, rs2;
					for (SzI subGroupIdx = kThFirst; subGroupIdx < groupBase._val._subGroup._sz; ++subGroupIdx)
						if (groupBase._val._subGroup[subGroupIdx]._val._category._res == lowResID) {
							groupBase._val._subGroup[subGroupIdx]._val._category._res = highResID;
							rs1 = groupBase._val._subGroup[subGroupIdx]._val._res._sz;
							groupBase._val._subGroup.erase(subGroupIdx);
							hasl = true;
							break;
						}/*
					for (SzI subGroupIdx = kFirstItemIdx; subGroupIdx < groupHD._subGroup._sz; ++subGroupIdx)
						if (groupHD._subGroup[subGroupIdx]._resID == highResID) {
							rs2 = groupHD._subGroup[subGroupIdx]._res._sz;
							groupBase._subGroup.add(groupHD._subGroup[subGroupIdx]);
							hash = true;
							break;
						}
					ds += rs1 - rs2;
					if (rs1 != rs2)
						wWarn("rs1 = %d, rs2 = %d, at %s, ds = %d", rs1, rs2, groupHD._id._str, ds);
					if (!hasl || !hash)
						wError("hasl = %u, hash = %u", hasl, hash);*/
				}
			$JSON::write(dstInfoFile, stBase.write($JSON::Obj().setNull()));
			return;
		}
		Void MixRSBStructInfoToHD(Path const & baseInfoFile, Path const & hdInfoFile, Path const & dstInfoFile, SzI const & lowResID, SzI const & highResID) {
			$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo stBase, stHD;
			stHD.read($JSON::read(hdInfoFile, $JSON::Obj().setNull()));
			stBase.read($JSON::read(baseInfoFile, $JSON::Obj().setNull()));
			for_riter(groupBase, stBase._group)
				for_criter(groupHD, stHD._group)
				if (groupBase._val._isComposite && groupBase._key == groupHD._key) {
					for_cnt(subGroupIdx, kThFirst, groupBase._val._subGroup._sz)
						if (groupBase._val._subGroup[subGroupIdx]._val._category._res == lowResID) {
							groupBase._val._subGroup[subGroupIdx]._val._category._res = highResID;
							groupBase._val._subGroup.erase(subGroupIdx);
							break;
						}/*
					for (SzI subGroupIdx = kThFirst; subGroupIdx < groupHD._val._subGroup._sz; ++subGroupIdx)
						if (groupHD._val._subGroup[subGroupIdx]._val._resID == highResID) {
							groupBase._val._subGroup.add(groupHD._val._subGroup[subGroupIdx]);
							break;
						}*/
				}
			$JSON::write(dstInfoFile, stBase.write($JSON::Obj().setNull()));
			//$JSON::write(Path(dstInfoFile).setSuffix(StrRef(".atlas.JSON")), $InfoUtil::$Cnv::$RSBStructInfoToImageInfoList::ParseInfo(stBase, $ImageUtil::_ImageInfoList().getThis(), $ImageUtil::_TexFmtIdxList().read($JSON::read(_Path("\\SPCUtil\\TexFmtIdxList\\ROW_iOS.json"), $JSON::Arr().setNull()))).write($JSON::Obj().setNull()));
			return;
		}
	}
}
