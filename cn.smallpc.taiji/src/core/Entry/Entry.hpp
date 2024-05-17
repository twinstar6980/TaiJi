#pragma once

#include "../Type.hpp"
#include "../Type/Str.hpp"

namespace $TJ::$Entry {

	Str getVerName();

	Void runIntro();
	Void runOutro();

	Void runEntry(List<Str> const & arg, Str const & logDir
			  #if defined PLATFORM_ANDROID
				  , Str const & storageDir
			  #endif
	);

}
