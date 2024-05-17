#pragma once

#include "../../Type.hpp"
#include "../../Type/ByteStream.hpp"
#include "../../Type/Path.hpp"

namespace $TJ::$Util::$RTON::$Encryptor {

	namespace $EnCrypt {

		extern Void enc(ByteSection const & plain, ByteSection & cipher, Str const & key);

		extern Void encExport(Path const & plainFile, Path const & cipherFile, Str const & key);

	}

	namespace $DeCrypt {

		extern Void dec(ByteSection const & cipher, ByteSection & plain, Str const & key);

		extern Void decExport(Path const & cipherFile, Path const & plainFile, Str const & key);

	}

}
