# pragma once

# include "../../Type.h"
# include "../../MyType/Str.h"
# include "../../MyType/Path.h"

# include "./StructInfo.h"
# include "./Struct.h"

# include "../RSBUtil/CipherText.h"

namespace $TJ::$ResPkgUtil::$RSGPUtil::$Packer {
	using namespace $RSBUtil::$CipherText;
}
namespace $TJ::$ResPkgUtil::$RSGPUtil::$Packer {
	namespace $Pack {
		Void Pack(
			Byte * const & data, SzI & sz,
			$StructInfo::StructInfo const & structInfo,
			Path const & resDir, Bool const & cipherTextUpperCase
		);
	}
	namespace $UnPack {
		Void OutPutResFile(
			Byte const * const & normalResData, SzI const & normalResDataSz,
			Byte const * const atlasResData, SzI const & atlasResDataSz,
			Map<Str, $Struct::ResInfoSlot> const & resInfoList, Path const & dstDir
		);
		Void OutPutResFile(
			Byte const * const & data, UI32 const & resDataCompressMethod,
			SzI const & normalResData_Ofs, SzI const & normalResData_InitSz, SzI const & normalResData_Sz,
			SzI const & atlasResData_Ofs, SzI const & atlasResData_InitSz, SzI const & atlasResData_Sz,
			Map<Str, $Struct::ResInfoSlot> const & resInfoList, Path const & dstDir
		);
		$StructInfo::StructInfo & ParseInfo(
			$StructInfo::StructInfo & structInfo,
			SzI const & headerType, UI32 const & resDataCompressMethod,
			Map<Str, $Struct::ResInfoSlot> const & resInfoList
		);
		Void Analysis(
			Byte const * const & data,
			$Struct::$Header::Member & header, CipherText<$Struct::ResInfoSlot> & resInfoList
		);
		Void UnPack(
			Byte const * const & data, SzI const & sz,
			Path const & structInfoFile, Path const & resDir
		);
	}
}
