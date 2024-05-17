#include "./PVRTC.hpp"

#include "../../../../dependency/PVRTCCompressor/Bitmap.h"
#include "../../../../dependency/PVRTCCompressor/RgbBitmap.h"
#include "../../../../dependency/PVRTCCompressor/RgbaBitmap.h"
#include "../../../../dependency/PVRTCCompressor/PvrTcEncoder.h"
#include "../../../../dependency/PVRTCCompressor/PvrTcDecoder.h"

namespace $TJ::$Util::$Image::$Encoder::$PVRTC {

	namespace $4BPP {

		namespace {

			static auto CnvColorRGBToByte(::Javelin::ColorRgb<unsigned char> const * const & src, Bitmap & dst) {
				for (auto y = kNo1; y < dst.$sz.$h; ++y) {
					for (auto x = kNo1; x < dst.$sz.$w; ++x) {
						dst[y][x][0] = src[(y * dst.$sz.$w) + x].r;
						dst[y][x][1] = src[(y * dst.$sz.$w) + x].g;
						dst[y][x][2] = src[(y * dst.$sz.$w) + x].b;
					}
				}
				return;
			}
			static auto CnvColorRGBAToByte(::Javelin::ColorRgba<unsigned char> const * const & src, Bitmap & dst) {
				for (auto y = kNo1; y < dst.$sz.$h; ++y) {
					for (auto x = kNo1; x < dst.$sz.$w; ++x) {
						dst[y][x][0] = src[(y * dst.$sz.$w) + x].r;
						dst[y][x][1] = src[(y * dst.$sz.$w) + x].g;
						dst[y][x][2] = src[(y * dst.$sz.$w) + x].b;
						dst[y][x][3] = src[(y * dst.$sz.$w) + x].a;
					}
				}
				return;
			}
			static auto CnvByteToColorRGB(Bitmap const & src, ::Javelin::ColorRgb<unsigned char> * const & dst) {
				for (auto y = kNo1; y < src.$sz.$h; ++y) {
					for (auto x = kNo1; x < src.$sz.$w; ++x) {
						dst[(y * src.$sz.$w) + x].r = src[y][x][0];
						dst[(y * src.$sz.$w) + x].g = src[y][x][1];
						dst[(y * src.$sz.$w) + x].b = src[y][x][2];
					}
				}
				return;
			}
			static auto CnvByteToColorRGBA(Bitmap const & src, ::Javelin::ColorRgba<unsigned char> * const & dst) {
				for (auto y = kNo1; y < src.$sz.$h; ++y) {
					for (auto x = kNo1; x < src.$sz.$w; ++x) {
						dst[(y * src.$sz.$w) + x].r = src[y][x][0];
						dst[(y * src.$sz.$w) + x].g = src[y][x][1];
						dst[(y * src.$sz.$w) + x].b = src[y][x][2];
						dst[(y * src.$sz.$w) + x].a = src[y][x][3];
					}
				}
				return;
			}

		}

		Void encRGB(Bitmap const & src, ByteSection & dst) {
			::Javelin::RgbBitmap bitmapOfSrc(src.$sz.$w, src.$sz.$h);
			CnvByteToColorRGB(src, bitmapOfSrc.GetData());
			::Javelin::PvrTcEncoder::EncodeRgb4Bpp(dst.$data, bitmapOfSrc);
			return;
		}
		Void encRGBA(Bitmap const & src, ByteSection & dst) {
			::Javelin::RgbaBitmap bitmapOfSrc(src.$sz.$w, src.$sz.$h);
			CnvByteToColorRGBA(src, bitmapOfSrc.GetData());
			::Javelin::PvrTcEncoder::EncodeRgba4Bpp(dst.$data, bitmapOfSrc);
			return;
		}
		Void decRGB(ByteSection const & src, Bitmap & dst) {
			auto result = new ::Javelin::ColorRgba<unsigned char>[dst.$sz.calcSz()];
			::Javelin::PvrTcDecoder::DecodeRgb4Bpp(result, ::Javelin::Point2<int>(dst.$sz.$w, dst.$sz.$w), src.$data);
			CnvColorRGBToByte(result, dst);
			pFreeN(result);
			return;
		}
		Void decRGBA(ByteSection const & src, Bitmap & dst) {
			auto result = new ::Javelin::ColorRgba<unsigned char>[dst.$sz.calcSz()];
			::Javelin::PvrTcDecoder::DecodeRgba4Bpp(result, ::Javelin::Point2<int>(dst.$sz.$w, dst.$sz.$w), src.$data);
			CnvColorRGBAToByte(result, dst);
			pFreeN(result);
			return;
		}

	}

}
