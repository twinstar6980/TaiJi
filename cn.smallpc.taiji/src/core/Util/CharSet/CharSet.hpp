#pragma once

#include "../../Type.hpp"

namespace $TJ::$Util::$CharSet {

#if defined PLATFORM_WINDOWS

	Ch * cnvUTF16ToUTF8(ChW const * const & src);
	ChW * cnvUTF8ToUTF16(Ch const * const & src);

	Ch * cnvUTF8ToANSI(Ch const * const & src);
	Ch * cnvANSIToUTF8(Ch const * const & src);

#endif

}
