#pragma once

#include "../../../Type.hpp"
#include "../../../Type/Path.hpp"

#include "./Struct.hpp"
#include "./Info.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter {

	namespace $ToXFL {

		extern Void cnvExport(Path const & infoFile, Path const & xflDir, Sz const & imgSz);

		extern Void cnvOneStepExport(Path const & dataFile, Path const & xflDir, Sz const & imgSz);

	}

	namespace $FromXFL {

		extern Void cnvExport(Path const & infoFile, Path const & xflDir, Sz const & imgSz);

		extern Void cnvOneStepExport(Path const & dataFile, Path const & xflDir, Sz const & imgSz, Sz const & memSz);

	}

	namespace $XFLScaleCnv {

		extern Void cnvExport(Path const & xflDir, Sz const & srcSz, Sz const & dstSz);

	}

	namespace $ToCSSInfo {

		extern Void cnvExport(Path const & infoFile, Path const & cssInfoFile);

		extern Void cnvOneStepExport(Path const & dataFile, Path const & cssInfoFile);

	}

}
