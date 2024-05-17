# pragma once

# include "./StructInfo.h"
# include "./Struct.h"
# include "../../MyType/Path.h"

namespace $TJ::$AudioUtil::$SoundBankUtil::$NameMapperMod {
	Void load(StrList const & src);
	Void load($JSON::Arr const & src);
	Void clear();
}
namespace $TJ::$AudioUtil::$SoundBankUtil::$Packer {
	namespace $Pack {
		Void Pack(Byte * const & data, SzI & sz, Path const & mainDir);
	}
	namespace $UnPack {
		Void UnPack(Byte const * const & data, SzI const & sz, Path const & mainDir);
	}
}
