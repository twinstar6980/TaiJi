# include "./PvZ-2/RSBPatch.h"
# include "../MyType/Str.h"
# include "../UtilKit/JSONUtil.h"
# include "../MyType/ListP.h"

namespace $TJ::$SpecialMod {
	namespace $PvZ_2 {
		namespace $RSBPatch {
			Void patchResInfo(Path const & patchFile, Path const & srcFile, Path const & dstFile) {
				$InfoUtil::$ResPkgInfo::ResPkgInfo info;
				info.read($JSON::read(srcFile, $JSON::Obj().setNull()));
				$PatchResInfo::patch($PatchInfo::PatchInfo().read($JSON::read(patchFile, $JSON::Obj().setNull())), info);
				$JSON::write(dstFile, info.write($JSON::Obj().setNull()));
				return;
			}
			Void patchStructInfo(Path const & patchFile, Path const & srcFile, Path const & dstFile) {
				$ResPkgUtil::$RSBUtil::$StructInfo::StructInfo info;
				info.read($JSON::read(srcFile, $JSON::Obj().setNull()));
				$PatchStructInfo::patch($PatchInfo::PatchInfo().read($JSON::read(patchFile, $JSON::Obj().setNull())), info);
				$JSON::write(dstFile, info.write($JSON::Obj().setNull()));
				return;
			}
			Void patchRes(Path const & patchFile, Path const & structInfoFile, Path const & srcResDir, Path const & dstGroupDir) {
				$PatchRes::patch($PatchInfo::PatchInfo().read($JSON::read(patchFile, $JSON::Obj().setNull())), $ResPkgUtil::$RSBUtil::$StructInfo::StructInfo().read($JSON::read(structInfoFile, $JSON::Obj().setNull())), srcResDir, dstGroupDir);
				return;
			}
		}
	}
}
