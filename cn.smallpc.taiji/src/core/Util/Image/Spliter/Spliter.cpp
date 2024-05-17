#include "./Spliter.hpp"

#include "../ImgFile.hpp"
#include "../../File/File.hpp"

namespace $TJ::$Util::$Image::$Spliter {

	namespace $Cat {

		Void cat(List<ImageInfo> const & info, Bitmap & atlas, Path const & imgDir) {
			Path parentDir; $File::getCurDirThenChgDir(parentDir, imgDir);
			for (auto const & e : info) {
				Bitmap img;
				$ImgFile::$PNG::read(Path(e.$path).setExtension("png"_r), img);
				atlas.cpy(img, ImgPos(kNo1, kNo1), e.$area.$sz, e.$area.$pos);
				img.free();
			}
			$File::chgDirThenFree(parentDir);
			return;
		}
		Void cat(ImgSz const & sz, List<ImageInfo> const & info, Path const & imgDir, Path const & atlasFile) {
			Bitmap atlas;
			atlas.alloc(sz);
			cat(info, atlas, imgDir);
			$ImgFile::$PNG::write(atlasFile, atlas);
			atlas.free();
			return;
		}
		Void cat(AtlasInfo const & info, Path const & imgDir, Path const & atlasDir, Bool const & useSingleDir) {
			if (useSingleDir)
				return cat(info.$sz, info.$img, imgDir + info.$path, (atlasDir + info.$path).setExtension("png"_r));
			else
				return cat(info.$sz, info.$img, imgDir, (atlasDir + info.$path).setExtension("png"_r));
		}
		Void cat(List<AtlasInfo> const & info, Path const & imgDir, Path const & atlasDir, Bool const & useSingleDir) {
			for (auto const & e : info)
				cat(e, imgDir, atlasDir, useSingleDir);
			return;
		}

		extern Void catExport(ImgSz const & sz, Path const & infoFile, Path const & imgDir, Path const & atlasFile) {
			$JSON::Val info;
			$JSON::read(infoFile, info);
			cat(sz, InfoList<ImageInfo>().read(info), imgDir, atlasFile);
			return;
		}
		extern Void catByAtlasInfoExport(Path const & infoFile, Path const & imgDir, Path const & atlasDir, Bool const & useSingleDir) {
			$JSON::Val info;
			$JSON::read(infoFile, info);
			if (info.isArr())
				cat(InfoList<AtlasInfo>().read(info), imgDir, atlasDir, useSingleDir);
			else
				cat(AtlasInfo().read(info), imgDir, atlasDir, useSingleDir);
			return;
		}

	}

	namespace $Split {

		Void split(List<ImageInfo> const & info, Bitmap const & atlas, Path const & imgDir) {
			Path parentDir; $File::getCurDirThenChgDirForce(parentDir, imgDir);
			for (auto const & e : info)
				$ImgFile::$PNG::write(Path(e.$path).setExtension("png"_r), atlas, e.$area);
			$File::chgDirThenFree(parentDir);
			return;
		}
		Void split(ImgSz const & sz, List<ImageInfo> const & info, Path const & atlasFile, Path const & imgDir) {
			Bitmap atlas;
			$ImgFile::$PNG::read(atlasFile, atlas);
			split(info, atlas, imgDir);
			atlas.free();
			return;
		}
		Void split(AtlasInfo const & info, Path const & atlasDir, Path const & imgDir, Bool const & useSingleDir) {
			if (useSingleDir)
				return split(info.$sz, info.$img, (atlasDir + info.$path).setExtension("png"_r), imgDir + info.$path);
			else
				return split(info.$sz, info.$img, (atlasDir + info.$path).setExtension("png"_r), imgDir);
		}
		Void split(List<AtlasInfo> const & info, Path const & atlasDir, Path const & imgDir, Bool const & useSingleDir) {
			for (auto const & e : info)
				split(e, atlasDir, imgDir, useSingleDir);
			return;
		}

		extern Void splitExport(ImgSz const & sz, Path const & infoFile, Path const & atlasFile, Path const & imgDir) {
			$JSON::Val info;
			$JSON::read(infoFile, info);
			split(sz, InfoList<ImageInfo>().read(info), atlasFile, imgDir);
			return;
		}
		extern Void splitByAtlasInfoExport(Path const & infoFile, Path const & atlasDir, Path const & imgDir, Bool const & useSingleDir) {
			$JSON::Val info;
			$JSON::read(infoFile, info);
			if (info.isArr())
				split(InfoList<AtlasInfo>().read(info), atlasDir, imgDir, useSingleDir);
			else
				split(AtlasInfo().read(info), atlasDir, imgDir, useSingleDir);
			return;
		}

	}

}
