# pragma once

# include "../Type.h"
# include "../MyType/Path.h"

namespace $TJ::$Entry {

	Str getCoreInfo();

	Void runEntry(StrList const & arg
	# if defined PLATFORM_WINDOWS
		, Ch const & drive = '?'
	# endif
	);

}
