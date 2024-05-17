# pragma once

# include "../../Type.h"
# include "../../MyType/Str.h"
# include "../../MyType/Path.h"
# include "../../MyType/Map.h"

# include "./StructInfo.h"
# include "./Struct.h"
# include "../RSGPUtil/Struct.h"
# include "./CipherText.h"
# include "../../InfoUtil/ResPkgInfo_Ori.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$Packer {
	using namespace $CipherText;
}
namespace $TJ::$ResPkgUtil::$RSBUtil::$Packer {
	namespace $Pack {
		Void Pack(
			Byte * const & data, SzI & sz,
			$StructInfo::StructInfo const & structInfo,
			Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outputNewPacket, Bool const & useReadyPacket,
			Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir,
			Bool const & cipherTextUpperCase
		);
		Void PackToPart(
			Path const & partDir, $PartInfo::PartInfoList const & partInfoList,
			$StructInfo::StructInfo const & structInfo,
			Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outputNewPacket, Bool const & useReadyPacket,
			Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir,
			Bool const & cipherTextUpperCase, SzI const & memSz
		);
	}
	namespace $UnPack {
		Void UnPack(
			Byte const * const & data, SzI const & sz,
			Path const & structInfoFile, Path const & resInfoListFile,
			Bool const & outputRes, Path const & resDir,
			Bool const & outputPacket, Path const & packetDir,
			Bool const & outputPacketInfo, Path const & packetInfoDir,
			Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & groupDir
		);
		Void UnPackFromPart(
			Path const & partDir, Path const & partIDListFile, Path const & partInfoListFile,
			Path const & structInfoFile, Path const & resInfoListFile,
			Bool const & outputRes, Path const & resDir,
			Bool const & outputPacket, Path const & packetDir,
			Bool const & outputPacketInfo, Path const & packetInfoDir,
			Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & groupDir
		);
	}
}
