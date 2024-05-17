#pragma once

#include "../Type.hpp"
#include "../Type/List.hpp"
#include "../Type/Str.hpp"
#include "../Type/Path.hpp"

namespace $TJ::$Executor {

	Void exec(Path const & initDir, Str const & cfgSrc, List<Str> const & arg);

}
