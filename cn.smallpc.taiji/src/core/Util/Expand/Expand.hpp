#pragma once

#include "../../Type.hpp"
#include "../../Type/Path.hpp"

namespace $TJ::$Util::$Expand {

	namespace $PvZ2LawnStringText {

		extern Void upgradeExport(Path const & oldStyleFile, Path const & newStyleFile);

		extern Void downgradeExport(Path const & newStyleFile, Path const & oldStyleFile);

	}

	namespace $RSBSMF {

		extern Void compressExport(Path const & rawFile, Path const & ripeFile);

		extern Void uncompressExport(Path const & ripeFile, Path const & rawFile);

	}

	namespace $RSBDataDisturb {

		extern Void disturb(ByteSection & pkg);

		extern Void disturbExport(Path const & srcFile, Path const & dstFile);

	}

	extern Void renameFileCaseByRSBInfoX(Path const & infoFile, Path const & tgtDir);

	extern Void mixWwiseSoundBankHIRC(Path const & srcAFile, Path const & srcBFile, Path const & dstFile);

	extern Void removeUnityAssetHeader(Path const & srcFile, Path const & dstFile);

}
