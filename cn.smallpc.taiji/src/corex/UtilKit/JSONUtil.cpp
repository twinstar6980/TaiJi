# pragma once

# include "../Type.h"
# include "../MyType/Str.h"
# include "../MyType/Path.h"
# include "../MyType/JSON.h"

# include "../UtilKit/DevUtil.h"

namespace $TJ::$JSON {

	static Ch * gJSONStrBuf;

	namespace $IOBuf {
		Void freeBuf() {
			safeDelPtrArr(gJSONStrBuf);
			return;
		}
		Void modBufSz(SzI const & sz) {
			safeDelPtrArr(gJSONStrBuf);
			newPtr(gJSONStrBuf, sz);
			return;
		}
	}

	$JSON::Val & read(Str const & src, $JSON::Val & dst) {
		auto pos = kThFirst;
		dst.read(src._str, pos);
		return dst;
	}
	$JSON::Obj & read(Str const & src, $JSON::Obj & dst) {
		auto pos = kThSecond;
		dst.read(src._str, pos);
		return dst;
	}
	$JSON::Arr & read(Str const & src, $JSON::Arr & dst) {
		auto pos = kThSecond;
		dst.read(src._str, pos);
		return dst;
	}

	Str & write(Str & dst, $JSON::Val const & src) {
		auto len = kSzZero;
		src.write(gJSONStrBuf, len, 0);
		dst.cpy(gJSONStrBuf, len);
		return dst;
	}
	Str & write(Str & dst, $JSON::Obj const & src) {
		auto len = kSzZero;
		src.write(gJSONStrBuf, len, 0);
		dst.cpy(gJSONStrBuf, len);
		return dst;
	}
	Str & write(Str & dst, $JSON::Arr const & src) {
		auto len = kSzZero;
		src.write(gJSONStrBuf, len, 0);
		dst.cpy(gJSONStrBuf, len);
		return dst;
	}

	$JSON::Val & read(Path const & src, $JSON::Val & dst) {
		SzI sz;
		$DevUtil::ReadData(src, reinterpret_cast<Byte *&>(gJSONStrBuf), sz);
		auto pos = kThFirst;
		dst.read(gJSONStrBuf, pos);
		return dst;
	}
	$JSON::Obj & read(Path const & src, $JSON::Obj & dst) {
		SzI sz;
		$DevUtil::ReadData(src, reinterpret_cast<Byte *&>(gJSONStrBuf), sz);
		auto pos = kThSecond;
		dst.read(gJSONStrBuf, pos);
		return dst;
	}
	$JSON::Arr & read(Path const & src, $JSON::Arr & dst) {
		SzI sz;
		$DevUtil::ReadData(src, reinterpret_cast<Byte *&>(gJSONStrBuf), sz);
		auto pos = kThSecond;
		dst.read(gJSONStrBuf, pos);
		return dst;
	}

	Void write(Path const & dst, $JSON::Val const & src) {
		auto len = kSzZero;
		src.write(gJSONStrBuf, len, 0);
		$DevUtil::WriteData(dst, reinterpret_cast<Byte const * const &>(gJSONStrBuf), len);
		return;
	}
	Void write(Path const & dst, $JSON::Obj const & src) {
		auto len = kSzZero;
		src.write(gJSONStrBuf, len, 0);
		$DevUtil::WriteData(dst, reinterpret_cast<Byte const * const &>(gJSONStrBuf), len);
		return;
	}
	Void write(Path const & dst, $JSON::Arr const & src) {
		auto len = kSzZero;
		src.write(gJSONStrBuf, len, 0);
		$DevUtil::WriteData(dst, reinterpret_cast<Byte const * const &>(gJSONStrBuf), len);
		return;
	}
}
