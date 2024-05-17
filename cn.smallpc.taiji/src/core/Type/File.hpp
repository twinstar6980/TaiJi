#pragma once

#include "../Type.hpp"
#include "../Const.hpp"
#include "../TypeUtil.hpp"

#include "./Path.hpp"
#include "./ByteStream.hpp"

namespace $TJ {

	class File {

	protected:

		Void * $f;

	public:

		enum class OpenMethod : Sz {
			kRead = kNo1, kReadPlus = kNo2,
			kWrite = kNo3, kWritePlus = kNo4,
			kAppend = kNo5, kAppendPlus = kNo6
		};

	public:

		auto & f() { return $f; }
		auto const & f() const { return $f; }

		File & close();

		File & open(Path const & path, OpenMethod const & method, Bool const & execErr = kBoT);

		auto state() const {
			return $f != kNullPtr;
		}

		Bool eof() const;

		Size pos() const;

		Size sz() {
			assert(state());
			auto cur = pos();
			auto r = posEnd().pos();
			posBeg(cur);
			return r;
		}

		File & posBeg(Sz const & ofs = kNo1);
		File & posEnd(Sz const & ofs = kNo1);
		File & posGo(Sz const & ofs);
		File & posBack(Sz const & ofs);

		File & read(ByteSection & t, Sz const & n);

		File & readWhold(ByteSection & t) {
			posBeg();
			return read(t, sz());
		}

		File & write(ByteSection const & t);

		~File() { close(); }

		implicit File() : $f(kNullPtr) {}
		explicit File(Path const & path, OpenMethod const & method, Bool const & execErr = kBoT) :
			File() {
			open(path, method, execErr);
		}

	};

	class IFile : public File {

	public:

		implicit IFile() : File() {}
		explicit IFile(Path const & path, Bool const & execErr = kBoT) : File(path, OpenMethod::kRead, execErr) {}

		auto & open(Path const & path, Bool const & execErr = kBoT) { retwrf(File::open(path, OpenMethod::kRead, execErr)); }

	};

	class OFile : public File {

	public:

		implicit OFile() : File() {}
		explicit OFile(Path const & path, Bool const & execErr = kBoT) : File(path, OpenMethod::kWrite, execErr) {}

		auto & open(Path const & path, Bool const & execErr = kBoT) { retwrf(File::open(path, OpenMethod::kWrite, execErr)); }

	};

}
