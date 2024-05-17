#pragma once

#include "../../../Type.hpp"
#include "../../../Type/Path.hpp"

namespace $TJ::$Util::$Audio::$WwiseEncodedMedia::$Encoder {

	extern Str gFFMPEGFilePath;
	extern Str gWW2OGGFilePath;
	extern Str gWW2OGGPCBFilePath;

	namespace $DeCode {

		Void decExport(Path const & encodedFile, Path const & audioFile);

	}

}
