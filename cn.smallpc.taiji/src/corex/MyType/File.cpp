# include "./File.h"

# include "./Path.h"

# include "../Const.h"
# include "../UtilKit/DevUtil.h"

# include <cstdio>

auto f_fopen(char const * path, char const * mode) {
# if defined PLATFORM_WINDOWS
	if (memchr(path, ' ', f_strlen(path)) == nullptr) return fopen(path, mode);
	auto tmp = cnvPath(path);
	auto result = fopen(tmp, mode);
	delete[] tmp;
	tmp = nullptr;
	return result;
# else
	return fopen(path, mode);
# endif
}

namespace $TJ {
	using CFile = FILE;

	static constexpr SzI kFileOpenMethodSum(6);

	static Ch const * const kOpenMethodStr[kFileOpenMethodSum] = { "rb", "rb+", "wb", "wb+", "ab", "ab+" };

	Bool File::isEnd() const { return static_cast<Bool>(feof(static_cast<CFile *>(_f))); }

	File & File::close() {
		if (_f != kNullPtr) {
			fclose(static_cast<CFile *>(_f));
			_f = kNullPtr;
		}
		return *this;
	}
	File & File::open(Path const & path, OpenMethod const & method, Bool const & execErr) {
		if (method == OpenMethod::kWrite || method == OpenMethod::kWritePlus)
			$DevUtil::makeFileDir(path);
		_f = f_fopen(path.getFullPath(Str().gtStr())._str, kOpenMethodStr[static_cast<SzI>(method)]);
		if (execErr && !isOpen()) {
			wMsg("File Not Found");
			wCmt("\tCWD  : %s", $DevUtil::getCurWorkDir(Str().gtStr())._str);
			wCmt("\tPath : %s", path.getFullPath(Str().gtStr())._str);
			wErr("");
		}
		return *this;
	}

	SzI File::getPos() const { return static_cast<SzI>(ftell(static_cast<CFile *>(_f))); }

	File & File::setPosBeg(SzI const & ofs) {
		fseek(static_cast<CFile *>(_f), ofs, SEEK_SET);
		return *this;
	}
	File & File::setPosEnd(SzI const & ofs) {
		fseek(static_cast<CFile *>(_f), ofs, SEEK_END);
		return *this;
	}
	File & File::setPosCur(SzSI const & ofs) {
		fseek(static_cast<CFile *>(_f), ofs, SEEK_CUR);
		return *this;
	}
	File & File::setPosCurGo(SzI const & ofs) {
		fseek(static_cast<CFile *>(_f), +static_cast<SzSI>(ofs), SEEK_CUR);
		return *this;
	}
	File & File::setPosCurBack(SzI const & ofs) {
		fseek(static_cast<CFile *>(_f), -static_cast<SzSI>(ofs), SEEK_CUR);
		return *this;
	}

	File & File::read(Byte * const & dst, SzI const & sz, SzI const & cnt) {
		fread(dst, ternary_oper(sz == kSzNull, getSz() - getPos(), sz), cnt, static_cast<CFile *>(_f));
		return *this;
	}
	File & File::write(Byte const * const & src, SzI const & sz, SzI const & cnt) {
		fwrite(src, sz, cnt, static_cast<CFile *>(_f));
		return *this;
	}
}
