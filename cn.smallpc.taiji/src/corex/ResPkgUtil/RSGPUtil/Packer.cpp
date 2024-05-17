# include "./Packer.h"

# include "./StructInfo.h"
# include "./Struct.h"
# include "../RSBUtil/CipherText.h"

# include "../../Const.h"
# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"
# include "../../DataUtil/Compressor/ZLIB.h"

namespace $TJ::$ResPkgUtil::$RSGPUtil::$Packer {
	namespace $Pack {
		Void Pack(
			Byte * const & data, SzI & sz,
			$StructInfo::StructInfo const & structInfo,
			Path const & resDir, Bool const & cipherTextUpperCase
		) {
			$Struct::$Header::Member header;
			{
				{
					header._id = $Struct::$Header::$MemberConst::kID;
					header._headerType = structInfo._headerType;
					header._resDataCompressMethod
						= ternary_oper(structInfo._compressNormalRes, $Struct::$ExistBit_CompressMethod::kCompressNormalData, $Struct::$ExistBit_CompressMethod::kNone)
						| ternary_oper(structInfo._compressAtlasRes, $Struct::$ExistBit_CompressMethod::kCompressAtlasData, $Struct::$ExistBit_CompressMethod::kNone);
				}
				{
					StrList tmpResPathList(structInfo._res._sz);
					for_criter(res, structInfo._res)
						res._path.getFullPath(tmpResPathList.add().getLast(), $PathDelimiter::kWindowsStyle);
					if (cipherTextUpperCase) tmpResPathList.upperCase();
					header._headerAndResInfoListSize = $Struct::$Header::kBlockSize + (CalcCipherTextMinSize(tmpResPathList.sortUp()) + (structInfo._res._sz * $Struct::$ResDataInfo::kBlockSize) + (structInfo._res.GetAtlasResSum() * $Struct::$ResAtlasInfo::kBlockSize));
					memSet(data + header._headerAndResInfoListSize, getEnoughSz(header._headerAndResInfoListSize) - header._headerAndResInfoListSize);
					header._headerAndResInfoListSize = getEnoughSz(header._headerAndResInfoListSize);
					tmpResPathList.tfree();
				}
			}
			{
				Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, resDir);
				CipherText<$Struct::ResInfoSlot> resInfoList;
				resInfoList.alloc(structInfo._res._sz);
				{
					Byte * tmpResData(kNullPtr);
					{
						header._normalResData_Ofs = header._headerAndResInfoListSize;
						header._normalResData_InitSize = kSzNone;
						for_criter(res, structInfo._res) {
							if (res.isNormalType()) {
								auto & resInfo = resInfoList.add().getLast();
								res._path.getFullPath(resInfo._key, $PathDelimiter::kWindowsStyle);
								{
									resInfo._val._dataInfo._resType = $Struct::$ResTypeIdx::kNormal;
								}
								resInfo._val._dataInfo._ofs = header._normalResData_InitSize;
								$DevUtil::ReadData(res._path, data + header._normalResData_Ofs + resInfo._val._dataInfo._ofs, resInfo._val._dataInfo._sz);
								header._normalResData_InitSize += resInfo._val._dataInfo._sz;
								memSet(data + header._normalResData_Ofs + header._normalResData_InitSize, getEnoughSz(header._normalResData_InitSize) - header._normalResData_InitSize);
								header._normalResData_InitSize = getEnoughSz(header._normalResData_InitSize);
							}
						}
						if (!structInfo._compressNormalRes)
							header._normalResData_Size = header._normalResData_InitSize;
						else {
							newPtr(tmpResData, static_cast<SzI>($DataUtil::$Compressor::$ZLIB::GetCompressBound(header._normalResData_InitSize)));
							$DataUtil::$Compressor::$ZLIB::Compress(data + header._normalResData_Ofs, header._normalResData_InitSize, tmpResData, header._normalResData_Size);
							memCpy(data + header._normalResData_Ofs, tmpResData, header._normalResData_Size);
							delPtrArr(tmpResData);
							memSet(data + header._normalResData_Ofs + header._normalResData_Size, getEnoughSz(header._normalResData_Size) - header._normalResData_Size);
							header._normalResData_Size = getEnoughSz(header._normalResData_Size);
						}
					}
					{
						header._atlasResData_Ofs = header._headerAndResInfoListSize + header._normalResData_Size;
						header._atlasResData_InitSize = kSzNone;
						for_criter(res, structInfo._res) {
							if (res.isAtlasType()) {
								auto & resInfo = resInfoList.add().getLast();
								res._path.getFullPath(resInfo._key, $PathDelimiter::kWindowsStyle);
								{
									resInfo._val._dataInfo._resType = $Struct::$ResTypeIdx::kAtlas;
									resInfo._val._atlasInfo._idx = res.getAtlasInfo()._idx;
									resInfo._val._atlasInfo._w = res.getAtlasInfo()._sz._w;
									resInfo._val._atlasInfo._h = res.getAtlasInfo()._sz._h;
								}
								resInfo._val._dataInfo._ofs = header._atlasResData_InitSize;
								$DevUtil::ReadData(res._path, data + header._atlasResData_Ofs + resInfo._val._dataInfo._ofs, resInfo._val._dataInfo._sz);
								header._atlasResData_InitSize += resInfo._val._dataInfo._sz;
								memSet(data + header._atlasResData_Ofs + header._atlasResData_InitSize, getEnoughSz(header._atlasResData_InitSize) - header._atlasResData_InitSize);
								header._atlasResData_InitSize = getEnoughSz(header._atlasResData_InitSize);
							}
						}
						if (!structInfo._compressAtlasRes)
							header._atlasResData_Size = header._atlasResData_InitSize;
						else {
							if (header._atlasResData_InitSize == kSzNone)
								header._atlasResData_Size = kSzNone;
							else {
								newPtr(tmpResData, $DataUtil::$Compressor::$ZLIB::GetCompressBound(header._atlasResData_InitSize));
								$DataUtil::$Compressor::$ZLIB::Compress(data + header._atlasResData_Ofs, header._atlasResData_InitSize, tmpResData, header._atlasResData_Size);
								memCpy(data + header._atlasResData_Ofs, tmpResData, header._atlasResData_Size);
								delPtrArr(tmpResData);
								memSet(data + header._atlasResData_Ofs + header._atlasResData_Size, getEnoughSz(header._atlasResData_Size) - header._atlasResData_Size);
								header._atlasResData_Size = getEnoughSz(header._atlasResData_Size);
							}
						}
					}
				}
				header._resInfoList_Ofs = $Struct::$Header::kBlockSize;
				if (cipherTextUpperCase) resInfoList.AllStrUpperCase();
				resInfoList.sort().EnCrypt(data + header._resInfoList_Ofs, header._resInfoList_Size).tfree();
				$DevUtil::chgDirThenFree(parentDir);
			}
			$DataBlockUtil::write(data + kThFirst, header);
			sz = header._headerAndResInfoListSize + header._normalResData_Size + header._atlasResData_Size;
			return;
		}
	}
	namespace $UnPack {
		Void OutPutResFile(
			Byte const * const & normalResData, SzI const & normalResDataSz,
			Byte const * const atlasResData, SzI const & atlasResDataSz,
			Map<Str, $Struct::ResInfoSlot> const & resInfoList, Path const & dstDir
		) {
			Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, dstDir);
			for_criter(resInfo, resInfoList)
				switch (resInfo._val._dataInfo._resType) {
					default: wErr("UnKnown ResType"); break;
					case $Struct::$ResTypeIdx::kNormal: $DevUtil::WriteData(Path(resInfo._key), normalResData + resInfo._val._dataInfo._ofs, resInfo._val._dataInfo._sz); break;
					case $Struct::$ResTypeIdx::kAtlas: $DevUtil::WriteData(Path(resInfo._key), atlasResData + resInfo._val._dataInfo._ofs, resInfo._val._dataInfo._sz); break;
				}
			$DevUtil::chgDirThenFree(parentDir);
			return;
		}
		Void OutPutResFile(
			Byte const * const & data, UI32 const & resDataCompressMethod,
			SzI const & normalResData_Ofs, SzI const & normalResData_InitSz, SzI const & normalResData_Sz,
			SzI const & atlasResData_Ofs, SzI const & atlasResData_InitSz, SzI const & atlasResData_Sz,
			Map<Str, $Struct::ResInfoSlot> const & resInfoList, Path const & dstDir
		) {
			Bool compressNormalData, compressAtlasData;
			compressNormalData = resDataCompressMethod & $Struct::$ExistBit_CompressMethod::kCompressNormalData;
			compressAtlasData = resDataCompressMethod & $Struct::$ExistBit_CompressMethod::kCompressAtlasData;
			Byte * normalResData(kNullPtr), * atlasResData(kNullPtr);
			if (!compressNormalData)
				normalResData = const_cast<Byte *>(data + normalResData_Ofs);
			else {
				newPtr(normalResData, normalResData_InitSz);
				memSet(normalResData, normalResData_InitSz);
				$DataUtil::$Compressor::$ZLIB::UnCompress(data + normalResData_Ofs, normalResData_Sz, normalResData, normalResData_InitSz);
			}
			if (!compressAtlasData)
				atlasResData = const_cast<Byte *>(data + atlasResData_Ofs);
			else {
				newPtr(atlasResData, atlasResData_InitSz);
				memSet(atlasResData, atlasResData_InitSz);
				$DataUtil::$Compressor::$ZLIB::UnCompress(data + atlasResData_Ofs, atlasResData_Sz, atlasResData, atlasResData_InitSz);
			}
			OutPutResFile(normalResData, normalResData_InitSz, atlasResData, atlasResData_InitSz, resInfoList, dstDir);
			if (compressNormalData)
				delPtrArr(normalResData);
			else normalResData = kNullPtr;
			if (compressAtlasData)
				delPtrArr(atlasResData);
			else atlasResData = kNullPtr;
			return;
		}
		$StructInfo::StructInfo & ParseInfo(
			$StructInfo::StructInfo & structInfo,
			SzI const & headerType, UI32 const & resDataCompressMethod,
			Map<Str, $Struct::ResInfoSlot> const & resInfoList
		) {
			structInfo._headerType = headerType;
			structInfo._compressNormalRes = resDataCompressMethod & $Struct::$ExistBit_CompressMethod::kCompressNormalData;
			structInfo._compressAtlasRes = resDataCompressMethod & $Struct::$ExistBit_CompressMethod::kCompressAtlasData;
			structInfo._res.alloc(resInfoList._sz);
			for_criter(resInfo, resInfoList) {
				auto & res = structInfo._res.add().getLast();
				res._path.read(resInfo._key);
				switch (resInfo._val._dataInfo._resType) {
					default: wErr("UnKnown ResType"); break;
					case $Struct::$ResTypeIdx::kNormal:
						res.setNormalType();
						break;
					case $Struct::$ResTypeIdx::kAtlas:
						res.setAtlasType();
						res.getAtlasInfo()._idx = resInfo._val._atlasInfo._idx;
						res.getAtlasInfo()._sz.set(resInfo._val._atlasInfo._w, resInfo._val._atlasInfo._h);
						break;
				}
			}
			return structInfo;
		}
		Void Analysis(
			Byte const * const & data,
			$Struct::$Header::Member & header, CipherText<$Struct::ResInfoSlot> & resInfoList
		) {
			$DataBlockUtil::read(data + kThFirst, header);
			if (header._id != makeFourCC('p', 'g', 's', 'r')) {
				wWarn("this rsgp header error !");
				return;
			}
			resInfoList.DeCrypt(data + header._resInfoList_Ofs, header._resInfoList_Size);
			return;
		}
		Void UnPack(
			Byte const * const & data, SzI const & sz,
			Path const & structInfoFile, Path const & resDir
		) {
			$Struct::$Header::Member header;
			CipherText<$Struct::ResInfoSlot> resInfoList;
			{
				Analysis(data, header, resInfoList);
				$JSON::write(structInfoFile, ParseInfo($StructInfo::StructInfo().getThis(), header._headerType, header._resDataCompressMethod, resInfoList).write($JSON::Obj().setNull()));
				OutPutResFile(data, header._resDataCompressMethod, header._normalResData_Ofs, header._normalResData_InitSize, header._normalResData_Size, header._atlasResData_Ofs, header._atlasResData_InitSize, header._atlasResData_Size, resInfoList, resDir);
			}
			resInfoList.tfree();
			return;
		}
	}
}
