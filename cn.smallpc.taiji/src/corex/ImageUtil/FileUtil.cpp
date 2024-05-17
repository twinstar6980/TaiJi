# pragma once

# include "../MyType/File.h"
# include "./ImageType.h"
# include "./FileUtil.h"

# include "../../lib/libpng/png.h"
# include "../../lib/libpng/pngstruct.h"

namespace $TJ::$ImageUtil::$FileUtil {
	namespace $PNGUtil {
		ImageSize & getSz(Path const & path, ImageSize & sz) {
			File file; file.openR(path);
			auto pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = png_create_info_struct(pngStruct);
			png_init_io(pngStruct, static_cast<decltype(fopen(nullptr, nullptr))>(file._f));
			png_read_info(pngStruct, pngInfo);
			sz.set((*pngStruct).width, (*pngStruct).height);
			png_destroy_read_struct(&pngStruct, &pngInfo, kNullPtr);
			file.close();
			return sz;
		}
		Void read(Path const & path, Bitmap & image) {
			File file; file.openR(path);
			auto pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = png_create_info_struct(pngStruct);
			png_init_io(pngStruct, static_cast<decltype(fopen(nullptr, nullptr))>(file._f));
			png_read_info(pngStruct, pngInfo);
			image.alloc(ImageSize((*pngStruct).width, (*pngStruct).height));
			png_read_image(pngStruct, image.GetColorByteRef());
			png_read_end(pngStruct, pngInfo);
			png_destroy_read_struct(&pngStruct, &pngInfo, kNullPtr);
			file.close();
			return;
		}
		Void write(Path const & path, Bitmap const & image) {
			File file; file.openW(path);
			auto pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = png_create_info_struct(pngStruct);
			png_init_io(pngStruct, static_cast<decltype(fopen(nullptr, nullptr))>(file._f));
			png_set_IHDR(pngStruct, pngInfo, image._sz._w, image._sz._h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			png_write_info(pngStruct, pngInfo);
			png_write_image(pngStruct, const_cast<Color8 **>(image.GetColorByteRef()));
			png_write_end(pngStruct, pngInfo);
			png_destroy_write_struct(&pngStruct, &pngInfo);
			file.close();
			return;
		}
		Void write(Path const & path, Bitmap const & image, ImagePos const & pos, ImageSize const & sz) {
			File file; file.openW(path);
			auto imgData = image.GetColorByteRefX(pos);
			auto pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, kNullPtr, kNullPtr, kNullPtr);
			auto pngInfo = png_create_info_struct(pngStruct);
			png_init_io(pngStruct, static_cast<decltype(fopen(nullptr, nullptr))>(file._f));
			png_set_IHDR(pngStruct, pngInfo, sz._w, sz._h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			png_write_info(pngStruct, pngInfo);
			png_write_image(pngStruct, const_cast<Color8 **>(imgData));
			png_write_end(pngStruct, pngInfo);
			png_destroy_write_struct(&pngStruct, &pngInfo);
			file.close();
			delPtrArr(imgData);
			return;
		}
	}
}
