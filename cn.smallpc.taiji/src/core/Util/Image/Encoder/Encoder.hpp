#pragma once

#include "../../../Type.hpp"
#include "../Type.hpp"

namespace $TJ::$Util::$Image::$Encoder {

	ImgSz calcRealImgSz(ImgSz const & sz, TexFmt const & texFmt);
	Sz calcImgByteSz(ImgSz const & sz, TexFmt const & texFmt);

	namespace $EnCode {

		extern Void encExport(Path const & imgFile, Path const & dataFile, ImgSz const & sz, Str const & fmt);
		extern Void encByAtlasInfoExport(Path const & infoFile, Path const & imgDir, Path const & dataDir, Str const & dataFileExtension);

	}

	namespace $DeCode {

		extern Void decExport(Path const & dataFile, Path const & imgFile, ImgSz const & sz, Str const & fmt);
		extern Void decByAtlasInfoExport(Path const & infoFile, Path const & dataDir, Path const & imgDir, Str const & dataFileExtension);

	}

}
