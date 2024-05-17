#pragma once

#include "../../Type.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/Path.hpp"

#include "./Type.hpp"

namespace $TJ::$Util::$Image::$ImgFile {

	namespace $PNG {

		ImgSz getSz(Path const & path);

		Void read(Path const & path, Bitmap & img);

		Void write(Path const & path, Bitmap const & img, ImgArea const & area = ImgArea());

	}

}
