# include "./Packer_Export.h"

# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

namespace $TJ::$AudioUtil::$SoundBankUtil::$NameMapperMod {
	Void E_Load(Path const & nameMapperFile) {
		load($JSON::read(nameMapperFile, $JSON::Arr().setNull()));
		return;
	}
	Void E_Clear() {
		clear();
		return;
	}
}
namespace $TJ::$AudioUtil::$SoundBankUtil::$Packer {
	namespace $Pack {
		Void expPackDflt(Path const & soundBankFile, Path const & mainDir, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			Pack(data, sz, mainDir);
			$DevUtil::WriteData(soundBankFile, data, sz);
			delPtrArr(data);
			return;
		}
	}
	namespace $UnPack {
		Void expUnPackDflt(Path const & soundBankFile, Path const & mainDir) {
			Byte * data(kNullPtr); SzI sz;
			$DevUtil::ForceReadData(soundBankFile, data, sz);
			UnPack(data, sz, mainDir);
			delPtrArr(data);
			return;
		}
	}
}
