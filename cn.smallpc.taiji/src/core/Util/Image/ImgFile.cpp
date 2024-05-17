#include "./ImgFile.hpp"

#include "../../Type/File.hpp"

#include "../../../dependency/libpng/png.h"
#include "../../../dependency/libpng/pngstruct.h"

namespace $TJ::$Util::$Image::$ImgFile {

	namespace $PNG {

		ImgSz getSz(Path const & path) {
			ImgSz sz;
			IFile file(path);
			auto pngStruct = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = ::png_create_info_struct(pngStruct);
			::png_init_io(pngStruct, static_cast<::png_FILE_p>(file.f()));
			::png_read_info(pngStruct, pngInfo);
			sz.set((*pngStruct).width, (*pngStruct).height);
			::png_destroy_read_struct(&pngStruct, &pngInfo, kNullPtr);
			file.close();
			return sz;
		}

		Void read(Path const & path, Bitmap & img) {
			assert(img.isNull());
			IFile file(path);
			auto pngStruct = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = ::png_create_info_struct(pngStruct);
			::png_init_io(pngStruct, static_cast<::png_FILE_p>(file.f()));
			::png_read_info(pngStruct, pngInfo);
			img.alloc(ImgSz((*pngStruct).width, (*pngStruct).height));
			::png_read_image(pngStruct, img.getColorByteRef());
			::png_read_end(pngStruct, pngInfo);
			::png_destroy_read_struct(&pngStruct, &pngInfo, kNullPtr);
			file.close();
			return;
		}

		Void write(Path const & path, Bitmap const & img, ImgArea const & area) {
			OFile file(path);
			Bitmap realImg;
			realImg.bind(img, area);
			auto imgData = realImg.getColorByteRef();
			auto pngStruct = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = ::png_create_info_struct(pngStruct);
			::png_init_io(pngStruct, static_cast<::png_FILE_p>(file.f()));
			::png_set_IHDR(pngStruct, pngInfo, realImg.$sz.$w, realImg.$sz.$h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
			::png_write_info(pngStruct, pngInfo);
			::png_write_image(pngStruct, const_cast<Color8 **>(imgData));
			::png_write_end(pngStruct, pngInfo);
			::png_destroy_write_struct(&pngStruct, &pngInfo);
			file.close();
			realImg.unbind();
			pFreeN(imgData);
			return;
		}

	}

}
