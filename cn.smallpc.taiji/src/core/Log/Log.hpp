#pragma once

#include "../Platform.hpp"

namespace $TJ::$Log {

	struct Log {

		void * $f;
		bool $syncShow;

		Log const & txt(char const * const fmt, ...) const;
		Log const & txtln(char const * const fmt, ...) const;

		Log const & cmt(char const * const fmt, ...) const;
		Log const & msg(char const * const fmt, ...) const;
		Log const & war(char const * const fmt, ...) const;
		Log const & err(char const * const fmt, ...) const;

		Log const & empln() const {
			return txtln("");
		}

		Log & init(char const * const & path, bool const & syncShow);
		Log & free();

		~Log() {
			free();
		}
		Log() : $f(nullptr), $syncShow(false) {}
		Log(char const * const & path, bool const & syncShow) : Log() {
			init(path, syncShow);
		}

	};

	extern Log log;

}

namespace $TJ {
	using namespace $Log;
}
