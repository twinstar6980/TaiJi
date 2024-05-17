#pragma once

#include "../../Type.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/ByteStream.hpp"
#include "../../Type/Path.hpp"

namespace $TJ::$Util::$RTON::$Encoder {

	namespace $EnCode {

		extern Void encUnit($JSON::Val const & src, ByteStream & dst, List<Str> & idxStr, Bool const & enableRTIDType);

		extern Void encWhole($JSON::Obj const & src, ByteStream & dst, Bool const & enableStrIdx, Bool const & enableRTIDType);

		extern Void encExport(Path const & jsonFile, Path const & rtonFile, Bool const & enableStrIdx, Bool const & enableRTIDType, Bool const & isWhole, Sz const & memSz);

	}

	namespace $DeCode {

		extern Void decUnit(ByteStream & src, $JSON::Val & dst, List<Str> & idxStr, List<Str> & idxUTF8Str);

		extern Void decWhole(ByteStream & src, $JSON::Obj & dst);

		extern Void decExport(Path const & rtonFile, Path const & jsonFile, Bool const & isWhole);

	}

}
