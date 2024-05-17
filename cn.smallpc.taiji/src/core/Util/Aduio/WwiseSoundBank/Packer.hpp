#pragma once

#include "./Info.hpp"
#include "./Struct.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$Audio::$WwiseSoundBank::$NameMapperMod {

	Void load(List<Str> const & t);
	Void load($JSON::Arr const & t);
	Void clear();

}

namespace $TJ::$Util::$Audio::$WwiseSoundBank::$Packer {

	namespace $Pack {

		extern Void packExport(Path const & dataFile, Path const & mainDir, Sz const & memSz);

	}

	namespace $UnPack {

		extern Void unpackExport(Path const & dataFile, Path const & mainDir);

	}

}
