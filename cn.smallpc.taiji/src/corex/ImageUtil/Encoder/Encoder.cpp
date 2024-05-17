# include "./Encoder.h"

# include "../../MyType/File.h"

# include "../../UtilKit/DevUtil.h"

# include "./ETCEncoder.h"
# include "./PVRTCEncoder.h"
# include "../../SomethingElse.h"

namespace $TJ::$ImageUtil::$Encoder {
	ImageSize & GetActualImageSize(ImageSize const & oriSz, TexFmt const & texFmt, ImageSize & actualSz) {
		switch (texFmt) {
			default:
				actualSz.set(oriSz._w, oriSz._h);
				break;
			case TexFmt::kRGBcETC1:
			case TexFmt::kRGBcETC1sAd8:
			case TexFmt::kRGBcETC1sAcU:
				actualSz.set(GetExp2Int(oriSz._w), GetExp2Int(oriSz._h));
				break;
			case TexFmt::kRGBAcPVRTC2BPP:
			case TexFmt::kRGBAcPVRTC4BPP:
			case TexFmt::kRGBAcPVRTC4BPPsAd8:
				actualSz.set(GetExp2Int(oriSz._w), GetExp2Int(oriSz._h));
				if (actualSz._w > actualSz._h)
					actualSz._h = actualSz._w;
				else actualSz._w = actualSz._h;
				break;
		}
		return actualSz;
	}

	SzI GetActualImageByteSize(ImageSize const & sz, TexFmt const & texFmt) {
		SzI byteSz;
		switch (texFmt) {
			default: wErr("Error Texture Format"); break;
			case TexFmt::kUnKnown: wErr("UnKnown Texture Format"); break;
			case TexFmt::kNull:
				wWarn("Null Texture Format");
				byteSz = kSzZero;
				break;
			case TexFmt::kARGBd16x4444eL:
				byteSz = sz.GetByteSize(16);
				break;
			case TexFmt::kARGBd32x8888eL:
				byteSz = sz.GetByteSize(32);
				break;
			case TexFmt::kARGBd32x8888eLsAd8:
				byteSz = sz.GetByteSize(32 + 8);
				break;
			case TexFmt::kRGBd16x565eL:
				byteSz = sz.GetByteSize(16);
				break;
			case TexFmt::kRGBAd16x5551eL:
				byteSz = sz.GetByteSize(16);
				break;
			case TexFmt::kRGBAd16x4444eL:
				byteSz = sz.GetByteSize(16);
				break;
			case TexFmt::kRGBAd32x8888eB:
				byteSz = sz.GetByteSize(32);
				break;
			case TexFmt::kRGBcETC1:
				byteSz = sz.GetByteSize($ETCEncoder::$ETC1::kBitPerPixel);
				break;
			case TexFmt::kRGBcETC1sAd8:
				byteSz = sz.GetByteSize($ETCEncoder::$ETC1::kBitPerPixel + 8);
				break;
			case TexFmt::kRGBAcPVRTC4BPP:
				byteSz = sz.GetByteSize($PVRTCEncoder::$PVRTC_4BPP::kBitPerPixel);
				break;
			case TexFmt::kRGBAcPVRTC4BPPsAd8:
				byteSz = sz.GetByteSize($PVRTCEncoder::$PVRTC_4BPP::kBitPerPixel + 8);
				break;
		}
		return byteSz;
	}
	namespace $EnCode {
		Void EnCode(Bitmap & src, Byte * const & dst, TexFmt const & texFmt) {
			switch (texFmt) {
				default: wErr("Error Texture Format"); break;
				case TexFmt::kUnKnown: wErr("UnKnown Texture Format"); break;
				case TexFmt::kNull:
					wWarn("Null Texture Format");
					break;
				case TexFmt::kARGBd16x4444eL:
					src.Get_ARGBd16x4444eL(dst);
					break;
				case TexFmt::kARGBd32x8888eL:
					src.Get_ARGBd32x8888eL(dst);
					break;
				case TexFmt::kARGBd32x8888eLsAd8:
					src.Get_ARGBd32x8888eL(dst + kThFirst);
					src.Get_A8(dst + src.getSz(32));
					break;
				case TexFmt::kRGBd16x565eL:
					src.Get_RGBd16x565eL(dst);
					break;
				case TexFmt::kRGBAd16x5551eL:
					src.Get_RGBAd16x5551eL(dst);
					break;
				case TexFmt::kRGBAd16x4444eL:
					src.Get_RGBAd16x4444eL(dst);
					break;
				case TexFmt::kRGBAd32x8888eB:
					src.Get_RGBAd32x8888eB(dst);
					break;
				case TexFmt::kRGBcETC1:
					$ETCEncoder::$ETC1::EnCode(src, dst);
					break;
				case TexFmt::kRGBcETC1sAd8:
					$ETCEncoder::$ETC1::EnCode(src, dst + kThFirst);
					src.Get_A8(dst + src._sz.GetByteSize($ETCEncoder::$ETC1::kBitPerPixel));
					break;
				case TexFmt::kRGBAcPVRTC4BPP:
					$PVRTCEncoder::$PVRTC_4BPP::EnCodeRGBA(src, dst);
					break;
				case TexFmt::kRGBAcPVRTC4BPPsAd8:
					$PVRTCEncoder::$PVRTC_4BPP::EnCodeRGBA(src, dst + kThFirst);
					src.Get_A8(dst + src._sz.GetByteSize($PVRTCEncoder::$PVRTC_4BPP::kBitPerPixel));
					break;
			}
			return;
		}
		Void EnCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt) {
			Bitmap image;
			Byte * rawData(kNullPtr); SzI rawDataSz;
			$FileUtil::$PNGUtil::read(srcFile, image);
			rawDataSz = GetActualImageByteSize(image._sz, texFmt);
			newPtr(rawData, rawDataSz);
			EnCode(image, rawData, texFmt);
			$DevUtil::WriteData(dstFile, rawData, rawDataSz);
			delPtrArr(rawData);
			image.tfree();
			return;
		}
	}
	namespace $DeCode {
		Void DeCode(Byte const * const & src, Bitmap & dst, TexFmt const & texFmt) {
			switch (texFmt) {
				default: wErr("Error Texture Format"); break;
				case TexFmt::kUnKnown: wErr("UnKnown Texture Format"); break;
				case TexFmt::kNull:
					wWarn("Null Texture Format");
					dst.clear();
					break;
				case TexFmt::kARGBd16x4444eL:
					dst.Set_ARGBd16x4444eL(src);
					break;
				case TexFmt::kARGBd32x8888eL:
					dst.Set_ARGBd32x8888eL(src);
					break;
				case TexFmt::kARGBd32x8888eLsAd8:
					dst.Set_ARGBd32x8888eL(src + kThFirst);
					dst.Set_A8(src + dst.getSz(32));
					break;
				case TexFmt::kRGBd16x565eL:
					dst.Set_RGBd16x565eL(src);
					dst.Set_A8();
					break;
				case TexFmt::kRGBAd16x5551eL:
					dst.Set_RGBAd16x5551eL(src);
					break;
				case TexFmt::kRGBAd16x4444eL:
					dst.Set_RGBAd16x4444eL(src);
					break;
				case TexFmt::kRGBAd32x8888eB:
					dst.Set_RGBAd32x8888eB(src);
					break;
				case TexFmt::kRGBcETC1:
					$ETCEncoder::$ETC1::DeCode(src, dst);
					dst.Set_A8();
					break;
				case TexFmt::kRGBcETC1sAd8:
					$ETCEncoder::$ETC1::DeCode(src + kThFirst, dst);
					dst.Set_A8(src + dst._sz.GetByteSize($ETCEncoder::$ETC1::kBitPerPixel));
					break;
				case TexFmt::kRGBAcPVRTC4BPP:
					$PVRTCEncoder::$PVRTC_4BPP::DeCodeRGBA(src, dst);
					break;
				case TexFmt::kRGBAcPVRTC4BPPsAd8:
					$PVRTCEncoder::$PVRTC_4BPP::DeCodeRGBA(src + kThFirst, dst);
					dst.Set_A8(src + dst._sz.GetByteSize($PVRTCEncoder::$PVRTC_4BPP::kBitPerPixel));
					break;
			}
			return;
		}
		Void DeCode(Path const & srcFile, Path const & dstFile, ImageSize const & sz, TexFmt const & texFmt) {
			Bitmap image;
			Byte * rawData(kNullPtr); SzI rawDataSz;
			$DevUtil::ForceReadData(srcFile, rawData, rawDataSz);
			image.alloc(sz);
			DeCode(rawData, image, texFmt);
			delPtrArr(rawData);
			$FileUtil::$PNGUtil::write(dstFile, image);
			image.tfree();
			return;
		}
	}
}
