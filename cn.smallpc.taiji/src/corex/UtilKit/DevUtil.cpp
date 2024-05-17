# include "./DevUtil.h"

# include <cstdio>
# include <cwchar>

# include "../Type.h"
# include "../MyType/Path.h"
# include "../MyType/File.h"
# include "../SomethingElse.h"

# if defined PLATFORM_WINDOWS

# include <direct.h>

char * cnvPath(char const * src) {
	auto dst = new char[f_strlen(src) + 1];
	auto tmp = dst;
	dst[f_strlen(src)] = '\0';
	while (*src != '\0') {
		if ((*src == ' ') && ((*(src + 1) == '/') || (*(src + 1) == '\\') || (*(src + 1) == '\0')))
			*tmp = '#';
		else *tmp = *src;
		++src; ++tmp;
	}
	tmp = nullptr;
	return dst;
}

#include <windows.h>
#include <io.h>

inline auto f_chdir(char const * path) {
	if (memchr(path, ' ', strlen(path)) == nullptr) return _chdir(path);
	auto tmp = cnvPath(path);
	auto result = _chdir(tmp);
	delete[] tmp;
	tmp = nullptr;
	return result;
}
inline auto f_mkdir(char const * path) {
	if (memchr(path, ' ', strlen(path)) == nullptr) return _mkdir(path);
	char * tmp = cnvPath(path);
	int result = _mkdir(tmp);
	delete[] tmp;
	tmp = nullptr;
	return result;
}
inline auto f_rmdir(char const * path) {
	if (memchr(path, ' ', strlen(path)) == nullptr) return _rmdir(path);
	char * tmp = cnvPath(path);
	int result = _rmdir(tmp);
	delete[] tmp;
	tmp = nullptr;
	return result;
}
inline auto f_getcwd(char * str, int len) { return _getcwd(str, len); }

# elif defined PLATFORM_LINUX

# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>

inline int f_mkdir(char const * absPath) { return mkdir(absPath, 0777); }
inline int f_chdir(char const * absPath) { return chdir(absPath); }
inline int f_rmdir(char const * absPath) { return rmdir(absPath); }
inline char * f_getcwd(char * str, int len) { return getcwd(str, len); }

# endif

namespace $TJ::$DevUtil {

# if defined PLATFORM_WINDOWS

	Ch getCurDrive() {
		static Ch cwd[260];
		_getcwd(cwd, 260);
		return cwd[kThFirst];
	}
	Void chgDrive(Ch const & letterCh) {
		Ch letter[]{ letterCh, ':', '\\', '\0' };
		f_chdir(letter);
		return;
	}
# endif

	Void chgDirToRoot() {
		f_chdir(kRootPathStr._str);
		return;
	}

	Str & getCurWorkDir(Str & dst) {
		static Ch tmp[$MaxStrLenDef::kPathStr + kSzOne];
		f_getcwd(tmp, $MaxStrLenDef::kPathStr);
		dst = getPathStr(tmp);
		return dst;
	}
	Path & getCurWorkDir(Path & dst) {
		static Ch tmp[$MaxStrLenDef::kPathStr + kSzOne];
		f_getcwd(tmp, $MaxStrLenDef::kPathStr);
		dst.parse(getPathStr(tmp));
		return dst;
	}

	Path const & makeDir(Path const & path, Bool const & isFile) {
		Str s(path.getStrLenRaw());
		for_cnt(i, kThFirst, path.lastIdx()) {
			s.append(path[i]);
			f_mkdir(s._str);
			s.append($PathDelimiter::kNativeStyle);
		}
		if (!isFile) {
			s.append(path.last());
			f_mkdir(s._str);
		}
		s.tfree();
		return path;
	}

	Path const & chgDir(Path const & path, Bool const & isFile) {
		f_chdir(isFile ? path.getDirPath(Str().gtStr())._str : path.getFullPath(Str().gtStr())._str);
		return path;
	}

	Path const & forceChgDir(Path const & path, Bool const & isFile) {
		makeDir(path, isFile);
		chgDir(path, isFile);
		return path;
	}

	Path & makeDir(Path & path, Bool const & isFile) {
		makeDir(static_cast<Path const>(path), isFile);
		return path;
	}
	Path & chgDir(Path & path, Bool const & isFile) {
		chgDir(static_cast<Path const>(path), isFile);
		return path;
	}
	Path & forceChgDir(Path & path, Bool const & isFile) {
		forceChgDir(static_cast<Path const>(path), isFile);
		return path;
	}

	Path & getCurWorkDirThenChgDir(Path & workDir, Path const & goToDir) {
		getCurWorkDir(workDir);
		chgDir(goToDir);
		return workDir;
	}
	Path & getCurWorkDirThenForceChgDir(Path & workDir, Path const & goToDir) {
		getCurWorkDir(workDir);
		forceChgDir(goToDir);
		return workDir;
	}
	Path & chgDirThenFree(Path & dir) {
		chgDir(dir);
		dir.tfree();
		return dir;
	}

	Bool isDir(Path const & path) {
	# if defined PLATFORM_WINDOWS
		WIN32_FIND_DATAW findData;
		auto unicPath = $ChSet::UTF8ToUTF16(path.getFullPath(Str().gtStr())._str);
		auto handle = FindFirstFileW(unicPath, &findData);
		delPtrArr(unicPath);
		auto result = findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY;
		FindClose(handle);
		return result;
	# elif defined PLATFORM_LINUX
		auto dir = opendir(path.getFullPath(Str().gtStr())._str);
		if (dir != kNullPtr) {
			closedir(dir);
			return kTrue;
		}
		return kFalse;
	# endif
	}

	Void readData(Path const & path, Byte * const & data, SzI & sz) {
		File f;
		f.openR(path);
		sz = f.getSz();
		f.setPosBeg().read(data, sz).close();
		return;
	}
	Void forceReadData(Path const & path, Byte *& data, SzI & sz) {
		File f;
		f.openR(path);
		sz = f.getSz();
		newPtr(data, sz);
		f.setPosBeg().read(data, sz).close();
		return;
	}
	Void writeData(Path const & path, Byte const * const & data, SzI const & sz) {
		File().openW(path).setPosBeg().write(data, sz);
		return;
	}

	Void renameFile(Path const & srcPath, Path const & dstPath) {
		rename(srcPath.getFullPath(Str().gtStr())._str, dstPath.getFullPath(Str().gtStr())._str);
		return;
	}
	Void removFile(Path const & path) {
		remove(path.getFullPath(Str().gtStr())._str);
		return;
	}
	Void cpyFile(Path const & srcPath, Path const & dstPath) {
		File srcFile;
		Byte * srcData(kNullPtr);
		srcFile.openR(srcPath);
		auto srcSz = srcFile.getSz();
		newPtr(srcData, srcSz);
		srcFile.setPosBeg().read(srcData, srcSz).close();
		File().openW(dstPath).setPosBeg().write(srcData, srcSz);
		delPtrArr(srcData);
		return;
	}

	Void removFileByList(ListP<Path> const & list) {
		for_criter(e, list)
			removFile(e);
		return;
	}

	ListP<Path> & getPathList(Path const & absPath, ListP<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		static Path parentPath;
		if (isFirst)
			if (cpyParent) parentPath.cpy(absPath);
			else parentPath.setNull();
		else parentPath.append(absPath.last());
	# if defined PLATFORM_WINDOWS
		WIN32_FIND_DATAW findData;
		auto unciPath = $ChSet::UTF8ToUTF16(absPath.getFullPath(Str().gtStr()).append($PathDelimiter::kNativeStyle).append($Char::kStarKey)._str);
		auto handle = FindFirstFileW(unciPath, &findData);
		delPtrArr(unciPath);
		while (FindNextFileW(handle, &findData) != 0) {
			Str fileName;
			fileName._str = $ChSet::UTF16ToUTF8(findData.cFileName);
			fileName._len = f_strlen(fileName._str);
			if (fileName != "." && fileName != "..") {
				dst.appendSz().last().append(parentPath).append(fileName);
				if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					getPathList(Path(absPath).append(fileName), dst, cpyParent, kFalse);
			}
		}
	# elif defined PLATFORM_LINUX
		dirent * dirent(kNullPtr);
		auto dir = opendir(absPath.getFullPath(Str().getThis())._str);
		if (dir != kNullPtr) {
			while ((dirent = readdir(dir)) != NULL)
				if (f_strcmp(dirent->d_name, ".") != 0 && f_strcmp(dirent->d_name, "..") != 0) {
					dst.add().last().add(parentPath).add(dirent->d_name);
					if (dirent->d_type == DT_DIR)
						getPathList(Path(absPath).add(dirent->d_name), dst, cpyParent, kFalse);
				}
			closedir(dir);
		}
	# endif
		if (isFirst) parentPath.setNull();
		else parentPath.eraseLast();
		return dst;
	}
	ListP<Path> & getDirPathList(Path const & absPath, ListP<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		static Path parentPath;
		if (isFirst)
			if (cpyParent) parentPath.cpy(absPath);
			else parentPath.setNull();
		else parentPath.append(absPath.last());
	# if defined PLATFORM_WINDOWS
		WIN32_FIND_DATAW findData;
		auto unciPath = $ChSet::UTF8ToUTF16(absPath.getFullPath(Str().gtStr()).append($PathDelimiter::kNativeStyle).append($Char::kStarKey)._str);
		auto handle = FindFirstFileW(unciPath, &findData);
		delPtrArr(unciPath);
		while (FindNextFileW(handle, &findData) != 0) {
			Str fileName;
			fileName._str = $ChSet::UTF16ToUTF8(findData.cFileName);
			fileName._len = f_strlen(fileName._str);
			if (fileName != "." && fileName != ".." && findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
				dst.appendSz().last().append(parentPath).append(fileName);
				getDirPathList(Path(absPath).append(fileName), dst, cpyParent, kFalse);
			}
		}
		FindClose(handle);
	# elif defined PLATFORM_LINUX
		dirent * dirent(kNullPtr);
		auto dir = opendir(absPath.getFullPath(Str().getThis())._str);
		if (dir != kNullPtr) {
			while ((dirent = readdir(dir)) != NULL)
				if (f_strcmp(dirent->d_name, ".") != 0 && f_strcmp(dirent->d_name, "..") != 0)
					if (dirent->d_type == DT_DIR) {
						dst.add().last().add(parentPath).add(dirent->d_name);
						getDirPathList(Path(absPath).add(dirent->d_name), dst, cpyParent, kFalse);
					}
			closedir(dir);
		}
	# endif
		if (isFirst) parentPath.setNull();
		else parentPath.eraseLast();
		return dst;
	}
	ListP<Path> & getFilePathList(Path const & absPath, ListP<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		static Path parentPath;
		if (isFirst)
			if (cpyParent) parentPath.cpy(absPath);
			else parentPath.setNull();
		else parentPath.append(absPath.last());
	# if defined PLATFORM_WINDOWS
		WIN32_FIND_DATAW findData;
		auto unciPath = $ChSet::UTF8ToUTF16(absPath.getFullPath(Str().gtStr()).append($PathDelimiter::kNativeStyle).append($Char::kStarKey)._str);
		auto handle = FindFirstFileW(unciPath, &findData);
		delPtrArr(unciPath);
		while (FindNextFileW(handle, &findData) != 0) {
			Str fileName;
			fileName._str = $ChSet::UTF16ToUTF8(findData.cFileName);
			fileName._len = f_strlen(fileName._str);
			if (fileName != "." && fileName != "..")
				if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					getFilePathList(Path(absPath).append(fileName), dst, cpyParent, kFalse);
				else dst.appendSz().last().append(parentPath).append(fileName);
		}
		FindClose(handle);
	# elif defined PLATFORM_LINUX
		dirent * dirent(kNullPtr);
		auto dir = opendir(absPath.getFullPath(Str().getThis())._str);
		if (dir != kNullPtr) {
			while ((dirent = readdir(dir)) != NULL)
				if (f_strcmp(dirent->d_name, ".") != 0 && f_strcmp(dirent->d_name, "..") != 0)
					if (dirent->d_type == DT_DIR)
						getFilePathList(Path(absPath).add(dirent->d_name), dst, cpyParent, kFalse);
					else dst.add().last().add(parentPath).add(dirent->d_name);
			closedir(dir);
		}
	# endif
		if (isFirst) parentPath.setNull();
		else parentPath.eraseLast();
		return dst;
	}
	Void ReNameDir(Path const & srcPath, Path const & dstPath) {
	# if defined PLATFORM_WINDOWS/*
		SHFILEOPSTRUCTW fileOp;
		memSet(&fileOp, kSzOne);
		fileOp.hNameMappings = kNullPtr;
		fileOp.hwnd = kNullPtr;
		fileOp.lpszProgressTitle = kNullPtr;
		fileOp.pFrom = UTF8ToUTF16(srcPath.getFullPath(Str().getThis())._str);
		fileOp.pTo = UTF8ToUTF16(dstPath.getFullPath(Str().getThis())._str);
		//wprintf(L"%s\n%s\n", fileOp.pFrom, fileOp.pTo);
		fileOp.wFunc = FO_RENAME;
		fileOp.fFlags = FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION;
		SHFileOperationW(&fileOp);*/

		SHFILEOPSTRUCTA fileOp;
		memSet(&fileOp, kSzOne);
		fileOp.hNameMappings = kNullPtr;
		fileOp.hwnd = kNullPtr;
		fileOp.lpszProgressTitle = kNullPtr;
		fileOp.pFrom = $ChSet::UTF8ToANSI(srcPath.getFullPath(Str().getThis())._str);
		fileOp.pTo = $ChSet::UTF8ToANSI(dstPath.getFullPath(Str().getThis())._str);
		//wprintf(L"%s\n%s\n", fileOp.pFrom, fileOp.pTo);
		fileOp.wFunc = FO_RENAME;
		fileOp.fFlags = FOF_SILENT | FOF_RENAMEONCOLLISION | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOERRORUI | FOF_NO_UI;
		//fileOp.fFlags = FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NORECURSION;
		SHFileOperationA(&fileOp);
	# elif defined PLATFORM_LINUX
	# endif
		return;
	}
	Void CpyDir(Path const & srcPath, Path const & dstPath) {
	# if defined PLATFORM_WINDOWS
		WIN32_FIND_DATAW findData;
		auto unciPath = $ChSet::UTF8ToUTF16(srcPath.getFullPath(Str().getThis()).cat($PathDelimiter::kNativeStyle).cat($Char::kStarKey)._str);
		auto handle = FindFirstFileW(unciPath, &findData);
		delPtrArr(unciPath);
		while (FindNextFileW(handle, &findData) != 0) {
			Str fileName;
			fileName._str = $ChSet::UTF16ToUTF8(findData.cFileName);
			fileName._len = f_strlen(fileName._str);
			if (fileName != "." && fileName != "..") {
				Path srcSubPath, dstSubPath;
				srcSubPath.cpy(srcPath).add(fileName);
				dstSubPath.cpy(dstPath).add(fileName);
				if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					CpyDir(srcSubPath, dstSubPath);
				else CpyFile(srcSubPath, dstSubPath);
				srcSubPath.tfree(); dstSubPath.tfree();
			}
		}
		FindClose(handle);
	# elif defined PLATFORM_LINUX
		dirent * dirent(kNullPtr);
		auto dir = opendir(srcPath.getFullPath(Str().getThis())._str);
		if (dir != kNullPtr) {
			while ((dirent = readdir(dir)) != NULL)
				if (f_strcmp(dirent->d_name, ".") != 0 && f_strcmp(dirent->d_name, "..") != 0) {
					Path srcSubPath, dstSubPath;
					srcSubPath.cpy(srcPath).add(dirent->d_name);
					dstSubPath.cpy(dstPath).add(dirent->d_name);
					if (dirent->d_type == DT_DIR)
						CpyDir(srcSubPath, dstSubPath);
					else CpyFile(srcSubPath, dstSubPath);
					srcSubPath.tfree(); dstSubPath.tfree();
				}
			closedir(dir);
		}
	# endif
		return;
	}
	Void ReMovDir(Path const & path) {
	# if defined PLATFORM_WINDOWS
		WIN32_FIND_DATAW findData;
		auto unciPath = $ChSet::UTF8ToUTF16(path.getFullPath(Str().getThis()).cat($PathDelimiter::kNativeStyle).cat($Char::kStarKey)._str);
		auto handle = FindFirstFileW(unciPath, &findData);
		delPtrArr(unciPath);
		while (FindNextFileW(handle, &findData) != 0) {
			Str fileName;
			fileName._str = $ChSet::UTF16ToUTF8(findData.cFileName);
			fileName._len = f_strlen(fileName._str);
			if (fileName != "." && fileName != "..") {
				Path subPath;
				subPath.cpy(path).add(fileName);
				if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					ReMovDir(subPath);
				else ReMovFile(subPath);
				subPath.tfree();
			}
		}
		FindClose(handle);
	# elif defined PLATFORM_LINUX
		dirent * dirent(kNullPtr);
		auto dir = opendir(path.getFullPath(Str().getThis())._str);
		if (dir != kNullPtr) {
			while ((dirent = readdir(dir)) != NULL) {
				if (f_strcmp(dirent->d_name, ".") != 0 && f_strcmp(dirent->d_name, "..") != 0) {
					Path subPath;
					subPath.cpy(path).add(dirent->d_name);
					if (dirent->d_type == DT_DIR)
						ReMovDir(subPath);
					else ReMovFile(subPath);
					subPath.tfree();
				}
			}
			closedir(dir);
		}
	# endif
		f_rmdir(path.getFullPath(Str().getThis())._str);
		return;
	}
}
