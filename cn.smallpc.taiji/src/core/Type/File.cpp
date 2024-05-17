#include "../Type.hpp"
#include "../Const.hpp"
#include "../TypeUtil.hpp"

#include "./Str.hpp"
#include "./Path.hpp"
#include "./File.hpp"

#include "../Util/File/File.hpp"
#include "../Log/Log.hpp"

#include <cstdio>

namespace $TJ {

	namespace {

		using CFile = std::FILE;

		inline auto frd_fopen(Str const & path, ConstCStr const & mode) {
		#if defined PLATFORM_WINDOWS
			return std::fopen(isLegalPath(path) ? path.$data : makeLegalPath(path).$data, mode);
		#else
			return std::fopen(path.$data, mode);
		#endif
		}

		static Char const * const kOpenMethodStr[] = { "rb", "rb+", "wb", "wb+", "ab", "ab+" };

	}

	File & File::close() {
		if (state()) {
			std::fclose(static_cast<CFile *>($f));
			$f = kNullPtr;
		}
		return thiz;
	}
	File & File::open(Path const & path, OpenMethod const & method, Bool const & execErr) {
		if (method == OpenMethod::kWrite || method == OpenMethod::kWritePlus)
			$Util::$File::makeDirForFile(path);
		$f = frd_fopen(path.fullName(), kOpenMethodStr[static_cast<Sz>(method)]);
		if (execErr && !state())
			throw VAMGException("FileSystem", "file-open-failed : \n\tcwd  : %s\n\tpath : %s",
								$Util::$File::getCurDir(crv(Str())).$data, path.fullName().$data);
		return thiz;
	}

	Bool File::eof() const {
		assert(state());
		return static_cast<Bool>(std::feof(static_cast<CFile *>($f)));
	}

	Size File::pos() const {
		assert(state());
		return static_cast<Sz>(std::ftell(static_cast<CFile *>($f)));
	}

	File & File::posBeg(Sz const & ofs) {
		assert(state());
		std::fseek(static_cast<CFile *>($f), ofs, SEEK_SET);
		return thiz;
	}
	File & File::posEnd(Sz const & ofs) {
		assert(state());
		std::fseek(static_cast<CFile *>($f), ofs, SEEK_END);
		return thiz;
	}
	File & File::posGo(Sz const & ofs) {
		assert(state());
		std::fseek(static_cast<CFile *>($f), +static_cast<SSz>(ofs), SEEK_CUR);
		return thiz;
	}
	File & File::posBack(Sz const & ofs) {
		assert(state());
		std::fseek(static_cast<CFile *>($f), -static_cast<SSz>(ofs), SEEK_CUR);
		return thiz;
	}

	File & File::read(ByteSection & t, Sz const & n) {
		assert(state());
		assert(t.isEmpty() && n <= t.$cap && n <= sz() - pos());
		std::fread(t.$data, n, kSz1, static_cast<CFile *>($f));
		t.setSz(n);
		return thiz;
	}
	File & File::write(ByteSection const & t) {
		assert(state());
		assert(!t.isNull());
		std::fwrite(t.$data, t.$sz, kSz1, static_cast<CFile *>($f));
		return thiz;
	}

}
