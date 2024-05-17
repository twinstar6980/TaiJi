# pragma once

# include "./Packer.h"

namespace $TJ::$ResPkgUtil::$RSGPUtil::$Packer {
	namespace $DfltPath {
		Path const kStructInfoFile("StructInfo.json");
		Path const kResDir("Res");
	}
	namespace $Pack {
		Void expPack(Byte * const & data, SzI & sz, Path const & structInfoPath, Path const & resDir, Bool const & cipherTextUpperCase);
		Void expPackDflt(Byte * const & data, SzI & sz, Bool const & cipherTextUpperCase, Path const & mainDir);
		Void expPackDflt(Path const & pkgFile, Path const & mainDir, Bool const & cipherTextUpperCase, SzI const & memSz);
		Void expPackLiteral(Path const & pkgFile, Path const & structInfoPath, Path const & resDir, Bool const & cipherTextUpperCase, SzI const & memSz);
	}
	namespace $UnPack {
		Void expUnPackDflt(Byte const * const & data, SzI const & sz, Path const & mainDir);
		Void expUnPackDflt(Path const & pkgFile, Path const & mainDir);
	}
}
