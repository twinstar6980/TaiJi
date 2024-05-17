# pragma once

# include "./Packer.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$Packer {
	namespace $DfltPath {
		Path const kStructInfoFile("StructInfo.json");
		Path const kResInfoListFile("ResInfoList.json");
		Path const kResDir("Res");
		Path const kPacketDir("Packet");
		Path const kPacketInfoDir("PacketInfo");
		Path const kGroupDir("Group");
		Str const kPartIDListFile("ID.json");
		Str const kPartInfoListFile("Info.json");
	}
	namespace $Pack {
		Void expPack(Byte * const & data, SzI & sz, Path const & structInfoFile, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outputNewPacket, Bool const & useReadyPacket, Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir, Bool const & cipherTextUpperCase);
		Void expPackDflt(Byte * const & data, SzI & sz, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Bool const & cipherTextUpperCase, Path const & mainDir);
		Void expPackDflt(Path const & pkgFile, Path const & mainDir, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Bool const & cipherTextUpperCase, SzI const & memSz);
		Void expPackToPart(Path const & partDir, Path const & partInfoListFile, Path const & structInfoFile, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir, Bool const & cipherTextUpperCase, SzI const & memSz);
		Void expPackToPartDflt(Path const & partDir, Path const & mainDir, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Bool const & cipherTextUpperCase, SzI const & memSz);
	}
	namespace $UnPack {
		Void expUnPackDflt(Byte const * const & data, SzI const & sz, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & mainDir);
		Void expUnPackDflt(Path const & pkgFile, Path const & mainDir, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket);
		Void expUnPackFromPartDflt2(Path const & partDir, Path const & mainDir, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket);
		Void expUnPackFromPartDflt(Path const & partDir, Path const & mainDir, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket);
	}
}
