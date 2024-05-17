# include "./Packer_Export.h"

# include "../../UtilKit/DevUtil.h"
# include "../../MyType/JSON.h"
# include "../../UtilKit/JSONUtil.h"

namespace $TJ::$ResPkgUtil::$PAKUtil::$Packer {
	namespace $Pack {
		Void expPack(Byte * const & data, SzI & sz, Path const & structInfoFile, Path const & resDir) { return Pack(data, sz, $StructInfo::StructInfo().read($JSON::read(structInfoFile, $JSON::Obj().setNull())), resDir); }
		Void expPackDflt(Byte * const & data, SzI & sz, Path const & mainDir) { return expPack(data, sz, Path(mainDir).add($DfltPath::kStructInfoFile), Path(mainDir).add($DfltPath::kResDir)); }
		Void expPackDflt(Path const & pkgFile, Path const & mainDir, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			expPackDflt(data, sz, mainDir);
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
