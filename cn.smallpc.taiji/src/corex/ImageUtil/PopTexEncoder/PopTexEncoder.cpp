# include "./PopTexEncoder.h"

# include "../../MyType/File.h"

# include "../../UtilKit/DevUtil.h"

# include "../Encoder/Encoder.h"

namespace $TJ::$ImageUtil::$PopTexEncoder {
	namespace $EnCode {
		Void EnCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt) {
			return $Encoder::$EnCode::EnCode(srcFile, dstFile, sz, texFmt);
		}
		Void EnCodeAtlas(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir) {
			return $Encoder::$EnCode::EnCode(Path(srcDir).add(atlasInfo._path).setSuffix("png"), Path(dstDir).add(atlasInfo._path).setSuffix("ptx"), atlasInfo._sz, atlasInfo._texFmt);
		}
		Void EnCodeAtlasList(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir) {
			for_criter(AtlasInfo, atlasInfoList)
				EnCodeAtlas(AtlasInfo, srcDir, dstDir);
			return;
		}
	}
	namespace $DeCode {
		Void DeCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt) {
			return $Encoder::$DeCode::DeCode(srcFile, dstFile, sz, texFmt);
		}
		Void DeCodeAtlas(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir) {
			return $Encoder::$DeCode::DeCode(Path(srcDir).add(atlasInfo._path).setSuffix("ptx"), Path(dstDir).add(atlasInfo._path).setSuffix("png"), atlasInfo._sz, atlasInfo._texFmt);
		}
		Void DeCodeAtlasList(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir) {
			for_criter(AtlasInfo, atlasInfoList) {
				DeCodeAtlas(AtlasInfo, srcDir, dstDir);
				// 7.9.1 ios rsb test
				if (false && AtlasInfo._texFmt == TexFmt::kRGBAcPVRTC4BPP) {
					Bitmap image;
					$FileUtil::$PNGUtil::read(Path(dstDir).add(AtlasInfo._path).setSuffix("png"), image);
					image.ReverseY();
					$FileUtil::$PNGUtil::write(Path(dstDir).add(AtlasInfo._path).setSuffix("png"), image);
				}
			}
			return;
		}
	}
}
