#pragma once

#include <stdexcept>

namespace $TJ {

	struct Exception {

	protected:

		std::string $tag;
		std::string $msg;

	public:

		std::string what() const { return "{ " + $tag + " } : " + $msg; }

		Exception() : $tag(), $msg() {}
		Exception(std::string const & tag, std::string const & msg) : $tag(tag), $msg(msg) {}

	};

}
