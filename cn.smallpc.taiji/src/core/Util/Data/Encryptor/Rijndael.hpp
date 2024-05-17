#pragma once

#include "../../../Type.hpp"
#include "../../../Type/ByteStream.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$Data::$Encryptor::$Rijndael {

	namespace $EnCrypt {

		extern Void enc(Section<IU32> const & plain, Section<IU32> & cipher, Sz const & blockSz, Sz const & keySz, Str const & key, Str const & iv);

		extern Void encExport(Path const & plainFile, Path const & cipherFile, Sz const & blockSz, Sz const & keySz, Str const & key, Str const & iv);

	}

	namespace $DeCrypt {

		extern Void dec(Section<IU32> const & cipher, Section<IU32> & plain, Sz const & blockSz, Sz const & keySz, Str const & key, Str const & iv);

		extern Void decExport(Path const & cipherFile, Path const & plainFile, Sz const & blockSz, Sz const & keySz, Str const & key, Str const & iv);

	}

}
