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

	inline constexpr Sz kTexFmtSum(22);

	enum class TexFmt : Sz {

		kNull = kNo1,

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

	inline Str const kTexFmtStr[]{
		Str(),
		Str("ARGBd16x4444eL"_r),
		Str("ARGBd32x8888eL"_r),
		Str("ARGBd32x8888eLsAd8"_r),
		Str("RGBd16x565eL"_r),
		Str("RGBAd16x5551eL"_r),
		Str("RGBAd16x4444eL"_r),
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

	struct Bitmap {

		Pixel32 ** $data;
		ImgSz $sz;

		auto isNull() const {
			return $data == kNullPtr;
		}

		Pixel32 *& operator[](Sz const & y) {
			return $data[y];
		}
		Pixel32 const * const & operator[](Sz const & y) const {
			return $data[y];
		}

		auto & alloc(ImgSz const & sz) {
			pAlloc($data, sz.$h, sz.$w);
			$sz = sz;
			return *this;
		}
		auto & free() {
			if (!isNull()) {
				pFree($data, $sz.$h);
				$sz.setNull();
			}
			return *this;
		}

		auto & clear() {
			for (auto y = kNo1; y < $sz.$h; ++y)
				std::memset($data[y], szOf<Pixel32>($sz.$w), 0);
			return *this;
		}

		auto & cpy(Bitmap const & src, ImgPos const & srcPos, ImgSz const & sz, ImgPos const & dstPos) {
			for (auto y = kNo1; y < sz.$h; ++y)
				std::memcpy($data[y + dstPos.$y] + dstPos.$x, src[y + srcPos.$y] + srcPos.$x, szOf<Pixel32>(sz.$w));
			return *this;
		}

		Sz calcSz(Sz const & bpp = Sz(32)) const {
			return $sz.calcSz() * bpp / 8;
		}

		Byte **& getColorByteRef() {
			return reinterpret_cast<Byte **&>($data);
		}
		Byte const * const * const & getColorByteRef() const {
			return reinterpret_cast<Byte const * const * const &>($data);
		}
		Byte const * const * getColorByteRefX(ImgPos const & pos) const {
			Byte const ** dst(kNullPtr);
			pAlloc(dst, $sz.$h - pos.$y);
			for (auto y = pos.$y; y < $sz.$h; ++y)
				dst[y - pos.$y] = reinterpret_cast<Byte const *>($data[y] + pos.$x);
			return dst;
		}

		Byte ** getColorByte() const {
			Byte ** dst(kNullPtr);
			pAlloc(dst, szOf<Pixel32>($sz.$h), szOf<Pixel32>($sz.$w));
			for (auto y = kNo1; y < $sz.$h; ++y)
				std::memcpy(dst[y], $data[y], szOf<Pixel32>($sz.$w));
			return dst;
		}
		Byte ** getColorByte(ImgPos const & pos) const {
			Byte ** dst(kNullPtr);
			pAlloc(dst, szOf<Pixel32>($sz.$h - pos.$y), szOf<Pixel32>($sz.$w - pos.$x));
			for (auto y = kNo1; y < $sz.$h; ++y)
				std::memcpy(dst[y - pos.$y], $data[y], szOf<Pixel32>($sz.$w - pos.$x));
			return dst;
		}

		auto & reverseY() {
			for (auto y = kNo1; y < $sz.$h / 2; ++y)
				vSwp($data[y], $data[$sz.$h - kNo2 - y]);
			return *this;
		}

		auto & Set_A8() {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = "11111111"_bin8;
				}
			}
			return *this;
		}
		auto & Set_A8(ByteSection const & data) {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = data[(y * $sz.$w) + x];
				}
			}
			return *this;
		}

		auto & unbind() {
			$data = kNullPtr;
			$sz.setNull();
			return *this;
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
				sz.set($sz.$w - pos.$x, $sz.$h - pos.$y);
			} else {
				sz = area.$sz;
			}
			unbind();
			pAlloc($data, sz.$h);
			$sz = sz;
			for (auto i = kNo1; i < sz.$h; ++i) {
				$data[i] = const_cast<Pixel32 *>(t[pos.$y + i] + pos.$x);
			}
			return *this;
		}

		template <TexFmt fmt>
		auto & setDataProto(ByteSection const & dataOri) {
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
			} else if constexpr (fmt == TexFmt::kRGBd16x565eL) {
				auto data = reinterpret_cast<IU16 const *>(dataOri.$data);
				for (auto y = kNo1; y < $sz.$h; ++y) {
					for (auto x = kNo1; x < $sz.$w; ++x) {
						$data[y][x][0] = ((*data & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
						$data[y][x][1] = ((*data & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
						$data[y][x][2] = ((*data & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
						$data[y][x][3] = ((*data & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
					}
				}
			} else if constexpr (fmt == TexFmt::kRGBd16x565eL) {

			} else if constexpr (fmt == TexFmt::kRGBd16x565eL) {

			} else {
				static_assert_failed("unknown format");
			}
			return *this;
		}
		template <TexFmt fmt>
		auto & setData(ByteSection const & dataOri, ImgArea const & area = ImgArea()) {
			ImgPos pos;
			ImgSz sz;
			if (area.$pos.isNull()) {
				pos.set(kNo1, kNo1);
			} else {
				pos = area.$pos;
			}
			if (area.$sz.isNull()) {
				sz.set($sz.$w - pos.$x, $sz.$h - pos.$y);
			} else {
				sz = area.$sz;
			}
			return *this;
		}
		template <TexFmt fmt>
		auto & getData(ByteSection & dataOri, ImgArea const & area = ImgArea()) {

			return *this;
		}

		auto & Set_RGBd16x565eL(ByteSection const & dataOri) {
			auto const & data = reinterpret_cast<IU16 const *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][0] = ((data[(y * $sz.$w) + x] & "11111-000000-00000"_bin16) >> 11) * $ColorRate::k5A8;
					$data[y][x][1] = ((data[(y * $sz.$w) + x] & "00000-111111-00000"_bin16) >> 5) * $ColorRate::k6A8;
					$data[y][x][2] = ((data[(y * $sz.$w) + x] & "00000-000000-11111"_bin16) >> 0) * $ColorRate::k5A8;
				}
			}
			return *this;
		}
		auto & Set_RGBAd16x5551eL(ByteSection const & dataOri) {
			auto const & data = reinterpret_cast<IU16 const *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][0] = ((data[(y * $sz.$w) + x] & "11111-00000-00000-0"_bin16) >> 11) * $ColorRate::k5A8;
					$data[y][x][1] = ((data[(y * $sz.$w) + x] & "00000-11111-00000-0"_bin16) >> 6) * $ColorRate::k5A8;
					$data[y][x][2] = ((data[(y * $sz.$w) + x] & "00000-00000-11111-0"_bin16) >> 1) * $ColorRate::k5A8;
					$data[y][x][3] = ((data[(y * $sz.$w) + x] & "00000-00000-00000-1"_bin16) >> 0) * $ColorRate::k1A8;
				}
			}
			return *this;
		}
		auto & Set_RGBAd16x4444eL(ByteSection const & dataOri) {
			auto const & data = reinterpret_cast<IU16 const *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][0] = ((data[(y * $sz.$w) + x] & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
					$data[y][x][1] = ((data[(y * $sz.$w) + x] & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
					$data[y][x][2] = ((data[(y * $sz.$w) + x] & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
					$data[y][x][3] = ((data[(y * $sz.$w) + x] & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
				}
			}
			return *this;
		}
		auto & Set_RGBd24x888eB(ByteSection const & dataOri) {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][0] = dataOri[((y * $sz.$w) + x) * 3 + 0];
					$data[y][x][1] = dataOri[((y * $sz.$w) + x) * 3 + 1];
					$data[y][x][2] = dataOri[((y * $sz.$w) + x) * 3 + 2];
				}
			}
			return *this;
		}
		auto & Set_RGBAd32x8888eB(ByteSection const & data) {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][0] = data[((y * $sz.$w) + x) * 4 + 0];
					$data[y][x][1] = data[((y * $sz.$w) + x) * 4 + 1];
					$data[y][x][2] = data[((y * $sz.$w) + x) * 4 + 2];
					$data[y][x][3] = data[((y * $sz.$w) + x) * 4 + 3];
				}
			}
			return *this;
		}

		auto & Set_ARGBd16x4444eL(ByteSection const & dataOri) {
			auto const & data = reinterpret_cast<IU16 const *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = ((data[(y * $sz.$w) + x] & "1111-0000-0000-0000"_bin16) >> 12) * $ColorRate::k4A8;
					$data[y][x][0] = ((data[(y * $sz.$w) + x] & "0000-1111-0000-0000"_bin16) >> 8) * $ColorRate::k4A8;
					$data[y][x][1] = ((data[(y * $sz.$w) + x] & "0000-0000-1111-0000"_bin16) >> 4) * $ColorRate::k4A8;
					$data[y][x][2] = ((data[(y * $sz.$w) + x] & "0000-0000-0000-1111"_bin16) >> 0) * $ColorRate::k4A8;
				}
			}
			return *this;
		}
		auto & Set_ARGBd32x8888eL(ByteSection const & dataOri) {
			auto const & data = reinterpret_cast<IU32 const *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					$data[y][x][3] = (data[(y * $sz.$w) + x] & "11111111-00000000-00000000-00000000"_bin32) >> 24;
					$data[y][x][0] = (data[(y * $sz.$w) + x] & "00000000-11111111-00000000-00000000"_bin32) >> 16;
					$data[y][x][1] = (data[(y * $sz.$w) + x] & "00000000-00000000-11111111-00000000"_bin32) >> 8;
					$data[y][x][2] = (data[(y * $sz.$w) + x] & "00000000-00000000-00000000-11111111"_bin32) >> 0;
				}
			}
			return *this;
		}

		auto & Get_A8(ByteSection & data) const {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x] = $data[y][x][3];
				}
			}
			return *this;
		}

		auto & Get_RGBd16x565eL(ByteSection & dataOri) const {
			auto const & data = reinterpret_cast<IU16 *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x]
						= (static_cast<IU16>($data[y][x][0] / $ColorRate::k5A8) << 11)
						| (static_cast<IU16>($data[y][x][1] / $ColorRate::k6A8) << 5)
						| (static_cast<IU16>($data[y][x][2] / $ColorRate::k5A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_RGBAd16x5551eL(ByteSection & dataOri) const {
			auto const & data = reinterpret_cast<IU16 *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x]
						= (static_cast<IU16>($data[y][x][0] / $ColorRate::k5A8) << 11)
						| (static_cast<IU16>($data[y][x][1] / $ColorRate::k5A8) << 6)
						| (static_cast<IU16>($data[y][x][2] / $ColorRate::k5A8) << 1)
						| (static_cast<IU16>($data[y][x][3] / $ColorRate::k1A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_RGBAd16x4444eL(ByteSection & dataOri) const {
			auto const & data = reinterpret_cast<IU16 *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x]
						= (static_cast<IU16>($data[y][x][0] / $ColorRate::k4A8) << 12)
						| (static_cast<IU16>($data[y][x][1] / $ColorRate::k4A8) << 8)
						| (static_cast<IU16>($data[y][x][2] / $ColorRate::k4A8) << 4)
						| (static_cast<IU16>($data[y][x][3] / $ColorRate::k4A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_RGBd24x888eB(ByteSection & data) const {
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[((y * $sz.$w) + x) * 3 + 0] = $data[y][x][0];
					data[((y * $sz.$w) + x) * 3 + 1] = $data[y][x][1];
					data[((y * $sz.$w) + x) * 3 + 2] = $data[y][x][2];
				}
			}
			return *this;
		}
		auto & Get_RGBAd32x8888eB(ByteSection & dataOri) const {
			auto const & data = reinterpret_cast<IU32 *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x]
						= vRevEndian(static_cast<IU32>($data[y][x][0] << 24)
									 | static_cast<IU32>($data[y][x][1] << 16)
									 | static_cast<IU32>($data[y][x][2] << 8)
									 | static_cast<IU32>($data[y][x][3] << 0));
				}
			}
			return *this;
		}

		auto & Get_ARGBd16x4444eL(ByteSection & dataOri) const {
			auto const & data = reinterpret_cast<IU16 *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x]
						= (static_cast<IU16>($data[y][x][3] / $ColorRate::k4A8) << 12)
						| (static_cast<IU16>($data[y][x][0] / $ColorRate::k4A8) << 8)
						| (static_cast<IU16>($data[y][x][1] / $ColorRate::k4A8) << 4)
						| (static_cast<IU16>($data[y][x][2] / $ColorRate::k4A8) << 0);
				}
			}
			return *this;
		}
		auto & Get_ARGBd32x8888eL(ByteSection & dataOri) const {
			auto const & data = reinterpret_cast<IU32 *>(dataOri.$data);
			for (auto y = kNo1; y < $sz.$h; ++y) {
				for (auto x = kNo1; x < $sz.$w; ++x) {
					data[(y * $sz.$w) + x]
						= static_cast<IU32>($data[y][x][3] << 24)
						| static_cast<IU32>($data[y][x][0] << 16)
						| static_cast<IU32>($data[y][x][1] << 8)
						| static_cast<IU32>($data[y][x][2] << 0);
				}
			}
			return *this;
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
