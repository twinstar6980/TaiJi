# pragma once

# include "./Path.h"

# include "../Const.h"

namespace $TJ {
	struct File {
		Void * _f;

		enum class OpenMethod : SzI {
			kRead = kThFirst, kReadPlus = kThSecond,
			kWrite = kThThird, kWritePlus = kThFourth,
			kAppend = kThFifth, kAppendPlus = kThSixth
		};

		auto isOpen() const { return _f != kNullPtr; }

		Bool isEnd() const;

		File & close();
		File & open(Path const & path, OpenMethod const & method, Bool const & execErr = kTrue);
		auto & openR(Path const & path, Bool const & execErr = kTrue) { return open(path, OpenMethod::kRead, execErr); }
		auto & openW(Path const & path, Bool const & execErr = kTrue) { return open(path, OpenMethod::kWrite, execErr); }

		SzI getPos() const;

		auto getSz() {
			auto cur = getPos();
			auto sz = setPosEnd().getPos();
			setPosBeg(cur);
			return sz;
		}

		File & setPosBeg(SzI const & ofs = kThFirst);
		File & setPosEnd(SzI const & ofs = kThFirst);
		File & setPosCur(SzSI const & ofs);
		File & setPosCurGo(SzI const & ofs);
		File & setPosCurBack(SzI const & ofs);

		File & read(Byte * const & dst, SzI const & sz = kSzNull, SzI const & cnt = kSzOne);
		File & write(Byte const * const & src, SzI const & sz, SzI const & cnt = kSzOne);

		~File() { close(); }
		File() : _f(kNullPtr) {}

	};
}
