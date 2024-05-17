#pragma once

#include "../../../../Type.hpp"
#include "../../../../Type/Path.hpp"

#include "./Info.hpp"
#include "./Struct.hpp"
#include "../Common.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$RSGP::$Packer {

	namespace $Pack {

		extern Void pack(ByteSection & pkg, $Info::Info const & info, Path const & resDir);

		extern Void packExport(Path const & pkgFile, Path const & infoFile, Path const & resDir, Sz const & memSz);

	}

	namespace $UnPack {

		extern Void parseData(ByteSection const & data, $Struct::Header & header, Map<Str, $Struct::ResInfoSlot> & resInfo);

		extern $Info::Info & parseInfo($Info::Info & info, Sz const & headerType, IU32 const & resStoreMethod, Map<Str, $Struct::ResInfoSlot> const & resInfoList);

		extern Void exportRes(ByteSection const & normalResData, ByteSection const & atlasResData, Map<Str, $Struct::ResInfoSlot> const & resInfoList, Path const & dstDir);

		extern Void exportResEntry(ByteSection const & data, ResStoreMethod const & resStoreMethod,
								   Sz const & normalResDataOfs, Sz const & normalResDataOriSz, Sz const & normalResDataSz,
								   Sz const & atlasResDataOfs, Sz const & atlasResDataOriSz, Sz const & atlasResDataSz,
								   Map<Str, $Struct::ResInfoSlot> const & resInfo, Path const & dstDir
		);

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, Path const & resDir);

		extern Void unpackExport(Path const & pkgFile, Path const & infoFile, Path const & resDir);

	}

}
