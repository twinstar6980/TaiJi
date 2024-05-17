# pragma once

# include "./Packer.h"

namespace $TJ::$ResPkgUtil::$PAKUtil::$Packer {
	namespace $DfltPath {
		Path const kStructInfoFile("StructInfo.json");
		Path const kResDir("Res");
	}
	namespace $Pack {
		Void expPack(Byte * const & data, SzI & sz, Path const & structInfoFile, Path const & resDir);
		Void expPackDflt(Byte * const & data, SzI & sz, Path const & mainDir);
		Void expPackDflt(Path const & pkgFile, Path const & mainDir, SzI const & memSz);
	}
	namespace $UnPack {
		Void expUnPackDflt(Byte const * const & data, SzI const & sz, Path const & mainDir);
		Void expUnPackDflt(Path const & pkgFile, Path const & mainDir);
	}
}
