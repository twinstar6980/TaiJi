# include "./ETCEncoder.h"

# include "../../../lib/ETCPACK/_mylinker.h"

# include "../../UtilKit/MemoryUtil.h"
# include "../../UtilKit/DevUtil.h"

# include "../../../lib/rg_etc1/rg_etc1.h"

namespace $TJ::$ImageUtil::$Encoder::$ETCEncoder {
	namespace $ETC1 {
		static auto EnCode(Byte const * const & src, Byte * const & dst, ImageSize const & sz) {
			rg_etc1::etc1_pack_params etc1PackParm;
			UI32 tmpBlock[16] = { 0 };
			auto cnt = kThFirst;
			for_cnt(y, kThFirst, (sz._h / 4)) {
				wLog("\r>>> img-enc : %.1f%%", (static_cast<F64>(y) * 4.0 / static_cast<F64>(sz._h)) * 100.0);
				for_cnt(x, kThFirst, (sz._w / 4)) {
					auto const & srcBlockBase = src + 4 * (y * 4 * sz._w + x * 4);
					memcpy(tmpBlock + 0, srcBlockBase + sz._w * 4 * 0, 4 * 4);
					memcpy(tmpBlock + 4, srcBlockBase + sz._w * 4 * 1, 4 * 4);
					memcpy(tmpBlock + 8, srcBlockBase + sz._w * 4 * 2, 4 * 4);
					memcpy(tmpBlock + 12, srcBlockBase + sz._w * 4 * 3, 4 * 4);
					rg_etc1::pack_etc1_block_init();
					rg_etc1::pack_etc1_block(dst + 8 * cnt, tmpBlock, etc1PackParm);
					++cnt;
				}
			}
			wLogLn("\r>>> img-enc : 100.0%%");
			return;
		}
		static auto DeCode(Byte const * const & src, Byte * const & dst, ImageSize const & sz) {
			auto cnt = kThFirst;
			for_cnt(y, kThFirst, (sz._h / 4)) {
				for_cnt(x, kThFirst, (sz._w / 4)) {
					decompressBlockETC2(
						cnvEndian(*reinterpret_cast<UI32 const *>(src + 8 * cnt)),
						cnvEndian(*reinterpret_cast<UI32 const *>(src + 8 * cnt + 4)),
						dst, sz._w, sz._h, x * 4, y * 4
					);
					++cnt;
				}
			}
			return;
		}

		Void EnCode(Bitmap const & src, Byte * const & dst) {
			Byte * tmp(kNullPtr);
			newPtr(tmp, src.getSz(32));
			src.Get_RGBAd32x8888eB(tmp);
			EnCode(tmp, dst, src._sz);
			delPtrArr(tmp);
			return;
		}
		Void DeCode(Byte const * const & src, Bitmap & dst) {
			Byte * tmp(kNullPtr);
			newPtr(tmp, dst.getSz(24));
			DeCode(src, tmp, dst._sz);
			dst.Set_RGBd24x888eB(tmp);
			delPtrArr(tmp);
			return;
		}
	}
}
