#include "./Converter.hpp"

#include "../../../Type/Map.hpp"
#include "../../File/File.hpp"

#include "./Encoder.hpp"
#include "./Converter/XFL.hpp"
#include "./Converter/CSS.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter {

	namespace $ToXFL {

		extern Void cnvExport(Path const & infoFile, Path const & xflDir, Sz const & imgSz) {
		#if defined ENABLE_PAM_ENCODER_PLAIN
			$Info::Info info;
			info.read($JSON::read(infoFile, crv($JSON::Val())));
			$XFL::$To::cnv(info, xflDir, imgSz);
		#endif
			return;
		}

		extern Void cnvOneStepExport(Path const & dataFile, Path const & xflDir, Sz const & imgSz) {
			ByteSection data;
			$Info::Info info;
			$File::readFileForce(dataFile, data);
			$Encoder::$DeCode::dec(data, info);
			data.free();
			$XFL::$To::cnv(info, xflDir, imgSz);
			return;
		}

	}

	namespace $FromXFL {

		extern Void cnvExport(Path const & infoFile, Path const & xflDir, Sz const & imgSz) {
		#if defined ENABLE_PAM_ENCODER_PLAIN
			$Info::Info info;
			$XFL::$From::cnv(info, xflDir, imgSz);
			$JSON::write(infoFile, info.write(crv($JSON::Val())));
		#endif
			return;
		}

		extern Void cnvOneStepExport(Path const & dataFile, Path const & xflDir, Sz const & imgSz, Sz const & memSz) {
			$Info::Info info;
			$XFL::$From::cnv(info, xflDir, imgSz);
			ByteSection data;
			data.alloc(memSz);
			$Encoder::$EnCode::enc(data, info);
			$File::writeFile(dataFile, data);
			data.free();
			return;
		}

	}

	namespace $XFLScaleCnv {

		extern Void cnvExport(Path const & xflDir, Sz const & srcSz, Sz const & dstSz) {
			$XFL::$ScaleCnv::cnv(xflDir, srcSz, dstSz);
			return;
		}

	}

	namespace $ToCSSInfo {

		extern Void cnvExport(Path const & infoFile, Path const & cssInfoFile) {
		#if defined ENABLE_PAM_ENCODER_PLAIN
			$Info::Info info;
			$CSSInfo::Info cssInfo;
			info.read($JSON::read(infoFile, crv($JSON::Val())));
			$CSS::$To::cnv(info, cssInfo);
			$JSON::write(cssInfoFile, cssInfo.write(crv($JSON::Val())));
		#endif
			return;
		}

		extern Void cnvOneStepExport(Path const & dataFile, Path const & cssInfoFile) {
			ByteSection data;
			$Info::Info info;
			$CSSInfo::Info cssInfo;
			$File::readFileForce(dataFile, data);
			$Encoder::$DeCode::dec(data, info);
			data.free();
			$CSS::$To::cnv(info, cssInfo);
			$JSON::write(cssInfoFile, cssInfo.write(crv($JSON::Val())));
			return;
		}

	}

}
