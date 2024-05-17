# pragma once

# include "./Encoder.h"
# include "./Converter_Export.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Converter {
	namespace $CnvToXFL {
		Void expCnv(Path const & jsonFile, Path const & xflDir, SzI const & imgSz) {
			Cnv($StructInfo::Struct().read($JSON::read(jsonFile, $JSON::Obj().setNull())), xflDir, imgSz);
			return;
		}
		Void E_Cnv_OneStep(Path const & popAnimFile, Path const & xflDir, SzI const & imgSz) {
			Byte * data(kNullPtr); SzI sz;
			$DevUtil::ForceReadData(popAnimFile, data, sz);
			Cnv($Encoder::$DeCode::DeCode(data, sz, $StructInfo::Struct().getThis()), xflDir, imgSz);
			delPtrArr(data);
			return;
		}
	}
	namespace $CnvFromXFL {
		Void expCnv(Path const & xflDir, Path const & jsonFile, SzI const & imgSz) {
			$JSON::write(jsonFile, Cnv(xflDir, $StructInfo::Struct().getThis(), imgSz).write($JSON::Obj().setNull()));
			return;
		}
		Void E_Cnv_OneStep(Path const & xflDir, Path const & popAnimFile, SzI const & imgSz, SzI const & memSz) {
			Byte * data(kNullPtr); SzI sz;
			newPtr(data, memSz);
			$Encoder::$EnCode::EnCode(data, sz, Cnv(xflDir, $StructInfo::Struct().getThis(), imgSz));
			$DevUtil::WriteData(popAnimFile, data, sz);
			delPtrArr(data);
			return;
		}
	}
	namespace $XFLScaleCnv {
		Void E_cnv(Path const & xflDir, SzI const & srcSz, SzI const & dstSz) {
			return cnv(xflDir, srcSz, dstSz);
		}
	}
	namespace $CnvToRipe {
		Void expCnv(Path const & jsonFile, Path const & ripeJSONFile) {
			$JSON::write(ripeJSONFile, Cnv($StructInfo::Struct().read($JSON::read(jsonFile, $JSON::Obj().setNull())), $RipePAMStructInfo::StructInfo().getThis()).write($JSON::Obj().setNull()));
			return;
		}
		Void E_Cnv_OneStep(Path const & popAnimFile, Path const & ripeJSONFile) {
			Byte * data(kNullPtr); SzI sz;
			$DevUtil::ForceReadData(popAnimFile, data, sz);
			$JSON::write(ripeJSONFile, Cnv($Encoder::$DeCode::DeCode(data, sz, $StructInfo::Struct().getThis()), $RipePAMStructInfo::StructInfo().getThis()).write($JSON::Obj().setNull()));
			delPtrArr(data);
			return;
		}
	}
}
