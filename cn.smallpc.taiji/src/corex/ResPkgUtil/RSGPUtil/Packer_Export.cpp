# include "./Packer_Export.h"

# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

namespace $TJ::$ResPkgUtil::$RSGPUtil::$Packer {
	namespace $Pack {
		Void expPack(Byte * const & data, SzI & sz, Path const & structInfoFile, Path const & resDir, Bool const & cipherTextUpperCase) { return Pack(data, sz, $StructInfo::StructInfo().read($JSON::read(structInfoFile, $JSON::Obj().setNull())), resDir, cipherTextUpperCase); }
		Void expPackDflt(Byte * const & data, SzI & sz, Bool const & cipherTextUpperCase, Path const & mainDir) { return expPack(data, sz, Path(mainDir).add($DfltPath::kStructInfoFile), Path(mainDir).add($DfltPath::kResDir), cipherTextUpperCase); }
		Void expPackDflt(Path const & pkgFile, Path const & mainDir, Bool const & cipherTextUpperCase, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			expPackDflt(data, sz, cipherTextUpperCase, mainDir);
			$DevUtil::WriteData(pkgFile, data, sz);
			delPtrArr(data);
			return;
		}
		Void expPackLiteral(Path const & pkgFile, Path const & structInfoPath, Path const & resDir, Bool const & cipherTextUpperCase, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			expPack(data, sz, structInfoPath, resDir, cipherTextUpperCase);
			$DevUtil::WriteData(pkgFile, data, sz);
			delPtrArr(data);
			return;
		}
	}
	namespace $UnPack {
		Void expUnPackDflt(Byte const * const & data, SzI const & sz, Path const & mainDir) { return UnPack(data, sz, Path(mainDir).add($DfltPath::kStructInfoFile), Path(mainDir).add($DfltPath::kResDir)); }
		Void expUnPackDflt(Path const & pkgFile, Path const & mainDir) {
			Byte * data(kNullPtr); SzI sz;
			$DevUtil::ForceReadData(pkgFile, data, sz);
			expUnPackDflt(data, sz, mainDir);
			delPtrArr(data);
			return;
		}
	}
}
