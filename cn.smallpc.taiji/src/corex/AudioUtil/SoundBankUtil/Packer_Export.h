# pragma once

# include "./Packer.h"

namespace $TJ::$AudioUtil::$SoundBankUtil::$NameMapperMod {
	Void E_Load(Path const & nameMapperFile);
	Void E_Clear();
}
namespace $TJ::$AudioUtil::$SoundBankUtil::$Packer {
	namespace $Pack {
		Void expPackDflt(Path const & soundBankFile, Path const & mainDir, SzI const & memSz);
	}
	namespace $UnPack {
		Void expUnPackDflt(Path const & soundBankFile, Path const & mainDir);
	}
}
