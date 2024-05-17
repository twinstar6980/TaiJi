# include "./Spliter.h"
# include "../FileUtil.h"
# include "../../UtilKit/DevUtil.h"

namespace $TJ::$ImageUtil::$Spliter {
	namespace $Cat {
		Void cat(ImageSize const & atlasSz, ListP<ImageInfo> const & imageInfoList, Path const & srcDir, Path const & dstFile) {
			Bitmap atlas, image;
			atlas.alloc(atlasSz).clear();
			Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, srcDir);
			for_criter(imageInfo, imageInfoList) {
				$FileUtil::$PNGUtil::read(Path(imageInfo._path).setSuffix("png"), image);
				atlas.cpy(image, ImagePos(kThFirst, kThFirst), imageInfo._sz, imageInfo._pos);
				image.tfree();
			}
			$DevUtil::chgDirThenFree(parentDir);
			$FileUtil::$PNGUtil::write(dstFile, atlas);
			atlas.tfree();
			return;
		}
		Void cat(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			return cat(atlasInfo._sz, atlasInfo._img, useSingleDir ? Path(srcDir).add(atlasInfo._path) : srcDir, Path(dstDir).add(atlasInfo._path).setSuffix("png"));
		}
		Void cat(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			for_criter(atlasInfo, atlasInfoList)
				cat(atlasInfo, srcDir, dstDir, useSingleDir);
			return;
		}
	}
	namespace $Split {
		Void Split(ListP<ImageInfo> const & imageInfoList, Path const & srcFile, Path const & dstDir) {
			Bitmap atlas, image;
			$FileUtil::$PNGUtil::read(srcFile, atlas);
			Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, dstDir);
			for_criter(imageInfo, imageInfoList)
				$FileUtil::$PNGUtil::write(Path(imageInfo._path).setSuffix("png"), atlas, imageInfo._pos, imageInfo._sz);
			$DevUtil::chgDirThenFree(parentDir);
			return;
		}
		Void Split(AtlasInfo const & atlasInfo, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			return Split(atlasInfo._img, Path(srcDir).add(atlasInfo._path).setSuffix("png"), useSingleDir ? Path(dstDir).add(atlasInfo._path) : dstDir);
		}
		Void Split(ListP<AtlasInfo> const & atlasInfoList, Path const & srcDir, Path const & dstDir, Bool const & useSingleDir) {
			for_criter(atlasInfo, atlasInfoList)
				Split(atlasInfo, srcDir, dstDir, useSingleDir);
			return;
		}
	}
}
