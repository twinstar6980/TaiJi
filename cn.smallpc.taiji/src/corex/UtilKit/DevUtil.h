# pragma once

# include "../Type.h"
# include "../MyType/Str.h"
# include "../MyType/Path.h"
# include "../MyType/File.h"

# if defined PLATFORM_WINDOWS
char * cnvPath(char const * src);
#endif

namespace $TJ::$DevUtil {

# if defined PLATFORM_WINDOWS

	Ch getCurDrive();
	Void chgDrive(Ch const & letterCh);

# endif

	Void chgDirToRoot();

	Str & getCurWorkDir(Str & dst);
	Path & getCurWorkDir(Path & dst);

	Path const & makeDir(Path const & path, Bool const & isFile = kFalse);
	Path const & chgDir(Path const & path, Bool const & isFile = kFalse);
	Path const & forceChgDir(Path const & path, Bool const & isFile = kFalse);

	inline auto & makeFileDir(Path const & path) { return makeDir(path, kTrue); }
	inline auto & chgFileDir(Path const & path) { return chgDir(path, kTrue); }
	inline auto & forceChgFileDir(Path const & path) { return forceChgDir(path, kTrue); }

	Path & makeDir(Path & path, Bool const & isFile = kFalse);
	Path & chgDir(Path & path, Bool const & isFile = kFalse);
	Path & forceChgDir(Path & path, Bool const & isFile = kFalse);

	inline auto & makeFileDir(Path & path) { return makeDir(path, kTrue); }
	inline auto & chgFileDir(Path & path) { return chgDir(path, kTrue); }
	inline auto & forceChgFileDir(Path & path) { return forceChgDir(path, kTrue); }

	Path & getCurWorkDirThenChgDir(Path & workDir, Path const & goToDir);
	Path & getCurWorkDirThenForceChgDir(Path & workDir, Path const & goToDir);
	Path & chgDirThenFree(Path & dir);

	inline auto existFile(Path const & path) { return File().openR(path, kFalse).isOpen(); }
	inline auto getFileSize(Path const & path) { return File().openR(path).getSz(); }

	Bool isDir(Path const & path);
	inline auto isFile(Path const & path) { return !isDir(path); }

	Void readData(Path const & path, Byte * const & data, SzI & sz);
	Void forceReadData(Path const & path, Byte *& data, SzI & sz);
	Void writeData(Path const & path, Byte const * const & data, SzI const & sz);

	Void renameFile(Path const & srcPath, Path const & dstPath);
	Void removFile(Path const & path);
	Void cpyFile(Path const & srcPath, Path const & dstPath);

	Void removFileByList(ListP<Path> const & list);

	ListP<Path> & getPathList(Path const & absPath, ListP<Path> & dst, Bool const & cpyParent, Bool const & isFirst = kTrue);
	ListP<Path> & getDirPathList(Path const & absPath, ListP<Path> & dst, Bool const & cpyParent, Bool const & isFirst = kTrue);
	ListP<Path> & getFilePathList(Path const & absPath, ListP<Path> & dst, Bool const & cpyParent, Bool const & isFirst = kTrue);

	Void renameDir(Path const & srcPath, Path const & dstPath);
	Void cpyDir(Path const & srcPath, Path const & dstPath);
	Void removDir(Path const & path);
}
