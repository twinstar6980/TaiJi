#pragma once

#include "../../Type.hpp"
#include "../../Type/JSON.hpp"
#include "../../Type/ImgType.hpp"
#include "../../UserLiteral.hpp"

namespace $TJ::$Util::$Image {

	using Color8 = IU8;
	using Pixel32 = Color8[4];

	inline constexpr Pixel32 kNullColor{ 0, 0, 0, 0 };

	inline constexpr Sz kSzOfA8(1);

	namespace $ColorRate {

		inline constexpr F64 k1A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("1"_bin8));
		inline constexpr F64 k2A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("11"_bin8));
		inline constexpr F64 k3A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("111"_bin8));
		inline constexpr F64 k4A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("1111"_bin8));
		inline constexpr F64 k5A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("11111"_bin8));
		inline constexpr F64 k6A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("111111"_bin8));
		inline constexpr F64 k7A8(static_cast<F64>("11111111"_bin8) / static_cast<F64>("1111111"_bin8));

	}

	inline constexpr Sz kTexFmtSum(27);

	enum class TexFmt : Sz {

		kNull = kNo1,

		// rgbx non-compress
		kRGBAd16x4444eL, kRGBd16x565eL, kRGBAd16x5551eL,

		kRGBd24x888eB,
		kRGBAd32x8888eB,

		// argb non-compress
		kARGBd16x4444eL,
		kARGBd32x8888eL, kARGBd32x8888eLsAd8,

		// rgbx non-compress
		kRGBcETC1, kRGBcETC1sAd8,
		kRGBcETC2, kRGBAcETC2,
		kRGBAcPVRTC2BPP, kRGBAcPVRTC4BPP, kRGBAcPVRTC4BPPsAd8,
		kRGBcDXT1, kRGBAcDXT3, kRGBAcDXT5,
		kRGBcATITC, kRGBAcATITC,

		// 0x20 series
		kRGBAd16x4444eLmSpecialOfPvZ1RSB, kRGBd16x565eLmSpecialOfPvZ1RSB, kRGBAd16x5551eLmSpecialOfPvZ1RSB,
		kARGBd32x8888eLmSpecialOfPvZ1RSB,

		// special
		kAdVmSpecialOfPvZ2CHSAndroid,
		kRGBcETC1sAdVmSpecialOfPvZ2CHSAndroid

	};

	inline Str const kTexFmtStr[]{
		Str(),

		Str("RGBAd16x4444eL"_r),
		Str("RGBd16x565eL"_r),
		Str("RGBAd16x5551eL"_r),

		Str("RGBd24x888eB"_r),
		Str("RGBAd32x8888eB"_r),

		Str("ARGBd16x4444eL"_r),
		Str("ARGBd32x8888eL"_r),
		Str("ARGBd32x8888eLsAd8"_r),

		Str("RGBcETC1"_r),
		Str("RGBcETC1sAd8"_r),

		Str("RGBcETC2"_r),
		Str("RGBAcETC2"_r),

		Str("RGBAcPVRTC2BPP"_r),
		Str("RGBAcPVRTC4BPP"_r),
		Str("RGBAcPVRTC4BPPsAd8"_r),

		Str("RGBcDXT1"_r),
		Str("RGBAcDXT3"_r),
		Str("RGBAcDXT5"_r),

		Str("RGBcATITC"_r),
		Str("RGBAcATITC"_r),

		Str("RGBAd16x4444eLmSpecialOfPvZ1RSB"_r),
		Str("RGBd16x565eLmSpecialOfPvZ1RSB"_r),
		Str("RGBAd16x5551eLmSpecialOfPvZ1RSB"_r),
		Str("ARGBd32x8888eLmSpecialOfPvZ1RSB"_r),

		Str("AdVmSpecialOfPvZ2CHSAndroid"_r),
		Str("RGBcETC1sAdVmSpecialOfPvZ2CHSAndroid"_r),
	};

	inline auto getTexFmt(Str const & s) {
		auto i = Range(&*kTexFmtStr, kTexFmtSum).findBy(
			[&s](auto & v) {
				return v == s;
			}
		);
		return i == kNoX ? TexFmt::kNull : static_cast<TexFmt>(i);
	}
	inline auto & getTexFmt(TexFmt const & i) {
		return kTexFmtStr[static_cast<Sz>(i)];
	}

	struct Bitmap;

	namespace $PixelCast {

		template <TexFmt fmt>
		inline constexpr auto from = [](Pixel32 & pixel, Byte const * const & data) {
			if constexpr (fmt == TexFmt::kRGBAd16x4444eL) {
				auto dataV16 = *reinterpret_cast<IU16 const * const>(data);
				pixel[0] = ((dataV16 & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
				pixel[1] = ((dataV16 & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
				pixel[2] = ((dataV16 & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
				pixel[3] = ((dataV16 & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
			} else if constexpr (fmt == TexFmt::kRGBd16x565eL) {
				auto dataV16 = *reinterpret_cast<IU16 const * const>(data);
				pixel[0] = ((dataV16 & "11111-000000-00000"_bin16) >> 11) * $ColorRate::k5A8;
				pixel[1] = ((dataV16 & "00000-111111-00000"_bin16) >> 5) * $ColorRate::k6A8;
				pixel[2] = ((dataV16 & "00000-000000-11111"_bin16) >> 0) * $ColorRate::k5A8;
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eL) {
				auto dataV16 = *reinterpret_cast<IU16 const * const>(data);
				pixel[0] = ((dataV16 & "11111-00000-00000-0"_bin16) >> 11) * $ColorRate::k5A8;
				pixel[1] = ((dataV16 & "00000-11111-00000-0"_bin16) >> 6) * $ColorRate::k5A8;
				pixel[2] = ((dataV16 & "00000-00000-11111-0"_bin16) >> 1) * $ColorRate::k5A8;
				pixel[3] = ((dataV16 & "00000-00000-00000-1"_bin16) >> 0) * $ColorRate::k1A8;
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				pixel[0] = data[0];
				pixel[1] = data[1];
				pixel[2] = data[2];
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB) {
				pixel[0] = data[0];
				pixel[1] = data[1];
				pixel[2] = data[2];
				pixel[3] = data[3];
			} else if constexpr (fmt == TexFmt::kARGBd16x4444eL) {
				auto dataV16 = *reinterpret_cast<IU16 const * const>(data);
				pixel[3] = ((dataV16 & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
				pixel[0] = ((dataV16 & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
				pixel[1] = ((dataV16 & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
				pixel[2] = ((dataV16 & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eL) {
				pixel[3] = data[3];
				pixel[0] = data[2];
				pixel[1] = data[1];
				pixel[2] = data[0];
			} else {
				static_assert_failed("unknown format");
			}
			return;
		};
		template <TexFmt fmt>
		inline constexpr auto fromWithPos = [](Pixel32 & pixel, Byte const * const & data, Sz const & pos) {
			if constexpr (fmt == TexFmt::kRGBd16x565eL ||
						  fmt == TexFmt::kRGBAd16x5551eL ||
						  fmt == TexFmt::kRGBAd16x4444eL ||
						  fmt == TexFmt::kARGBd16x4444eL) {
				from<fmt>(pixel, data + pos * 2);
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				from<fmt>(pixel, data + pos * 3);
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB ||
								 fmt == TexFmt::kARGBd32x8888eL) {
				from<fmt>(pixel, data + pos * 4);
			} else {
				static_assert_failed("unknown format");
			}
			return;
		};

		inline constexpr auto cnvAlphaXXMapperX(IU8 const & oriVal) {
			constexpr IU8 kMapper[16] = {
				0x0, 0x1, 0x8, 0x6,
				0x4, 0x7, 0x2, 0x3,
				0xF, 0xD, 0x5, 0xA,
				0xE, 0xB, 0x9, 0xC
			};
			return kMapper[oriVal];
		};

		template <TexFmt fmt>
		inline constexpr auto to = [](Pixel32 const & pixel, Byte * const & data) {
			if constexpr (fmt == TexFmt::kRGBAd16x4444eL) {
				*reinterpret_cast<IU16 * const>(data)
					= (static_cast<IU16>(pixel[0] / $ColorRate::k4A8) << 12)
					| (static_cast<IU16>(pixel[1] / $ColorRate::k4A8) << 8)
					| (static_cast<IU16>(pixel[2] / $ColorRate::k4A8) << 4)
					| (static_cast<IU16>(pixel[3] / $ColorRate::k4A8) << 0);
			} else if constexpr (fmt == TexFmt::kRGBd16x565eL) {
				*reinterpret_cast<IU16 * const>(data)
					= (static_cast<IU16>(pixel[0] / $ColorRate::k5A8) << 11)
					| (static_cast<IU16>(pixel[1] / $ColorRate::k6A8) << 5)
					| (static_cast<IU16>(pixel[2] / $ColorRate::k5A8) << 0);
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eL) {
				*reinterpret_cast<IU16 * const>(data)
					= (static_cast<IU16>(pixel[0] / $ColorRate::k5A8) << 11)
					| (static_cast<IU16>(pixel[1] / $ColorRate::k5A8) << 6)
					| (static_cast<IU16>(pixel[2] / $ColorRate::k5A8) << 1)
					| (static_cast<IU16>(pixel[3] / $ColorRate::k1A8) << 0);
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				data[0] = pixel[0];
				data[1] = pixel[1];
				data[2] = pixel[2];
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB) {
				data[0] = pixel[0];
				data[1] = pixel[1];
				data[2] = pixel[2];
				data[3] = pixel[3];
			} else if constexpr (fmt == TexFmt::kARGBd16x4444eL) {
				*reinterpret_cast<IU16 * const>(data)
					= (static_cast<IU16>(pixel[3] / $ColorRate::k4A8) << 12)
					| (static_cast<IU16>(pixel[0] / $ColorRate::k4A8) << 8)
					| (static_cast<IU16>(pixel[1] / $ColorRate::k4A8) << 4)
					| (static_cast<IU16>(pixel[2] / $ColorRate::k4A8) << 0);
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eL) {
				data[0] = pixel[2];
				data[1] = pixel[1];
				data[2] = pixel[0];
				data[3] = pixel[3];
			} else {
				static_assert_failed("unknown format");
			}
		};
		template <TexFmt fmt>
		inline constexpr auto toWithPos = [](Pixel32 const & pixel, Byte * const & data, Sz const & pos) {
			if constexpr (fmt == TexFmt::kRGBAd16x4444eL ||
						  fmt == TexFmt::kRGBd16x565eL ||
						  fmt == TexFmt::kRGBAd16x5551eL ||
						  fmt == TexFmt::kARGBd16x4444eL) {
				to<fmt>(pixel, data + pos * 2);
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				to<fmt>(pixel, data + pos * 3);
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB ||
								 fmt == TexFmt::kARGBd32x8888eL) {
				to<fmt>(pixel, data + pos * 4);
			} else {
				static_assert_failed("unknown format");
			}
			return;
		};

		inline constexpr auto textureConverter = [](auto & img, auto const & data, auto const & converter) {
			auto i = kNo1;
			for (auto y = kNo1; y < img.$sz.$h; ++y) {
				for (auto x = kNo1; x < img.$sz.$w; ++x) {
					converter(img.$data[y][x], data.$data, i);
					++i;
				}
			}
			return;
		};

		inline constexpr auto textureConverterForSpecialOfPvZ1RSB = [](auto & img, auto & data, auto const & converter) {
			/*
			constexpr Sz unitSz = 32;
			constexpr Sz blockSumOnX = 32;
			Sz blockHeight = img.$sz.$w / unitSz;
			Sz blockSz = unitSz * blockHeight;
			Sz blockRowSz = blockSz * blockSumOnX;
			Sz blockSumOnY = img.$sz.$h / unitSz;
			auto y = kNo1;
			for (auto blockRow = kNo1; blockRow < blockSumOnY; ++blockRow) {
				for (auto blockCol = kNo1; blockCol < blockSumOnX; ++blockCol) {
					auto xBase = kNo1;
					for (auto unitNum = kNo1; unitNum < blockHeight; ++unitNum) {
						for (auto i = kNo1; i < unitSz; ++i) {
							Sz posOnSrc = (blockRow * blockHeight + unitNum) * (unitSz * blockSumOnX) + (blockCol * unitSz + i);
							converter(img.$data[y][xBase * unitSz + i], data.$data, posOnSrc);
						}
						++xBase;
					}
					++y;
				}
			}
			 */
			return;
		};

		namespace $PvZ2CHSAndroidVarAlpha {

			class Mapper {

				Sz $bpp;
				ByteSection $data;

			public:

				auto & read(ByteSection const & header) {
					Sz sz = header.first();
					$bpp = (sz == kSzN) ? kSz1 : sz;
					$data.cpy(ByteSection().ref(header, kNo2, sz));
					return thiz;
				}
				auto & write(ByteSection & header) const {
					//header << $data.$sz << $data;
					return thiz;
				}

				auto map(IU8 const & t) const {
					return $data[t];
				}

			};

		}

		inline constexpr auto fromForVarSizeAlpha = [](Color8 & alpha, Byte const & data, Sz const & bpp, Sz const & th, ByteSection const & mapper) {
			Sz posFromHigh = bpp * th;
			Sz posToLow = 8 - posFromHigh - bpp;
			IU8 validBit = IU8(IU8(IU8("11111111"_bin8 << posFromHigh) >> posFromHigh) >> posToLow) << posToLow;
			F64 rate = static_cast<F64>("100000000"_bin16) / static_cast<F64>(("1"_bin8 << bpp) - 0);
			IU8 val = (data & validBit) >> posToLow;
			//log.cmt("d=%u", data);
			//log.txt("high: %u, low: %u, vbit: %u ", posFromHigh, posToLow, validBit);
			//log.txt("val: %u, ", val);
			if (!mapper.isEmpty() && !mapper.isNull()) {
				val = mapper[val];
			}
			//log.txt("val-ed: %u\n", val);
			alpha = static_cast<F64>(val) * rate;
			return;
		};
		inline constexpr auto toForVarSizeAlpha = [](Color8 const & alpha, Byte & data, Sz const & bpp, Sz const & th, ByteSection const & mapper) {
			Sz posFromHigh = bpp * th;
			Sz posToLow = 8 - posFromHigh + bpp;
			IU8 validBit = ((("11111111"_bin8 << posFromHigh) >> posFromHigh) >> posToLow) << posToLow;
			F64 rate = static_cast<F64>("11111111"_bin8) / static_cast<F64>(("1"_bin8 << bpp) - 1);
			IU8 val = alpha / rate;
			if (!mapper.isEmpty() && !mapper.isNull()) {
				val = mapper[val];
			}
			data |= val << posToLow;
			return;
		};

		/*
		inline constexpr auto fromForAdVmSpecialOfPvZ2CHSAndroid = [](Byte const & data, Bool const & atFirst, Color8 & alpha, ByteSection const & mapper) {
			IU8 alphaMapped;
			if (atFirst) {
				alphaMapped = (data & "11110000"_bin8) >> 4;
			} else {
				alphaMapped = (data & "00001111"_bin8) >> 0;
			}
			alpha = cnvInt8Mapper(alphaMapped, mapper) * 0x10;
			return;
		};
		inline constexpr auto toForAdVmSpecialOfPvZ2CHSAndroid = [](Byte & data, Bool const & atFirst, Color8 const & alpha, ByteSection const & revMapper) {
			IU8 alphaMapped = cnvInt8Mapper(alpha, revMapper);
			if (atFirst) {
				data |= alphaMapped << 4;
			} else {
				data |= alphaMapped << 0;
			}
			return;
		};
*/

		inline constexpr auto textureConverterForVarSizeAlpha = [](auto & img, auto & data, auto const & converter, Sz const & bpp, ByteSection const & mapper) {
			Sz const ppb = 8 / bpp;
			log.cmt("bpp: %u, ppb: %u", bpp, ppb);
			for (auto y = kNo1; y < img.$sz.$h; ++y) {
				for (auto x = kNo1; x < img.$sz.$w; ++x) {
					Sz bytePos = (y * img.$sz.$w + x) / ppb;
					//log.cmt("p=%u[%u]", bytePos, x % ppb);
					converter(img.$data[y][x][3], data[bytePos], bpp, x % ppb, mapper);
				}
			}
			return;
		};
		inline constexpr auto textureConverterForAdVmSpecialOfPvZ2CHSAndroid = [](auto & img, auto & data, auto const & converter) {
			ByteSection mapper, mappedAlpha;
			mapper.ref(data, kNo2, data.first());
			mappedAlpha.ref(data, kNo2 + mapper.$sz);
			return textureConverterForVarSizeAlpha(img, mappedAlpha, converter, mapper.isEmpty() ? kSz1 : 4, mapper);
		};

	}

	struct Bitmap {

		Pixel32 ** $data;
		ImgSz $sz;

		auto isNull() const {
			return $data == kNullPtr;
		}

		auto & operator[](Sz const & y) {
			return $data[y];
		}
		auto const & operator[](Sz const & y) const {
			return $data[y];
		}

		auto & alloc(ImgSz const & sz) {
			pAlloc($data, sz.$h, sz.$w);
			$sz = sz;
			return thiz;
		}
		auto & free() {
			if (!isNull()) {
				pFree($data, $sz.$h);
				$sz.setNull();
			}
			return thiz;
		}

		auto & clear() {
			for (auto y = kNo1; y < $sz.$h; ++y)
				std::memset($data[y], szOf<Pixel32>($sz.$w), 0);
			return thiz;
		}

		auto & cpy(Bitmap const & src, ImgPos const & srcPos, ImgSz const & sz, ImgPos const & dstPos) {
			for (auto y = kNo1; y < sz.$h; ++y)
				std::memcpy($data[y + dstPos.$y] + dstPos.$x, src[y + srcPos.$y] + srcPos.$x, szOf<Pixel32>(sz.$w));
			return thiz;
		}

		auto & unbind() {
			$data = kNullPtr;
			$sz.setNull();
			return thiz;
		}
		auto & bind(Bitmap const & t, ImgArea const & area = ImgArea()) {
			ImgPos pos;
			ImgSz sz;
			if (area.$pos.isNull()) {
				pos.set(kNo1, kNo1);
			} else {
				pos = area.$pos;
			}
			if (area.$sz.isNull()) {
				sz.set(t.$sz.$w - pos.$x, t.$sz.$h - pos.$y);
			} else {
				sz = area.$sz;
			}
			unbind();
			pAlloc($data, sz.$h);
			$sz = sz;
			for (auto i = kNo1; i < sz.$h; ++i) {
				$data[i] = const_cast<Pixel32 *>(t[pos.$y + i] + pos.$x);
			}
			return thiz;
		}

		Sz calcSz(Sz const & bpp = Sz(32)) const {
			return $sz.calcSz() * bpp / 8;
		}

		auto getColorByteRef() {
			return reinterpret_cast<Byte **>($data);
		}
		auto getColorByteRef() const {
			return reinterpret_cast<Byte const * const * const>($data);
		}

		auto getColorByte() const {
			Byte ** dst(kNullPtr);
			pAlloc(dst, szOf<Pixel32>($sz.$h), szOf<Pixel32>($sz.$w));
			for (auto y = kNo1; y < $sz.$h; ++y)
				std::memcpy(dst[y], $data[y], szOf<Pixel32>($sz.$w));
			return dst;
		}

		auto & reverseY() {
			for (auto y = kNo1; y < $sz.$h / 2; ++y)
				vSwp($data[y], $data[$sz.$h - kNo2 - y]);
			return thiz;
		}

		auto & setDataAlpha() {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = "11111111"_bin8;
				}
			}
			return thiz;
		}
		auto & setDataAlpha(ByteSection const & data) {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = data[(y * $sz.$w) + x];
				}
			}
			return thiz;
		}
		auto & setDataAlphaSpecial(ByteSection const & data) {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = data[(y * $sz.$w) + x];
				}
			}
			return thiz;
		}
		auto & getDataAplha(ByteSection & data) const {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x] = $data[y][x][3];
				}
			}
			return thiz;
		}

		template <TexFmt fmt>
		auto & setData(ByteSection const & data) {
			if constexpr (fmt == TexFmt::kRGBAd16x4444eL ||
						  fmt == TexFmt::kRGBd16x565eL ||
						  fmt == TexFmt::kRGBAd16x5551eL ||
						  fmt == TexFmt::kRGBd24x888eB ||
						  fmt == TexFmt::kRGBAd32x8888eB ||
						  fmt == TexFmt::kARGBd16x4444eL ||
						  fmt == TexFmt::kARGBd32x8888eL
						  ) {
				$PixelCast::textureConverter(thiz, data, $PixelCast::fromWithPos<fmt>);
			} else if constexpr (fmt == TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::fromWithPos<TexFmt::kRGBAd16x4444eL>);
			} else if constexpr (fmt == TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::fromWithPos<TexFmt::kRGBd16x565eL>);
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::fromWithPos<TexFmt::kRGBAd16x5551eL>);
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::fromWithPos<TexFmt::kARGBd32x8888eL>);
			} else if constexpr (fmt == TexFmt::kAdVmSpecialOfPvZ2CHSAndroid) {
				$PixelCast::textureConverterForAdVmSpecialOfPvZ2CHSAndroid(thiz, data, $PixelCast::fromForVarSizeAlpha);
			} else {
				static_assert_failed("unknown format");
			}
			return thiz;
		}

		template <TexFmt fmt>
		auto & getData(ByteSection & data) const {
			if constexpr (fmt == TexFmt::kRGBAd16x4444eL ||
						  fmt == TexFmt::kRGBd16x565eL ||
						  fmt == TexFmt::kRGBAd16x5551eL ||
						  fmt == TexFmt::kRGBd24x888eB ||
						  fmt == TexFmt::kRGBAd32x8888eB ||
						  fmt == TexFmt::kARGBd16x4444eL ||
						  fmt == TexFmt::kARGBd32x8888eL
						  ) {
				$PixelCast::textureConverter(thiz, data, $PixelCast::toWithPos<fmt>);
			} else if constexpr (fmt == TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::toWithPos<TexFmt::kRGBAd16x4444eL>);
			} else if constexpr (fmt == TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::toWithPos<TexFmt::kRGBd16x565eL>);
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::toWithPos<TexFmt::kRGBAd16x5551eL>);
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB) {
				$PixelCast::textureConverterForSpecialOfPvZ1RSB(thiz, data, $PixelCast::toWithPos<TexFmt::kARGBd32x8888eL>);
			} else if constexpr (fmt == TexFmt::kAdVmSpecialOfPvZ2CHSAndroid) {
				//$PixelCast::textureConverterForAdVmSpecialOfPvZ2CHSAndroid(thiz, data);
			} else {
				static_assert_failed("unknown format");
			}
			return thiz;
		}

		~Bitmap() {
			free();
		}
		implicit Bitmap() :
			$data(kNullPtr), $sz() {
		}
		explicit Bitmap(ImgSz const & sz) :
			Bitmap() {
			alloc(sz);
		}

	};

}
