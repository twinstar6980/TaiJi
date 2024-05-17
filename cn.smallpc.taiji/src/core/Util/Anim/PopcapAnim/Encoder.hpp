#pragma once

#include "../../../Type.hpp"
#include "../../../Type/Path.hpp"

#include "./Struct.hpp"
#include "./Info.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Encoder {

	namespace $EnCode {

		extern Void enc(ByteSection & data, $Info::Info const & info);

		extern Void encExport(Path const & dataFile, Path const & infoFile, Sz const & memSz);

	}

	namespace $DeCode {

		extern $Info::Info & dec(ByteSection const & data, $Info::Info & info);

		extern Void decExport(Path const & dataFile, Path const & infoFile);

	}

}
