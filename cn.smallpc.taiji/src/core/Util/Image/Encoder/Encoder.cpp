#include "./Encoder.hpp"

#include "../../File/File.hpp"

#include "../ImgFile.hpp"
#include "../InfoType.hpp"

#include "./ETC.hpp"
#include "./PVRTC.hpp"

namespace $TJ::$Util::$Image::$Encoder {

	template <typename T>
	inline constexpr auto calcExp2Int(T const & val) {
		T exp = "1"_bin8 << 1;
		while (exp < val)
			exp <<= 1;
		return exp;
	}

	ImgSz calcRealImgSz(ImgSz const & sz, TexFmt const & fmt) {
		ImgSz r;
		switch (fmt) {
			default:
				r.set(sz.$w, sz.$h);
				break;
			case TexFmt::kRGBcETC1:
			case TexFmt::kRGBcETC1sAd8:
			case TexFmt::kRGBcETC1sAdVmSpecialOfPvZ2CHSAndroid:
				r.set(calcExp2Int(sz.$w), calcExp2Int(sz.$h));
				break;
			case TexFmt::kRGBAcPVRTC2BPP:
			case TexFmt::kRGBAcPVRTC4BPP:
			case TexFmt::kRGBAcPVRTC4BPPsAd8:
				r.set(calcExp2Int(sz.$w), calcExp2Int(sz.$h));
				if (r.$w > r.$h)
					r.$h = r.$w;
				else
					r.$w = r.$h;
				break;
			case TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB:
			case TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB:
			case TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB:
			case TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB:
				r.set(sz.$w, sz.$h);
				break;
		}
		return r;
	}

	Sz calcImgByteSz(ImgSz const & sz, TexFmt const & fmt) {
		Sz r;
		switch (fmt) {
			default:
				throw VAMGException("Image-CalcTextureByteSize", "error texture format");
				break;
			case TexFmt::kNull:
				log.war("null texture format");
				r = kSzN;
				break;
			case TexFmt::kRGBAd16x4444eL:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kRGBd16x565eL:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kRGBAd16x5551eL:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kRGBd24x888eB:
				r = sz.calcByteSz(24);
				break;
			case TexFmt::kRGBAd32x8888eB:
				r = sz.calcByteSz(32);
				break;
			case TexFmt::kARGBd16x4444eL:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kARGBd32x8888eL:
				r = sz.calcByteSz(32);
				break;
			case TexFmt::kARGBd32x8888eLsAd8:
				r = sz.calcByteSz(32 + 8);
				break;
			case TexFmt::kRGBcETC1:
				r = sz.calcByteSz($ETC::$ETC1::kBPP);
				break;
			case TexFmt::kRGBcETC1sAd8:
				r = sz.calcByteSz($ETC::$ETC1::kBPP + 8);
				break;
			case TexFmt::kRGBAcPVRTC4BPP:
				r = sz.calcByteSz($PVRTC::$4BPP::kBPP);
				break;
			case TexFmt::kRGBAcPVRTC4BPPsAd8:
				r = sz.calcByteSz($PVRTC::$4BPP::kBPP + 8);
				break;
			case TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB:
				r = sz.calcByteSz(16);
				break;
			case TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB:
				r = sz.calcByteSz(32);
				break;
		}
		return r;
	}

	namespace $EnCode {

		Void encProto(Bitmap const & img, ByteSection & data, TexFmt const & fmt) {
			assert(data.isNull());
			data.allocThenUse(calcImgByteSz(img.$sz, fmt));
			switch (fmt) {
				default:
					throw VAMGException("Image-Encoder", "error texture format");
					break;
				case TexFmt::kNull:
					log.war("null texture format");
					break;
				case TexFmt::kRGBAd16x4444eL:
					img.getData<TexFmt::kRGBAd16x4444eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBd16x565eL:
					img.getData<TexFmt::kRGBd16x565eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBAd16x5551eL:
					img.getData<TexFmt::kRGBAd16x5551eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBd24x888eB:
					img.getData<TexFmt::kRGBd24x888eB>(ByteSection().ref(data, kNo1, img.calcSz(24)));
					break;
				case TexFmt::kRGBAd32x8888eB:
					img.getData<TexFmt::kRGBAd32x8888eB>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					break;
				case TexFmt::kARGBd16x4444eL:
					img.getData<TexFmt::kARGBd16x4444eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kARGBd32x8888eL:
					img.getData<TexFmt::kARGBd32x8888eL>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					break;
				case TexFmt::kARGBd32x8888eLsAd8:
					img.getData<TexFmt::kARGBd32x8888eL>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					img.getDataAplha(ByteSection().ref(data, img.calcSz(32), img.calcSz(8)));
					break;
				case TexFmt::kRGBcETC1:
					$ETC::$ETC1::enc(img, ByteSection().ref(data, kNo1, img.calcSz($ETC::$ETC1::kBPP)));
					break;
				case TexFmt::kRGBcETC1sAd8:
					$ETC::$ETC1::enc(img, ByteSection().ref(data, kNo1, img.calcSz($ETC::$ETC1::kBPP)));
					img.getDataAplha(ByteSection().ref(data, img.calcSz($ETC::$ETC1::kBPP), img.calcSz(8)));
					break;
				case TexFmt::kRGBAcPVRTC4BPP:
					$PVRTC::$4BPP::encRGBA(img, ByteSection().ref(data, kNo1, img.calcSz($PVRTC::$4BPP::kBPP)));
					break;
				case TexFmt::kRGBAcPVRTC4BPPsAd8:
					$PVRTC::$4BPP::encRGBA(img, ByteSection().ref(data, kNo1, img.calcSz($PVRTC::$4BPP::kBPP)));
					img.getDataAplha(ByteSection().ref(data, img.calcSz($PVRTC::$4BPP::kBPP), img.calcSz(8)));
					break;
				case TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB:
					img.getData<TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB:
					img.getData<TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB:
					img.getData<TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB:
					img.getData<TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBcETC1sAdVmSpecialOfPvZ2CHSAndroid:
					$ETC::$ETC1::enc(img, ByteSection().ref(data, kNo1, img.calcSz($ETC::$ETC1::kBPP)));
					//img.getData<TexFmt::kAdVmSpecialOfPvZ2CHSAndroid>(ByteSection().ref(data, img.calcSz($ETC::$ETC1::kBPP), img.calcSz(8)));
					break;
			}
			return;
		}

		Void enc(Bitmap const & img, ByteSection & data, ImgSz const & sz, TexFmt const & fmt) {
			auto realSz = calcRealImgSz(sz, fmt);
			Bitmap realImg;
			auto shouldReDraw = realSz.$w > img.$sz.$w || realSz.$h > img.$sz.$h;
			if (shouldReDraw) {
				ImgSz validSz;
				validSz.$w = vMin(realSz.$w, img.$sz.$w);
				validSz.$h = vMin(realSz.$h, img.$sz.$h);
				realImg.alloc(realSz);
				realImg.cpy(img, ImgPos(kNo1, kNo1), validSz, ImgPos(kNo1, kNo1));
			} else {
				realImg.bind(img, ImgArea().set(kNo1, kNo1, realSz.$w, realSz.$h));
			}
			encProto(realImg, data, fmt);
			if (shouldReDraw) {
				realImg.free();
			} else {
				realImg.unbind();
			}
			return;
		}

		Void enc(Path const & imgFile, Path const & dataFile, ImgSz const & sz, TexFmt const & fmt) {
			Bitmap img;
			ByteSection data;
			$ImgFile::$PNG::read(imgFile, img);
			enc(img, data, sz, fmt);
			$File::writeFile(dataFile, data);
			data.free();
			img.free();
			return;
		}

		Void enc(AtlasInfo const & info, Path const & imgDir, Path const & dataDir, Str const & dataFileExtension) {
			return enc((imgDir + info.$path).setExtension("png"_r), (dataDir + info.$path).setExtension(dataFileExtension), info.$sz, info.$fmt);
		}
		Void enc(List<AtlasInfo> const & info, Path const & imgDir, Path const & dataDir, Str const & dataFileExtension) {
			for (auto const & e : info)
				enc(e, imgDir, dataDir, dataFileExtension);
			return;
		}

		extern Void encExport(Path const & imgFile, Path const & dataFile, ImgSz const & sz, Str const & fmt) {
			return enc(imgFile, dataFile, sz, getTexFmt(fmt));
		}
		extern Void encByAtlasInfoExport(Path const & infoFile, Path const & imgDir, Path const & dataDir, Str const & dataFileExtension) {
			$JSON::Val info;
			$JSON::read(infoFile, info);
			if (info.isArr())
				enc(InfoList<AtlasInfo>().read(info), imgDir, dataDir, dataFileExtension);
			else
				enc(AtlasInfo().read(info), imgDir, dataDir, dataFileExtension);
			return;
		}

	}

	namespace $DeCode {

		Void dec(ByteSection const & data, Bitmap & img, ImgSz const & sz, TexFmt const & fmt) {
			assert(img.isNull());
			auto realSz = calcRealImgSz(sz, fmt);
			img.alloc(realSz);
			switch (fmt) {
				default:
					throw VAMGException("Image-Decoder", "error texture format");
					break;
				case TexFmt::kNull:
					log.war("null texture format");
					img.clear();
					break;
				case TexFmt::kRGBAd16x4444eL:
					img.setData<TexFmt::kRGBAd16x4444eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBd16x565eL:
					img.setData<TexFmt::kRGBd16x565eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					img.setDataAlpha();
					break;
				case TexFmt::kRGBAd16x5551eL:
					img.setData<TexFmt::kRGBAd16x5551eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBd24x888eB:
					img.setData<TexFmt::kRGBd24x888eB>(ByteSection().ref(data, kNo1, img.calcSz(24)));
					img.setDataAlpha();
					break;
				case TexFmt::kRGBAd32x8888eB:
					img.setData<TexFmt::kRGBAd32x8888eB>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					break;
				case TexFmt::kARGBd16x4444eL:
					img.setData<TexFmt::kARGBd16x4444eL>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kARGBd32x8888eL:
					img.setData<TexFmt::kARGBd32x8888eL>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					break;
				case TexFmt::kARGBd32x8888eLsAd8:
					img.setData<TexFmt::kARGBd32x8888eL>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					img.setDataAlpha(ByteSection().ref(data, img.calcSz(32), img.calcSz(8)));
					break;
				case TexFmt::kRGBcETC1:
					$ETC::$ETC1::dec(ByteSection().ref(data, kNo1, img.calcSz($ETC::$ETC1::kBPP)), img);
					img.setDataAlpha();
					break;
				case TexFmt::kRGBcETC1sAd8:
					$ETC::$ETC1::dec(ByteSection().ref(data, kNo1, img.calcSz($ETC::$ETC1::kBPP)), img);
					img.setDataAlpha(ByteSection().ref(data, img.$sz.calcByteSz($ETC::$ETC1::kBPP), img.calcSz(8)));
					break;
				case TexFmt::kRGBAcPVRTC4BPP:
					$PVRTC::$4BPP::decRGBA(ByteSection().ref(data, kNo1, img.calcSz($PVRTC::$4BPP::kBPP)), img);
					break;
				case TexFmt::kRGBAcPVRTC4BPPsAd8:
					$PVRTC::$4BPP::decRGBA(ByteSection().ref(data, kNo1, img.calcSz($PVRTC::$4BPP::kBPP)), img);
					img.setDataAlpha(ByteSection().ref(data, img.$sz.calcByteSz($PVRTC::$4BPP::kBPP), img.calcSz(8)));
					break;
				case TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB:
					img.setData<TexFmt::kRGBAd16x4444eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB:
					img.setData<TexFmt::kRGBd16x565eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					img.setDataAlpha();
					break;
				case TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB:
					img.setData<TexFmt::kRGBAd16x5551eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(16)));
					break;
				case TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB:
					img.setData<TexFmt::kARGBd32x8888eLmSpecialOfPvZ1RSB>(ByteSection().ref(data, kNo1, img.calcSz(32)));
					break;
				case TexFmt::kAdVmSpecialOfPvZ2CHSAndroid:
					img.setData<TexFmt::kAdVmSpecialOfPvZ2CHSAndroid>(ByteSection().ref(data, kNo1, img.calcSz(4)));
					break;
				case TexFmt::kRGBcETC1sAdVmSpecialOfPvZ2CHSAndroid:
					$ETC::$ETC1::dec(ByteSection().ref(data, kNo1, img.calcSz($ETC::$ETC1::kBPP)), img);
					img.setData<TexFmt::kAdVmSpecialOfPvZ2CHSAndroid>(ByteSection().ref(data, img.$sz.calcByteSz($ETC::$ETC1::kBPP)));
					break;
			}
			return;
		}

		Void dec(Path const & dataFile, Path const & imgFile, ImgSz const & sz, TexFmt const & fmt) {
			ByteSection data;
			Bitmap img;
			$File::readFileForce(dataFile, data);
			dec(data, img, sz, fmt);
			data.free();
			$ImgFile::$PNG::write(imgFile, img, ImgArea().set(kNo1, kNo1, sz.$w, sz.$h));
			img.free();
			return;
		}

		Void dec(AtlasInfo const & info, Path const & dataDir, Path const & imgDir, Str const & dataFileExtension) {
			return dec((dataDir + info.$path).setExtension(dataFileExtension), (imgDir + info.$path).setExtension("png"_r), info.$sz, info.$fmt);
		}
		Void dec(List<AtlasInfo> const & info, Path const & dataDir, Path const & imgDir, Str const & dataFileExtension) {
			for (auto const & e : info)
				dec(e, dataDir, imgDir, dataFileExtension);
			return;
		}

		extern Void decExport(Path const & dataFile, Path const & imgFile, ImgSz const & sz, Str const & fmt) {
			return dec(dataFile, imgFile, sz, getTexFmt(fmt));
		}
		extern Void decByAtlasInfoExport(Path const & infoFile, Path const & dataDir, Path const & imgDir, Str const & dataFileExtension) {
			$JSON::Val info;
			$JSON::read(infoFile, info);
			if (info.isArr())
				dec(InfoList<AtlasInfo>().read(info), dataDir, imgDir, dataFileExtension);
			else
				dec(AtlasInfo().read(info), dataDir, imgDir, dataFileExtension);
			return;
		}

	}

}
