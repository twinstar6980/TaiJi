#pragma once

#include "../../Type.hpp"
#include "../../Const.hpp"
#include "../../TypeUtil.hpp"
#include "../../Type/List.hpp"
#include "../../Type/Str.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/File.hpp"

namespace $TJ::$Util::$File {

	FileType getType(Path const & path);

	inline auto existPath(Path const & path) {
		return getType(path) != FileType::kNotExist;
	}
	inline auto existFile(Path const & path) {
		return getType(path) == FileType::kFile;
	}
	inline auto existDir(Path const & path) {
		return getType(path) == FileType::kDir;
	}

	Void cpy(Path const & src, Path const & dst);
	Void remove(Path const & path);
	Void rename(Path const & dir, Str const & oldName, Str const & newName);

	inline auto remove(List<Path> const & list) {
		list.range().each([](auto & v) { remove(v); });
		return;
	}

	Str & getCurDir(Str & path);
	Path & getCurDir(Path & path);

	Void chgDirToRoot();

	Void makeDir(Path const & path);
	Void chgDir(Path const & path);
	inline auto chgDirForce(Path const & path) {
		makeDir(path);
		chgDir(path);
		return;
	}

	inline auto makeDirForFile(Path const & path) {
		if (path.$sz > kSz1)
			makeDir(path.dir());
		return;
	}
	inline auto chgDirForFile(Path const & path) {
		if (path.$sz > kSz1)
			chgDir(path.dir());
		return;
	}
	inline auto chgDirForceForFile(Path const & path) {
		if (path.$sz > kSz1)
			chgDirForce(path.dir());
		return;
	}

	inline auto getCurDirThenChgDir(Path & curDir, Path const & gotoDir) {
		getCurDir(curDir);
		chgDir(gotoDir);
		return;
	}
	inline auto getCurDirThenChgDirForce(Path & curDir, Path const & gotoDir) {
		getCurDir(curDir);
		chgDirForce(gotoDir);
		return;
	}
	inline auto chgDirThenFree(Path & dir) {
		chgDir(dir);
		dir.free();
		return;
	}

	Sz getPathCnt(Path const & path);
	Sz getPathCntOfFile(Path const & path);
	Sz getPathCntOfDir(Path const & path);

	List<Path> & getPathList(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst = kBoT);
	List<Path> & getPathListOfFile(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst = kBoT);
	List<Path> & getPathListOfDir(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst = kBoT);

	inline auto getFileSize(Path const & path) {
		return IFile(path).sz();
	}

	// need empty byte-section
	inline auto readFile(Path const & path, ByteSection & dst) {
		assert(dst.isEmpty());
		IFile(path).readWhold(dst).close();
		return;
	}
	// alloc a new byte-section
	inline auto readFileForce(Path const & path, ByteSection & dst) {
		assert(dst.isNull());
		IFile f(path);
		dst.alloc(f.sz());
		f.readWhold(dst).close();
		return;
	}
	inline auto writeFile(Path const & path, ByteSection const & data) {
		assert(!data.isNull());
		OFile(path).posBeg().write(data);
		return;
	}

#if defined PLATFORM_WINDOWS

	DriveLetter getCurDrive();
	Void chgDrive(DriveLetter const & drive);

#endif

}
