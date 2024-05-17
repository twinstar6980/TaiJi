#pragma once

#include "./Info.hpp"
#include "./Struct.hpp"
#include "./RSGP/Info.hpp"
#include "./RSGP/Struct.hpp"
#include "./RSGP/Packer.hpp"
#include "./Common.hpp"

#include "../../../Type/JSON.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$Packer {

	namespace $Pack {

		extern Void pack(ByteSection & pkg, $Info::Info const & info, NullPtr const & embeddedResInfo,
						 Path const & packetDir, Path const & resDir,
						 Path const & groupDir, Path const & groupPacketFile, Path const & groupResDir,
						 Bool const & packFromGroup, Bool const & exportNewPacket, Bool const & useReadyPacket
		);

		extern Void packExport(Path const & pkgFile, Path const & infoFile, Path const & embeddedResInfoFile,
							   Path const & packetDir, Path const & resDir,
							   Path const & groupDir, Path const & groupPacketFile, Path const & groupResDir,
							   Bool const & packFromGroup, Bool const & exportNewPacket, Bool const & useReadyPacket, Sz const & memSz
		);

	}

	namespace $UnPack {

		extern Void parseData(ByteSection const & data, $Struct::Header & header,
							  Map<Str, $Struct::StrIdxInfoSlot> & groupID,
							  Map<Str, $Struct::StrIdxInfoSlot> & subGroupID,
							  Map<Str, $Struct::StrIdxInfoSlot> & resPath,
							  List<$Struct::GroupInfo> & groupInfo,
							  List<$Struct::SubGroupInfo> & subGroupInfo,
							  List<$Struct::AutoPoolInfo> & autoPoolInfo,
							  List<$Struct::AtlasResInfo> & atlasResInfo,
							  List<$Struct::AtlasResExpandInfo> & atlasResExpandInfo,
							  List<$RSGP::$Struct::Header> & rsgpHeader,
							  List<Map<Str, $RSGP::$Struct::ResInfoSlot>> & rsgpResInfo,
							  NullPtr const & embeddedResInfo
		);
		extern Void parseInfo($Info::Info & info, Sz const & headerType,
							  Bool const & enableEmbeddedResInfo, Bool const & enableAtlasInfoExpand,
							  List<$Struct::GroupInfo> const & groupInfoList,
							  List<$Struct::SubGroupInfo> const & subGroupInfoList,
							  Map<Str, $Struct::StrIdxInfoSlot> const & resPathList,
							  List<$Struct::AtlasResInfo> const & atlasResInfoList,
							  List<$RSGP::$Struct::Header> const & rsgpHeaderList,
							  List<Map<Str, $RSGP::$Struct::ResInfoSlot>> const & rsgpResInfoList
		);

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, NullPtr const & embeddedResInfo,
						   Path const & packetDir, Path const & packetInfoDir, Path const & resDir,
						   Path const & groupDir, Path const & groupPacketFile, Path const & groupPacketInfoFile, Path const & groupResDir
		);

		extern Void unpackExport(Path const & pkgFile, Path const & infoFile, Path const & embeddedResInfoFile,
								 Path const & packetDir, Path const & packetInfoDir, Path const & resDir,
								 Path const & groupDir, Path const & groupPacketFile, Path const & groupPacketInfoFile, Path const & groupResDir
		);

	}

}
