# pragma once

# include "../Type.h"
# include "../MyType/PairVal.h"
# include "../MyType/JSON.h"

# include "../BitVal.h"

namespace $TJ::$ImageUtil {
	using Color8 = UI8;
	using Pixel32 = Color8[4];

	constexpr Pixel32 kNullColor{ 0, 0, 0, 0 };

	constexpr SzI kImgSzZero(0);
	constexpr SzSI kSgdImageSizeZero(0);

	constexpr SzI kImgSzNull(0xFFFFFFFF);
	constexpr SzSI kSgdImageSizeNull(0x7FFFFFFF);

	constexpr SzI kSzOfA8(1);

	namespace $ColorRate {
		constexpr F64 k1A8(255.0 / 1.0);
		constexpr F64 k2A8(255.0 / 3.0);
		constexpr F64 k3A8(255.0 / 7.0);
		constexpr F64 k4A8(255.0 / 15.0);
		constexpr F64 k5A8(255.0 / 31.0);
		constexpr F64 k6A8(255.0 / 63.0);
		constexpr F64 k7A8(255.0 / 127.0);
	}

	struct ImageSize : PairVal<SzI> {
		SzI & _w;
		SzI & _h;

		auto & getThis() { return *this; }

		auto & setNull() {
			set(kImgSzNull, kImgSzNull);
			return *this;
		}
		auto & SetZero() {
			set(kImgSzZero, kImgSzZero);
			return *this;
		}

		auto getSz() const { return static_cast<SzI>(_w * _h); }
		auto GetByteSize(SzI const & bitPerPixel) const { return static_cast<SzI>((bitPerPixel * getSz()) / 8); }

		auto isNull() const { return _w == kImgSzNull || _h == kImgSzNull; }

		auto & read($JSON::Arr const & src, SzI const & widthIdx = kThFirst, SzI const & heightIdx = kThSecond) {
			src[widthIdx].getX(_w, kImgSzNull);
			src[heightIdx].getX(_h, kImgSzNull);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			dst.add(_w, _w == kImgSzNull);
			dst.add(_h, _h == kImgSzNull);
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["Width"].getX(_w, kImgSzNull);
			src["Height"].getX(_h, kImgSzNull);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Width", _w, _w == kImgSzNull);
			dst.add("Height", _h, _h == kImgSzNull);
			return *this;
		}

		ImageSize() : PairVal(kImgSzNull, kImgSzNull), _w(_left), _h(_right) { return; }
		explicit ImageSize(SzI const & width, SzI const & height) : PairVal(width, height), _w(_left), _h(_right) { return; }

		auto & operator=(ImageSize const & src) { return cpy(src); }

	};
	struct ImagePos : PairVal<SzI> {
		SzI & _x;
		SzI & _y;

		auto & getThis() { return *this; }

		auto & setNull() {
			set(kImgSzNull, kImgSzNull);
			return *this;
		}
		auto & SetZero() {
			set(kImgSzZero, kImgSzZero);
			return *this;
		}

		auto getSz() const { return static_cast<SzI>(_x * _y); }

		auto isNull() const { return _x == kImgSzNull || _y == kImgSzNull; }

		auto & read($JSON::Arr const & src, SzI const & xIdx = kThFirst, SzI const & yIdx = kThSecond) {
			src[xIdx].getX(_x, kImgSzNull);
			src[yIdx].getX(_y, kImgSzNull);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			dst.add(_x, _x == kImgSzNull);
			dst.add(_y, _y == kImgSzNull);
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["X"].getX(_x, kImgSzNull);
			src["Y"].getX(_y, kImgSzNull);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("X", _x, _x == kImgSzNull);
			dst.add("Y", _y, _y == kImgSzNull);
			return *this;
		}

		ImagePos() : PairVal(kImgSzNull, kImgSzNull), _x(_left), _y(_right) { return; }
		explicit ImagePos(SzI const & x, SzI const & y) : PairVal(x, y), _x(_left), _y(_right) { return; }

		auto & operator=(ImagePos const & src) { return cpy(src); }
		auto & operator++() {
			++_x; ++_y;
			return *this;
		}
		auto & operator--() {
			--_x; --_y;
			return *this;
		}

	};
	struct ImageSgdPos : PairVal<SzSI> {
		SzSI & _x;
		SzSI & _y;

		auto & getThis() { return *this; }

		auto & setNull() {
			set(kSgdImageSizeNull, kSgdImageSizeNull);
			return *this;
		}
		auto & SetZero() {
			set(kSgdImageSizeZero, kSgdImageSizeZero);
			return *this;
		}

		auto getSz() const { return static_cast<SzI>(_x * _y); }

		auto isNull() const { return _x == kSgdImageSizeNull || _y == kSgdImageSizeNull; }

		auto & read($JSON::Arr const & src, SzI const & xIdx = kThFirst, SzI const & yIdx = kThSecond) {
			src[xIdx].getX(_x, kSgdImageSizeNull);
			src[yIdx].getX(_y, kSgdImageSizeNull);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			dst.add(_x, _x == kSgdImageSizeNull);
			dst.add(_y, _y == kSgdImageSizeNull);
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			src["X"].getX(_x, kSgdImageSizeNull);
			src["Y"].getX(_y, kSgdImageSizeNull);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("X", _x, _x == kSgdImageSizeNull);
			dst.add("Y", _y, _y == kSgdImageSizeNull);
			return *this;
		}

		ImageSgdPos() : PairVal(kSgdImageSizeNull, kSgdImageSizeNull), _x(_left), _y(_right) { return; }
		explicit ImageSgdPos(SzI const & x, SzI const & y) : PairVal(x, y), _x(_left), _y(_right) { return; }

		auto & operator=(ImageSgdPos const & src) { return cpy(src); }
	};

	constexpr SzI kTexFmtSum(23);
	enum class TexFmt : SzI {
		kUnKnown = kThFirst,
		kNull,

		kARGBd16x4444eL,
		kARGBd32x8888eL, kARGBd32x8888eLsAd8,

		kRGBd16x565eL, kRGBAd16x5551eL, kRGBAd16x4444eL,
		kRGBd24x888eB,
		kRGBAd32x8888eB,

		kRGBcETC1, kRGBcETC1sAd8, kRGBcETC1sAcU,
		kRGBcETC2, kRGBAcETC2,

		kRGBAcPVRTC2BPP, kRGBAcPVRTC4BPP, kRGBAcPVRTC4BPPsAd8,

		kRGBcDXT1, kRGBAcDXT3, kRGBAcDXT5,

		kRGBcATITC, kRGBAcATITC,
	};
	Str const kTexFmtStr[]{
		Str("UnKnown"),
		Str("Null"),
		Str("ARGBd16x4444eL"),
		Str("ARGBd32x8888eL"),
		Str("ARGBd32x8888eLsAd8"),
		Str("RGBd16x565eL"),
		Str("RGBAd16x5551eL"),
		Str("RGBAd16x4444eL"),
		Str("RGBd24x888eB"),
		Str("RGBAd32x8888eB"),
		Str("RGBcETC1"),
		Str("RGBcETC1sAd8"),
		Str("RGBcETC1sAcU"),
		Str("RGBcETC2"),
		Str("RGBAcETC2"),
		Str("RGBAcPVRTC2BPP"),
		Str("RGBAcPVRTC4BPP"),
		Str("RGBAcPVRTC4BPPsAd8"),
		Str("RGBcDXT1"),
		Str("RGBAcDXT3"),
		Str("RGBAcDXT5"),
		Str("RGBcATITC"),
		Str("RGBAcATITC"),
	};
	inline auto GetTexFmt(Str const & str) {
		auto idx = kThFirst;
		for_criter(texFmtStr, kTexFmtStr) {
			if (texFmtStr == str) break;
			++idx;
		}
		return (idx == kTexFmtSum) ? TexFmt::kUnKnown : static_cast<TexFmt>(idx);
	}
	inline auto & GetTexFmt(TexFmt const & val) {
		return kTexFmtStr[static_cast<SzI>(val)];
	}

	struct Bitmap {
		Pixel32 ** _pixel;
		ImageSize _sz;

		auto isNull() const { return _pixel == kNullPtr; }

		Pixel32 *& operator[](SzI const & y) {
			return _pixel[y];
		}
		Pixel32 const * const & operator[](SzI const & y) const {
			return _pixel[y];
		}
		Pixel32 & operator[](ImagePos const & pos) {
			return _pixel[pos._y][pos._x];
		}
		Pixel32 const & operator[](ImagePos const & pos) const {
			return _pixel[pos._y][pos._x];
		}

		auto & alloc(ImageSize const & sz) {
			newPtrLv2ArrArr(_pixel, sz._h, sz._w);
			_sz.cpy(sz);
			return *this;
		}
		auto & tfree() {
			if (!isNull()) {
				delPtrArrArr(_pixel, _sz._h);
				_sz.setNull();
			}
			return *this;
		}

		auto & clear() {
			for_cnt(y, kThFirst, _sz._h)
				memSet(_pixel[y], _sz._w);
			return *this;
		}

		SzI getSz(SzI const & bitPerPixel = 32u) const {
			return (bitPerPixel * _sz.getSz()) / 8;
		}

		Byte **& GetColorByteRef() {
			return reinterpret_cast<Byte **&>(_pixel);
		}
		Byte const * const * const & GetColorByteRef() const {
			return reinterpret_cast<Byte const * const * const &>(_pixel);
		}
		Byte const * const * GetColorByteRefX(ImagePos const & pos) const {
			Byte const ** dst(kNullPtr);
			newPtr(dst, _sz._h - pos._y);
			for_cnt(y, pos._y, _sz._h)
				dst[y - pos._y] = reinterpret_cast<Byte const *>(_pixel[y] + pos._x);
			return dst;
		}

		Byte ** GetColorByte() const {
			Byte ** dst(kNullPtr);
			newPtrLv2ArrArr(dst, szOf<Pixel32>(_sz._h), szOf<Pixel32>(_sz._w));
			for_cnt(y, kThFirst, _sz._h)
				memCpy(dst[y], _pixel[y], szOf<Pixel32>(_sz._w));
			return dst;
		}
		Byte ** GetColorByte(ImagePos const & pos) const {
			Byte ** dst(kNullPtr);
			newPtrLv2ArrArr(dst, szOf<Pixel32>(_sz._h - pos._y), szOf<Pixel32>(_sz._w - pos._x));
			for_cnt(y, pos._y, _sz._h)
				memCpy(dst[y - pos._y], _pixel[y], szOf<Pixel32>(_sz._w - pos._x));
			return dst;
		}

		auto & ReverseY() {
			for (SzI i = kThFirst; i < _sz._h / 2; ++i)
				swpPtr(_pixel[i], _pixel[_sz._h - kThSecond - i]);
			return *this;
		}

		auto & Set_A8() {
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][3] = 255;
				}
			}
			return *this;
		}
		auto & Set_A8(Byte const * const & data) {
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][3] = data[(y * _sz._w) + x];
				}
			}
			return *this;
		}

		auto & Set_RGBd16x565eL(Byte const * const & dataOri) {
			auto const & data = reinterpret_cast<UI16 const *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][0] = ((data[(y * _sz._w) + x] & BIN_16_15_06_05) >> 11) * $ColorRate::k5A8;
					_pixel[y][x][1] = ((data[(y * _sz._w) + x] & BIN_16_05_16_05) >> 5) * $ColorRate::k6A8;
					_pixel[y][x][2] = ((data[(y * _sz._w) + x] & BIN_16_05_06_15) >> 0) * $ColorRate::k5A8;
				}
			}
			return *this;
		}
		auto & Set_RGBAd16x5551eL(Byte const * const & dataOri) {
			auto const & data = reinterpret_cast<UI16 const *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][0] = ((data[(y * _sz._w) + x] & BIN_16_15_05_05_01) >> 11) * $ColorRate::k5A8;
					_pixel[y][x][1] = ((data[(y * _sz._w) + x] & BIN_16_05_15_05_01) >> 6) * $ColorRate::k5A8;
					_pixel[y][x][2] = ((data[(y * _sz._w) + x] & BIN_16_05_05_15_01) >> 1) * $ColorRate::k5A8;
					_pixel[y][x][3] = ((data[(y * _sz._w) + x] & BIN_16_05_05_05_11) >> 0) * $ColorRate::k1A8;
				}
			}
			return *this;
		}
		auto & Set_RGBAd16x4444eL(Byte const * const & dataOri) {
			auto const & data = reinterpret_cast<UI16 const *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][0] = ((data[(y * _sz._w) + x] & BIN_16_14_04_04_04) >> 12) * $ColorRate::k4A8;
					_pixel[y][x][1] = ((data[(y * _sz._w) + x] & BIN_16_04_14_04_04) >> 8) * $ColorRate::k4A8;
					_pixel[y][x][2] = ((data[(y * _sz._w) + x] & BIN_16_04_04_14_04) >> 4) * $ColorRate::k4A8;
					_pixel[y][x][3] = ((data[(y * _sz._w) + x] & BIN_16_04_04_04_14) >> 0) * $ColorRate::k4A8;
				}
			}
			return *this;
		}
		auto & Set_RGBd24x888eB(Byte const * const & dataOri) {
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][0] = dataOri[((y * _sz._w) + x) * 3 + 0];
					_pixel[y][x][1] = dataOri[((y * _sz._w) + x) * 3 + 1];
					_pixel[y][x][2] = dataOri[((y * _sz._w) + x) * 3 + 2];
				}
			}
			return *this;
		}
		auto & Set_RGBAd32x8888eB(Byte const * const & data) {
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][0] = data[((y * _sz._w) + x) * 4 + 0];
					_pixel[y][x][1] = data[((y * _sz._w) + x) * 4 + 1];
					_pixel[y][x][2] = data[((y * _sz._w) + x) * 4 + 2];
					_pixel[y][x][3] = data[((y * _sz._w) + x) * 4 + 3];
				}
			}
			return *this;
		}

		auto & Set_ARGBd16x4444eL(Byte const * const & dataOri) {
			auto const & data = reinterpret_cast<UI16 const *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][3] = ((data[(y * _sz._w) + x] & BIN_16_14_04_04_04) >> 12) * $ColorRate::k4A8;
					_pixel[y][x][0] = ((data[(y * _sz._w) + x] & BIN_16_04_14_04_04) >> 8) * $ColorRate::k4A8;
					_pixel[y][x][1] = ((data[(y * _sz._w) + x] & BIN_16_04_04_14_04) >> 4) * $ColorRate::k4A8;
					_pixel[y][x][2] = ((data[(y * _sz._w) + x] & BIN_16_04_04_04_14) >> 0) * $ColorRate::k4A8;
				}
			}
			return *this;
		}
		auto & Set_ARGBd32x8888eL(Byte const * const & dataOri) {
			auto const & data = reinterpret_cast<UI32 const *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					_pixel[y][x][3] = (data[(y * _sz._w) + x] & BIN_32_18_08_08_08) >> 24;
					_pixel[y][x][0] = (data[(y * _sz._w) + x] & BIN_32_08_18_08_08) >> 16;
					_pixel[y][x][1] = (data[(y * _sz._w) + x] & BIN_32_08_08_18_08) >> 8;
					_pixel[y][x][2] = (data[(y * _sz._w) + x] & BIN_32_08_08_08_18) >> 0;
				}
			}
			return *this;
		}

		auto & Get_A8(Byte * const & data) const {
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x] = _pixel[y][x][3];
				}
			}
			return *this;
		}

		auto & Get_RGBd16x565eL(Byte * const & dataOri) const {
			auto const & data = reinterpret_cast<UI16 *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x]
						= (static_cast<UI16>(_pixel[y][x][0] / $ColorRate::k5A8) << 11)
						| (static_cast<UI16>(_pixel[y][x][1] / $ColorRate::k6A8) << 5)
						| (static_cast<UI16>(_pixel[y][x][2] / $ColorRate::k5A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_RGBAd16x5551eL(Byte * const & dataOri) const {
			auto const & data = reinterpret_cast<UI16 *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x]
						= (static_cast<UI16>(_pixel[y][x][0] / $ColorRate::k5A8) << 11)
						| (static_cast<UI16>(_pixel[y][x][1] / $ColorRate::k5A8) << 6)
						| (static_cast<UI16>(_pixel[y][x][2] / $ColorRate::k5A8) << 1)
						| (static_cast<UI16>(_pixel[y][x][3] / $ColorRate::k1A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_RGBAd16x4444eL(Byte * const & dataOri) const {
			auto const & data = reinterpret_cast<UI16 *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x]
						= (static_cast<UI16>(_pixel[y][x][0] / $ColorRate::k4A8) << 12)
						| (static_cast<UI16>(_pixel[y][x][1] / $ColorRate::k4A8) << 8)
						| (static_cast<UI16>(_pixel[y][x][2] / $ColorRate::k4A8) << 4)
						| (static_cast<UI16>(_pixel[y][x][3] / $ColorRate::k4A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_RGBd24x888eB(Byte * const & data) const {
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[((y * _sz._w) + x) * 3 + 0] = _pixel[y][x][0];
					data[((y * _sz._w) + x) * 3 + 1] = _pixel[y][x][1];
					data[((y * _sz._w) + x) * 3 + 2] = _pixel[y][x][2];
				}
			}
			return *this;
		}
		auto & Get_RGBAd32x8888eB(Byte * const & dataOri) const {
			auto const & data = reinterpret_cast<UI32 *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x]
						= cnvEndian(static_cast<UI32>(_pixel[y][x][0] << 24)
									| static_cast<UI32>(_pixel[y][x][1] << 16)
									| static_cast<UI32>(_pixel[y][x][2] << 8)
									| static_cast<UI32>(_pixel[y][x][3] << 0));
				}
			}
			return *this;
		}

		auto & Get_ARGBd16x4444eL(Byte * const & dataOri) const {
			auto const & data = reinterpret_cast<UI16 *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x]
						= (static_cast<UI16>(_pixel[y][x][3] / $ColorRate::k4A8) << 12)
						| (static_cast<UI16>(_pixel[y][x][0] / $ColorRate::k4A8) << 8)
						| (static_cast<UI16>(_pixel[y][x][1] / $ColorRate::k4A8) << 4)
						| (static_cast<UI16>(_pixel[y][x][2] / $ColorRate::k4A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_ARGBd32x8888eL(Byte * const & dataOri) const {
			auto const & data = reinterpret_cast<UI32 *>(dataOri);
			for_cnt(y, kThFirst, _sz._h) {
				for_cnt(x, kThFirst, _sz._w) {
					data[(y * _sz._w) + x]
						= static_cast<UI32>(_pixel[y][x][3] << 24)
						| static_cast<UI32>(_pixel[y][x][0] << 16)
						| static_cast<UI32>(_pixel[y][x][1] << 8)
						| static_cast<UI32>(_pixel[y][x][2] << 0);
				}
			}
			return *this;
		}

		auto & cpy(Bitmap const & src, ImagePos const & srcPos, ImageSize const & sz, ImagePos const & dstPos) {
			for_cnt(y, kThFirst, sz._h)
				memCpy(_pixel[y + dstPos._y] + dstPos._x, src[y + srcPos._y] + srcPos._x, sz._w);
			return *this;
		}

		~Bitmap() {
			tfree();
			return;
		}
		Bitmap() : _pixel(kNullPtr), _sz() { return; }
		explicit Bitmap(ImageSize const & sz) : Bitmap() {
			alloc(sz);
			return;
		}
	};
}
