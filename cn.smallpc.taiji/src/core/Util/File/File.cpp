#include "./File.hpp"

#include "../../Type.hpp"
#include "../../Const.hpp"
#include "../../TypeUtil.hpp"
#include "../../Type/List.hpp"
#include "../../Type/Str.hpp"
#include "../../Type/Path.hpp"
#include "../../Type/File.hpp"
#include "../Other/Other.hpp"

#if defined USE_STD_FILESYSTEM

#include <filesystem>

namespace fs = std::filesystem;

#else

#include "../../../dependency/boost/filesystem.hpp"

namespace fs = boost::filesystem;

#endif

namespace $TJ::$Util::$File {

	namespace {

		inline auto chgDirWithValidate(Str const & s) {
			if (s.isEmpty())
				return;
			return fs::current_path(isLegalPath(s) ? s.$data : makeLegalPath(s).$data);
		}
		inline auto makeDirWithValidate(Str const & s) {
			if (s.isEmpty())
				return kBoF;
			return fs::create_directories(isLegalPath(s) ? s.$data : makeLegalPath(s).$data);
		}
		inline auto renameWithValidate(Path const & dir, Str const & oldName, Str const & newName) {
		#if defined PLATFORM_ANDROID
			if (!$Str::cmpIgnoreCase(oldName, newName)) {
				fs::rename((dir + oldName).fullName().$data, (dir + newName).fullName().$data);
			} else {
				while (kBoT) {
					auto rand = randomNum32();
					auto tmpPath = dir + $Str::printFmt("%u", rand);
					if (fs::exists(tmpPath.fullName().$data))
						continue;
					fs::rename((dir + oldName).fullName().$data, tmpPath.fullName().$data);
					fs::rename(tmpPath.fullName().$data, (dir + newName).fullName().$data);
					break;
				}
			}
		#else
			fs::rename((dir + oldName).fullName().$data, (dir + newName).fullName().$data);
		#endif
			return;
		}

	}

	inline auto getType(fs::path const & path) {
		auto status = fs::status(fs::path(path));
		if (!fs::exists(status))
			return FileType::kNotExist;
		else if (fs::is_directory(status))
			return FileType::kDir;
		else if (fs::is_regular_file(status))
			return FileType::kFile;
		else
			return FileType::kOther;
	}
	inline auto getType(Str const & path) {
		return getType(fs::path(path.$data));
	}
	FileType getType(Path const & path) {
		if (path.isNull() || path.isEmpty())
			return FileType::kNotExist;
		return getType(path.fullName());
	}

	Void cpy(Path const & src, Path const & dst) {
		makeDirForFile(dst);
		fs::copy(src.fullName().$data, dst.fullName().$data, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		return;
	}
	Void remove(Path const & path) {
		fs::remove_all(path.fullName().$data);
		return;
	}
	Void rename(Path const & dir, Str const & oldName, Str const & newName) {
		if (dir.isEmpty() || existDir(dir)) {
			switch (getType(dir + oldName)) {
				default:
					log.msg("can not remove beacuse unknown");
					break;
				case FileType::kNotExist:
					log.msg("can not rename beacuse not-exist");
					break;
				case FileType::kFile:
				case FileType::kDir:
					renameWithValidate(dir, oldName, newName);
					break;
				case FileType::kOther:
					log.msg("can not remove beacuse other");
					break;
			}
		} else {
			log.msg("dir not exist");
			log.msg("cwd : %s", getCurDir(crv(Str())).$data);
			log.msg("dir : %s", dir.fullName().$data);
		}
		return;
	}

	Str & getCurDir(Str & path) {
		auto s = fs::current_path().string();
		path.cpy(s.c_str(), s.size());
		return path;
	}
	Path & getCurDir(Path & path) {
		return path.parse(getCurDir(crv(Str())));
	}

	Void chgDirToRoot() {
		chgDirWithValidate(kRootDir.fullName());
		return;
	}

	Void makeDir(Path const & path) {
		makeDirWithValidate(path.fullName());
		return;
	}
	Void chgDir(Path const & path) {
		chgDirWithValidate(path.fullName());
		return;
	}

	enum class FileSelectOption : IU8 { kAll, kFile, kDir };

	template <FileSelectOption option>
	Sz getPathCntProto(Path const & path) {
		if (!existDir(path))
			return kSzN;
		auto cnt = kSzN;
		for (auto & f : fs::directory_iterator(path.fullName().$data)) {
			Str fn;
			auto fnOri = f.path().filename().string();
			cpyCStr(fn, fnOri.c_str(), fnOri.size());
			if constexpr (option == FileSelectOption::kAll) {
				++cnt;
				if (fs::is_directory(f)) {
					cnt += getPathCntProto<option>(path + fn);
				}
			} else if constexpr (option == FileSelectOption::kFile) {
				if (fs::is_directory(f)) {
					cnt += getPathCntProto<option>(path + fn);
				} else {
					++cnt;
				}
			} else if constexpr (option == FileSelectOption::kDir) {
				if (fs::is_directory(f)) {
					++cnt;
					cnt += getPathCntProto<option>(path + fn);
				}
			} else {
				static_assert_failed("error option");
			}
		}
		return cnt;
	}
	Sz getPathCnt(Path const & path) {
		return getPathCntProto<FileSelectOption::kAll>(path);
	}
	Sz getPathCntOfFile(Path const & path) {
		return getPathCntProto<FileSelectOption::kFile>(path);
	}
	Sz getPathCntOfDir(Path const & path) {
		return getPathCntProto<FileSelectOption::kDir>(path);
	}

	template <FileSelectOption option>
	List<Path> & getPathListProto(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		static Path parentPath;
		if (isFirst) {
			dst.alloc(getPathCntProto<option>(path));
			if (cpyParent)
				parentPath = path;
			else
				parentPath.alloc();
			if (!existDir(path))
				return dst;
		} else {
			parentPath.append(path.last());
		}
		for (auto & f : fs::directory_iterator(path.fullName().$data)) {
			Str fn;
			auto fnOri = f.path().filename().string();
			cpyCStr(fn, fnOri.c_str(), fnOri.size());
			if constexpr (option == FileSelectOption::kAll) {
				dst.append(parentPath + fn);
				if (fs::is_directory(f)) {
					getPathListProto<option>(path + fn, dst, cpyParent, kBoF);
				}
			} else if constexpr (option == FileSelectOption::kFile) {
				if (fs::is_directory(f)) {
					getPathListProto<option>(path + fn, dst, cpyParent, kBoF);
				} else {
					dst.append(parentPath + fn);
				}
			} else if constexpr (option == FileSelectOption::kDir) {
				if (fs::is_directory(f)) {
					dst.append(parentPath + fn);
					getPathListProto<option>(path + fn, dst, cpyParent, kBoF);
				}
			} else {
				static_assert_failed("error option");
			}
		}
		if (isFirst)
			parentPath.free();
		else
			parentPath.eraseFromEnd();
		return dst;
	}

	List<Path> & getPathList(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		return getPathListProto<FileSelectOption::kAll>(path, dst, cpyParent, isFirst);
	}
	List<Path> & getPathListOfFile(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		return getPathListProto<FileSelectOption::kFile>(path, dst, cpyParent, isFirst);
	}
	List<Path> & getPathListOfDir(Path const & path, List<Path> & dst, Bool const & cpyParent, Bool const & isFirst) {
		return getPathListProto<FileSelectOption::kDir>(path, dst, cpyParent, isFirst);
	}

#if defined PLATFORM_WINDOWS

	DriveLetter getCurDrive() {
		return DriveLetter(getCurDir(crv(Str())).first());
	}
	Void chgDrive(DriveLetter const & drive) {
		chgDirWithValidate(drive.toStr() + $PathDelimiter::kOnWindows);
		return;
	}

#endif

}
