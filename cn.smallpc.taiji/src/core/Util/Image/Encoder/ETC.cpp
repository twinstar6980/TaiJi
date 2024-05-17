#include "./ETC.hpp"

#include "../../../../dependency/ETCPACK/_linker.hpp"
#include "../../../../dependency/rg_etc1/rg_etc1.h"

namespace $TJ::$Util::$Image::$Encoder::$ETC {

	namespace $ETC1 {

		namespace {

			static auto enc(Byte const * const & src, Byte * const & dst, ImgSz const & sz) {
				::rg_etc1::etc1_pack_params etc1PackParm;
				IU32 tmpBlock[16] = { 0 };
				auto cnt = kSzN;
				for (auto y = kNo1; y < sz.$h / 4; ++y) {
					log.txt("\r>>> img-enc : %.1f%%", (static_cast<F64>(y) * 4.0 / static_cast<F64>(sz.$h)) * 100.0);
					for (auto x = kNo1; x < sz.$w / 4; ++x) {
						auto const & srcBlockBase = src + 4 * (y * 4 * sz.$w + x * 4);
						std::memcpy(tmpBlock + 0, srcBlockBase + sz.$w * 4 * 0, 4 * 4);
						std::memcpy(tmpBlock + 4, srcBlockBase + sz.$w * 4 * 1, 4 * 4);
						std::memcpy(tmpBlock + 8, srcBlockBase + sz.$w * 4 * 2, 4 * 4);
						std::memcpy(tmpBlock + 12, srcBlockBase + sz.$w * 4 * 3, 4 * 4);
						::rg_etc1::pack_etc1_block_init();
						::rg_etc1::pack_etc1_block(dst + 8 * cnt, tmpBlock, etc1PackParm);
						++cnt;
					}
				}
				log.txtln("\r>>> img-enc : 100.0%%");
				return;
			}
			static auto dec(Byte const * const & src, Byte * const & dst, ImgSz const & sz) {
				auto cnt = kSzN;
				for (auto y = kNo1; y < sz.$h / 4; ++y) {
					for (auto x = kNo1; x < sz.$w / 4; ++x) {
						::decompressBlockETC2(
							vRevEndian(*reinterpret_cast<IU32 const *>(src + 8 * cnt)),
							vRevEndian(*reinterpret_cast<IU32 const *>(src + 8 * cnt + 4)),
							dst, sz.$w, sz.$h, x * 4, y * 4
						);
						++cnt;
					}
				}
				return;
			}

		}

		Void enc(Bitmap const & src, ByteSection & dst) {
			ByteSection tmp;
			tmp.allocThenUse(src.calcSz(32));
			src.getData<TexFmt::kRGBAd32x8888eB>(tmp);
			enc(tmp.$data, dst.$data, src.$sz);
			tmp.free();
			return;
		}
		Void dec(ByteSection const & src, Bitmap & dst) {
			ByteSection tmp;
			tmp.allocThenUse(dst.calcSz(24));
			dec(src.$data, tmp.$data, dst.$sz);
			dst.setData<TexFmt::kRGBd24x888eB>(tmp);
			tmp.free();
			return;
		}

	}

}
