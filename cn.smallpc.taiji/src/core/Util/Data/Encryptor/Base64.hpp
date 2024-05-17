#pragma once

#include "../../../Type.hpp"
#include "../../../Type/Path.hpp"
#include "../../../Type/ByteStream.hpp"

namespace $TJ::$Util::$Data::$Encryptor::$Base64 {

	namespace $EnCrypt {

		extern Void enc(ByteSection const & plain, ByteSection & cipher);

		extern Void encExport(Path const & plainFile, Path const & cipherFile);

	}

	namespace $DeCrypt {

		extern Void dec(ByteSection const & cipher, ByteSection & plain);

		extern Void decExport(Path const & cipherFile, Path const & plainFile);

	}

}
