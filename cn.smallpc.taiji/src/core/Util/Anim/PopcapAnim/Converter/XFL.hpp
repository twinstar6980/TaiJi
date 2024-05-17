#pragma once

#include "../../../../Type.hpp"
#include "../../../../Type/Path.hpp"

#include "../Struct.hpp"
#include "../Info.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter::$XFL {

	namespace $To {

		Void cnv($Info::Info const & info, Path const & xflDir, Sz const & imgSz);

	}

	namespace $From {

		$Info::Info & cnv($Info::Info & info, Path const & xflDir, Sz const & imgSz);

	}

	namespace $ScaleCnv {

		Void cnv(Path const & xflDir, Sz const & srcSz, Sz const & dstSz);

	}

}
