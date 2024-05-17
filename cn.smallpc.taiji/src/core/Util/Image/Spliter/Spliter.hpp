#pragma once

#include "../../../Type.hpp"
#include "../Type.hpp"
#include "../InfoType.hpp"

namespace $TJ::$Util::$Image::$Spliter {

	namespace $Cat {

		extern Void catExport(ImgSz const & sz, Path const & infoFile, Path const & imgDir, Path const & atlasFile);
		extern Void catByAtlasInfoExport(Path const & infoFile, Path const & imgDir, Path const & atlasDir, Bool const & useSingleDir);

	}

	namespace $Split {

		extern Void splitExport(ImgSz const & sz, Path const & infoFile, Path const & atlasFile, Path const & imgDir);
		extern Void splitByAtlasInfoExport(Path const & infoFile, Path const & atlasDir, Path const & imgDir, Bool const & useSingleDir);

	}

}
