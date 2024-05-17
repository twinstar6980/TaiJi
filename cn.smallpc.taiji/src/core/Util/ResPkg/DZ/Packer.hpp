#pragma once

#include "../../../Type.hpp"
#include "../../../Type/Path.hpp"

#include "./Info.hpp"
#include "./Struct.hpp"

namespace $TJ::$Util::$ResPkg::$DZ::$Packer {

	namespace $Pack {

		extern Void pack(ByteSection & pkg, $Info::Info const & info, Path const & resDir);

		extern Void packExport(Path const & pkgFile, Path const & infoFile, Path const & resDir, Sz const & memSz);

	}

	namespace $UnPack {

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, Path const & resDir);

		extern Void unpackExport(Path const & pkgFile, Path const & infoFile, Path const & resDir);

	}

}
