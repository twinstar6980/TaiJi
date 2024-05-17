# include "./Packer_Export.h"

# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

namespace $TJ::$ResPkgUtil::$RSBUtil::$Packer {
	namespace $Pack {
		Void expPack(Byte * const & data, SzI & sz, Path const & structInfoFile, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outputNewPacket, Bool const & useReadyPacket, Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir, Bool const & cipherTextUpperCase) {
			return Pack(data, sz, $StructInfo::StructInfo().read($JSON::read(structInfoFile, $JSON::Obj().setNull())), packFromGroup, packRSGPByMainInfo, outputNewPacket, useReadyPacket, resDir, packetDir, packetInfoDir, groupDir, cipherTextUpperCase);
		}
		Void expPackDflt(Byte * const & data, SzI & sz, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Bool const & cipherTextUpperCase, Path const & mainDir) {
			return expPack(data, sz, Path(mainDir).add($DfltPath::kStructInfoFile), packFromGroup, packRSGPByMainInfo, outPutNewPacket, useReadyPacket, Path(mainDir).add($DfltPath::kResDir), Path(mainDir).add($DfltPath::kPacketDir), Path(mainDir).add($DfltPath::kPacketInfoDir), Path(mainDir).add($DfltPath::kGroupDir), cipherTextUpperCase);
		}
		Void expPackDflt(Path const & pkgFile, Path const & mainDir, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Bool const & cipherTextUpperCase, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			expPackDflt(data, sz, packFromGroup, packRSGPByMainInfo, outPutNewPacket, useReadyPacket, cipherTextUpperCase, mainDir);
			$DevUtil::WriteData(pkgFile, data, sz);
			delPtrArr(data);
			return;
		}
		Void expPackToPart(Path const & partDir, Path const & partInfoListFile, Path const & structInfoFile, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Path const & resDir, Path const & packetDir, Path const & packetInfoDir, Path const & groupDir, Bool const & cipherTextUpperCase, SzI const & memSz) {
			return PackToPart(partDir, $PartInfo::PartInfoList().read($JSON::read(partInfoListFile, $JSON::Arr().setNull())), $StructInfo::StructInfo().read($JSON::read(structInfoFile, $JSON::Obj().setNull())), packFromGroup, packRSGPByMainInfo, outPutNewPacket, useReadyPacket, resDir, packetDir, packetInfoDir, groupDir, cipherTextUpperCase, memSz);
		}
		Void expPackToPartDflt(Path const & partDir, Path const & mainDir, Bool const & packFromGroup, Bool const & packRSGPByMainInfo, Bool const & outPutNewPacket, Bool const & useReadyPacket, Bool const & cipherTextUpperCase, SzI const & memSz) {
			return expPackToPart(partDir, Path(partDir).add($DfltPath::kPartInfoListFile), Path(mainDir).add($DfltPath::kStructInfoFile), packFromGroup, packRSGPByMainInfo, outPutNewPacket, useReadyPacket, Path(mainDir).add($DfltPath::kResDir), Path(mainDir).add($DfltPath::kPacketDir), Path(mainDir).add($DfltPath::kPacketInfoDir), Path(mainDir).add($DfltPath::kGroupDir), cipherTextUpperCase, memSz);
		}
	}
	namespace $UnPack {
		Void expUnPackDflt(Byte const * const & data, SzI const & sz, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket, Path const & mainDir) {
			return UnPack(data, sz, Path(mainDir).add($DfltPath::kStructInfoFile), Path(mainDir).add($DfltPath::kResInfoListFile), outputRes, Path(mainDir).add($DfltPath::kResDir), outputPacket, Path(mainDir).add($DfltPath::kPacketDir), outputPacketInfo, Path(mainDir).add($DfltPath::kPacketInfoDir), outputGroup, outputGroupRes, outputGroupPacket, Path(mainDir).add($DfltPath::kGroupDir));
		}
		Void expUnPackDflt(Path const & pkgFile, Path const & mainDir, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket) {
			Byte * data(kNullPtr); SzI sz;
			$DevUtil::ForceReadData(pkgFile, data, sz);
			expUnPackDflt(data, sz, outputRes, outputPacket, outputPacketInfo, outputGroup, outputGroupRes, outputGroupPacket, mainDir);
			delPtrArr(data);
			return;
		}
		Void expUnPackFromPartDflt2(Path const & partDir, Path const & mainDir, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket) {
			return UnPackFromPart(partDir, Path(partDir).add($DfltPath::kPartIDListFile), Path(partDir).add($DfltPath::kPartInfoListFile), Path(mainDir).add($DfltPath::kStructInfoFile), Path(mainDir).add($DfltPath::kResInfoListFile), outputRes, Path(mainDir).add($DfltPath::kResDir), outputPacket, Path(mainDir).add($DfltPath::kPacketDir), outputPacketInfo, Path(mainDir).add($DfltPath::kPacketInfoDir), outputGroup, outputGroupRes, outputGroupPacket, Path(mainDir).add($DfltPath::kGroupDir));
		}
		Void expUnPackFromPartDflt(Path const & partDir, Path const & mainDir, Bool const & outputRes, Bool const & outputPacket, Bool const & outputPacketInfo, Bool const & outputGroup, Bool const & outputGroupRes, Bool const & outputGroupPacket) {
			return expUnPackFromPartDflt2(partDir, mainDir, outputRes, outputPacket, outputPacketInfo, outputGroup, outputGroupRes, outputGroupPacket);
		}
	}
}
