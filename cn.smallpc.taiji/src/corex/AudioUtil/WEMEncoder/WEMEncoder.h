# pragma once

# include "../../UtilKit/DataBlockUtil.h"
# include "../../UtilKit/JSONUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../../MyType/Path.h"

namespace $TJ::$AudioUtil::$WEMEncoder {
	Void WEMDeCodeToOGG(Path const & srcFilePath, Path const & dstFilePath);
	Void RWave(Path const & path);
}
