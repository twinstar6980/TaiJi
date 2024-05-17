# include "./PVRTCEncoder.h"

# include "../../../lib/PVRTCCompressor/Bitmap.h"
# include "../../../lib/PVRTCCompressor/RgbBitmap.h"
# include "../../../lib/PVRTCCompressor/RgbaBitmap.h"
# include "../../../lib/PVRTCCompressor/PvrTcEncoder.h"
# include "../../../lib/PVRTCCompressor/PvrTcDecoder.h"

namespace $TJ::$ImageUtil::$Encoder::$PVRTCEncoder {
	namespace $PVRTC_4BPP {
		static auto CnvColorRGBToByte(Javelin::ColorRgb<unsigned char> const * const & src, Bitmap & dst) {
			for_cnt(y, kThFirst, dst._sz._h)
				for_cnt(x, kThFirst, dst._sz._w) {
				dst[y][x][0] = src[(y * dst._sz._w) + x].r;
				dst[y][x][1] = src[(y * dst._sz._w) + x].g;
				dst[y][x][2] = src[(y * dst._sz._w) + x].b;
			}
			return;
		}
		static auto CnvColorRGBAToByte(Javelin::ColorRgba<unsigned char> const * const & src, Bitmap & dst) {
			for_cnt(y, kThFirst, dst._sz._h)
				for_cnt(x, kThFirst, dst._sz._w) {
				dst[y][x][0] = src[(y * dst._sz._w) + x].r;
				dst[y][x][1] = src[(y * dst._sz._w) + x].g;
				dst[y][x][2] = src[(y * dst._sz._w) + x].b;
				dst[y][x][3] = src[(y * dst._sz._w) + x].a;
			}
			return;
		}
		static auto CnvByteToColorRGB(Bitmap const & src, Javelin::ColorRgb<unsigned char> * const & dst) {
			for_cnt(y, kThFirst, src._sz._h)
				for_cnt(x, kThFirst, src._sz._w) {
				dst[(y * src._sz._w) + x].r = src[y][x][0];
				dst[(y * src._sz._w) + x].g = src[y][x][1];
				dst[(y * src._sz._w) + x].b = src[y][x][2];
			}
			return;
		}
		static auto CnvByteToColorRGBA(Bitmap const & src, Javelin::ColorRgba<unsigned char> * const & dst) {
			for_cnt(y, kThFirst, src._sz._h)
				for_cnt(x, kThFirst, src._sz._w) {
				dst[(y * src._sz._w) + x].r = src[y][x][0];
				dst[(y * src._sz._w) + x].g = src[y][x][1];
				dst[(y * src._sz._w) + x].b = src[y][x][2];
				dst[(y * src._sz._w) + x].a = src[y][x][3];
			}
			return;
		}

		Void EnCodeRGB(Bitmap const & src, Byte * const & dst) {
			auto bitmapOfSrc = new Javelin::RgbBitmap(src._sz._w, src._sz._h);
			CnvByteToColorRGB(src, (*bitmapOfSrc).GetData());
			Javelin::PvrTcEncoder::EncodeRgb4Bpp(dst, *bitmapOfSrc);
			delPtrSingle(bitmapOfSrc);
			return;
		}
		Void EnCodeRGBA(Bitmap const & src, Byte * const & dst) {
			auto bitmapOfSrc = new Javelin::RgbaBitmap(src._sz._w, src._sz._h);
			CnvByteToColorRGBA(src, (*bitmapOfSrc).GetData());
			Javelin::PvrTcEncoder::EncodeRgba4Bpp(dst, *bitmapOfSrc);
			delPtrSingle(bitmapOfSrc);
			return;
		}
		Void DeCodeRGB(Byte const * const & src, Bitmap & dst) {
			auto result = new Javelin::ColorRgba<unsigned char>[dst._sz.getSz()];
			Javelin::PvrTcDecoder::DecodeRgb4Bpp(result, Javelin::Point2<int>(dst._sz._w, dst._sz._w), src);
			CnvColorRGBToByte(result, dst);
			delPtrArr(result);
			return;
		}
		Void DeCodeRGBA(Byte const * const & src, Bitmap & dst) {
			auto result = new Javelin::ColorRgba<unsigned char>[dst._sz.getSz()];
			Javelin::PvrTcDecoder::DecodeRgba4Bpp(result, Javelin::Point2<int>(dst._sz._w, dst._sz._w), src);
			CnvColorRGBAToByte(result, dst);
			delPtrArr(result);
			return;
		}
	}
}
