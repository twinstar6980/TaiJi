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

	inline constexpr Sz kTexFmtSum(26);

	enum class TexFmt : Sz {

		kNull = kNo1,

		kARGBd16x4444eL,
		kARGBd32x8888eL, kARGBd32x8888eLsAd8,

		kRGBAd16x4444eL, kRGBd16x565eL, kRGBAd16x5551eL,
		kRGBd24x888eB,
		kRGBAd32x8888eB,

		kRGBcETC1, kRGBcETC1sAd8, kRGBcETC1sAcU,
		kRGBcETC2, kRGBAcETC2,

		kRGBAcPVRTC2BPP, kRGBAcPVRTC4BPP, kRGBAcPVRTC4BPPsAd8,

		kRGBcDXT1, kRGBAcDXT3, kRGBAcDXT5,

		kRGBcATITC, kRGBAcATITC,

		// 0x20 series
		kARGBd32x8888eL_SpecialByPopcap,
		kRGBAd16x4444eL_SpecialByPopcap, kRGBd16x565eL_SpecialByPopcap, kRGBAd16x5551eL_SpecialByPopcap,

	};

	inline Str const kTexFmtStr[]{
		Str(),
		Str("ARGBd16x4444eL"_r),
		Str("ARGBd32x8888eL"_r),
		Str("ARGBd32x8888eLsAd8"_r),
		Str("RGBAd16x4444eL"_r),
		Str("RGBd16x565eL"_r),
		Str("RGBAd16x5551eL"_r),
		Str("RGBd24x888eB"_r),
		Str("RGBAd32x8888eB"_r),
		Str("RGBcETC1"_r),
		Str("RGBcETC1sAd8"_r),
		Str("RGBcETC1sAcU"_r),
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
		Str("ARGBd32x8888eL_SpecialByPopcap"_r),
		Str("RGBAd16x4444eL_SpecialByPopcap"_r),
		Str("RGBd16x565eL_SpecialByPopcap"_r),
		Str("RGBAd16x5551eL_SpecialByPopcap"_r)
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

	inline constexpr auto textureConverter = [](ByteSection const & src, Bitmap & dst, auto const & converter) {
		auto i = kNo1;
		for (auto y = kNo1; y < dst.$sz.$h; ++y) {
			for (auto x = kNo1; x < dst.$sz.$w; ++x) {
				converter(dst[y][x], src, i);
				++i;
			}
		}
	};

	inline constexpr auto textureConverterForSpecialByPopcap = [](ByteSection const & src, Bitmap & dst, auto const & converter) {
		constexpr Sz unitSz = 32;
		constexpr Sz blockSumOnX = 32;
		Sz blockHeight = dst.$sz.$w / unitSz;
		Sz blockSz = unitSz * blockHeight;
		Sz blockRowSz = blockSz * blockSumOnX;
		Sz blockSumOnY = dst.$sz.$h / unitSz;
		auto y = kNo1;
		for (auto blockRow = kNo1; blockRow < blockSumOnY; ++blockRow) {
			for (auto blockCol = kNo1; blockCol < blockSumOnX; ++blockCol) {
				auto xBase = kNo1;
				for (auto unitNum = kNo1; unitNum < blockHeight; ++unitNum) {
					for (auto i = kNo1; i < unitSz; ++i) {
						Sz posOnSrc = (blockRow * blockHeight + unitNum) * (unitSz * blockSumOnX) + (blockCol * unitSz + i);
						converter(src, posOnSrc, dst, y, xBase * unitSz + i);
					}
					++xBase;
				}
				++y;
			}
		}
		return;
	};

	namespace $PixelCast {

		template <TexFmt fmt>
		inline auto from(Pixel32 & dst, Byte const * const & src) {
			if constexpr (fmt == TexFmt::kRGBd16x565eL) {
				auto srcV16 = *reinterpret_cast<IU16 const * const>(src);
				dst[0] = ((srcV16 & "11111-000000-00000"_bin16) >> 11) * $ColorRate::k5A8;
				dst[1] = ((srcV16 & "00000-111111-00000"_bin16) >> 5) * $ColorRate::k6A8;
				dst[2] = ((srcV16 & "00000-000000-11111"_bin16) >> 0) * $ColorRate::k5A8;
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eL) {
				auto srcV16 = *reinterpret_cast<IU16 const * const>(src);
				dst[0] = ((srcV16 & "11111-00000-00000-0"_bin16) >> 11) * $ColorRate::k5A8;
				dst[1] = ((srcV16 & "00000-11111-00000-0"_bin16) >> 6) * $ColorRate::k5A8;
				dst[2] = ((srcV16 & "00000-00000-11111-0"_bin16) >> 1) * $ColorRate::k5A8;
				dst[3] = ((srcV16 & "00000-00000-00000-1"_bin16) >> 0) * $ColorRate::k1A8;
			} else if constexpr (fmt == TexFmt::kRGBAd16x4444eL) {
				auto srcV16 = *reinterpret_cast<IU16 const * const>(src);
				dst[0] = ((srcV16 & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
				dst[1] = ((srcV16 & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
				dst[2] = ((srcV16 & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
				dst[3] = ((srcV16 & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB) {
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = src[3];
			} else if constexpr (fmt == TexFmt::kARGBd16x4444eL) {
				auto srcV16 = *reinterpret_cast<IU16 const * const>(src);
				dst[3] = ((srcV16 & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
				dst[0] = ((srcV16 & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
				dst[1] = ((srcV16 & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
				dst[2] = ((srcV16 & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eL) {
				dst[3] = src[3];
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
			} else {
				static_assert_failed("unknown format");
			}
			return;
		}
		template <TexFmt fmt>
		inline auto from(Pixel32 & dst, Byte const * const & src, Sz const & pos) {
			if constexpr (fmt == TexFmt::kRGBd16x565eL ||
						  fmt == TexFmt::kRGBAd16x5551eL ||
						  fmt == TexFmt::kRGBAd16x4444eL ||
						  fmt == TexFmt::kARGBd16x4444eL) {
				from<fmt>(dst, src + pos * 2);
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				from<fmt>(dst, src + pos * 3);
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB ||
								 fmt == TexFmt::kARGBd32x8888eL) {
				from<fmt>(dst, src + pos * 4);
			} else {
				static_assert_failed("unknown format");
			}
			return;
		}

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
		auto & getDataAplha(ByteSection & data) const {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x] = $data[y][x][3];
				}
			}
			return thiz;
		}

		template <TexFmt fmt>
		auto & setData(ByteSection const & dataOri) {
			if constexpr (fmt == TexFmt::kRGBd16x565eL) {
				auto data = reinterpret_cast<IU16 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][0] = ((*data & "11111-000000-00000"_bin16) >> 11) * $ColorRate::k5A8;
						$data[y][x][1] = ((*data & "00000-111111-00000"_bin16) >> 5) * $ColorRate::k6A8;
						$data[y][x][2] = ((*data & "00000-000000-11111"_bin16) >> 0) * $ColorRate::k5A8;
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eL) {
				textureConverter(dataOri, thiz, $PixelCast::from<TexFmt::kRGBAd16x5551eL>);
				auto data = reinterpret_cast<IU16 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][0] = ((*data & "11111-00000-00000-0"_bin16) >> 11) * $ColorRate::k5A8;
						$data[y][x][1] = ((*data & "00000-11111-00000-0"_bin16) >> 6) * $ColorRate::k5A8;
						$data[y][x][2] = ((*data & "00000-00000-11111-0"_bin16) >> 1) * $ColorRate::k5A8;
						$data[y][x][3] = ((*data & "00000-00000-00000-1"_bin16) >> 0) * $ColorRate::k1A8;
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBAd16x4444eL_SpecialByPopcap) {
				textureConverterForSpecialByPopcap(
					dataOri, thiz,
					[](ByteSection const & src, Sz const & srcPos, Bitmap const & dst, Sz const & y, Sz const & x) {
						auto & srcV = reinterpret_cast<IU16 const *>(src.$data)[srcPos];
						auto & dstV = dst.$data[y][x];
						dstV[0] = ((srcV & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
						dstV[1] = ((srcV & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
						dstV[2] = ((srcV & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
						dstV[3] = ((srcV & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
					});
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eL_SpecialByPopcap) {
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eL_SpecialByPopcap) {
			} else if constexpr (fmt == TexFmt::kRGBd16x565eL_SpecialByPopcap) {
			} else if constexpr (fmt == TexFmt::kRGBAd16x4444eL) {
				auto data = reinterpret_cast<IU16 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][0] = ((*data & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
						$data[y][x][1] = ((*data & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
						$data[y][x][2] = ((*data & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
						$data[y][x][3] = ((*data & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				auto data = reinterpret_cast<IU8 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][0] = data[0];
						$data[y][x][1] = data[1];
						$data[y][x][2] = data[2];
						data += 3;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB) {
				auto data = reinterpret_cast<IU8 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][0] = data[0];
						$data[y][x][1] = data[1];
						$data[y][x][2] = data[2];
						$data[y][x][3] = data[3];
						data += 4;
					}
				}
			} else if constexpr (fmt == TexFmt::kARGBd16x4444eL) {
				auto data = reinterpret_cast<IU16 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][3] = ((*data & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
						$data[y][x][0] = ((*data & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
						$data[y][x][1] = ((*data & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
						$data[y][x][2] = ((*data & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eL) {
				auto data = reinterpret_cast<IU8 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][3] = data[3];
						$data[y][x][0] = data[2];
						$data[y][x][1] = data[1];
						$data[y][x][2] = data[0];
						data += 4;
					}
				}
			} else {
				static_assert_failed("unknown format");
			}
			return thiz;
		}

		template <TexFmt fmt>
		auto & getData(ByteSection & dataOri) const {
			if constexpr (fmt == TexFmt::kRGBd16x565eL) {
				auto data = reinterpret_cast<IU16 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						*data
							= (static_cast<IU16>($data[y][x][0] / $ColorRate::k5A8) << 11)
							| (static_cast<IU16>($data[y][x][1] / $ColorRate::k6A8) << 5)
							| (static_cast<IU16>($data[y][x][2] / $ColorRate::k5A8) << 0);
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBAd16x5551eL) {
				auto data = reinterpret_cast<IU16 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						*data
							= (static_cast<IU16>($data[y][x][0] / $ColorRate::k5A8) << 11)
							| (static_cast<IU16>($data[y][x][1] / $ColorRate::k5A8) << 6)
							| (static_cast<IU16>($data[y][x][2] / $ColorRate::k5A8) << 1)
							| (static_cast<IU16>($data[y][x][3] / $ColorRate::k1A8) << 0);
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBAd16x4444eL) {
				auto data = reinterpret_cast<IU16 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						*data
							= (static_cast<IU16>($data[y][x][0] / $ColorRate::k4A8) << 12)
							| (static_cast<IU16>($data[y][x][1] / $ColorRate::k4A8) << 8)
							| (static_cast<IU16>($data[y][x][2] / $ColorRate::k4A8) << 4)
							| (static_cast<IU16>($data[y][x][3] / $ColorRate::k4A8) << 0);
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBd24x888eB) {
				auto data = reinterpret_cast<IU8 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						data[0] = $data[y][x][0];
						data[1] = $data[y][x][1];
						data[2] = $data[y][x][2];
						data += 3;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBAd32x8888eB) {
				auto data = reinterpret_cast<IU8 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						data[0] = $data[y][x][0];
						data[1] = $data[y][x][1];
						data[2] = $data[y][x][2];
						data[3] = $data[y][x][3];
						data += 4;
					}
				}
			} else if constexpr (fmt == TexFmt::kARGBd16x4444eL) {
				auto data = reinterpret_cast<IU16 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						*data
							= (static_cast<IU16>($data[y][x][3] / $ColorRate::k4A8) << 12)
							| (static_cast<IU16>($data[y][x][0] / $ColorRate::k4A8) << 8)
							| (static_cast<IU16>($data[y][x][1] / $ColorRate::k4A8) << 4)
							| (static_cast<IU16>($data[y][x][2] / $ColorRate::k4A8) << 0);
						++data;
					}
				}
			} else if constexpr (fmt == TexFmt::kARGBd32x8888eL) {
				auto data = reinterpret_cast<IU8 *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						data[0] = $data[y][x][2];
						data[1] = $data[y][x][1];
						data[2] = $data[y][x][0];
						data[3] = $data[y][x][3];
						data += 4;
					}
				}
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
