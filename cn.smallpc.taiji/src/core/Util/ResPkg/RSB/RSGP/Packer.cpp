#include "./Packer.hpp"

#include "./Info.hpp"
#include "./Struct.hpp"
#include "../Common.hpp"

#include "../../../../Const.hpp"
#include "../../../../Type/BlockFunc.hpp"
#include "../../../File/File.hpp"
#include "../../../Data/Compressor/ZLib.hpp"

namespace $TJ::$Util::$ResPkg::$RSB::$RSGP::$Packer {

	namespace $Pack {

		extern Void pack(ByteSection & pkg, $Info::Info const & info, Path const & resDir) {
			assert(pkg.isEmpty());
			$Struct::Header header;
			{
				header.$id = $Struct::kHeaderID;
				header.$headerType = info.$headerType;
				header.$resStoreMethod = info.$resStoreMethod.makeInt();
			}
			{
				List<Str> tmpResPathList(info.$res.$sz);
				for (auto const & e : info.$res)
					tmpResPathList.appendX().last() = e.$path.fullName($PathDelimiter::kOnWindows);
				$Str::toUpper(tmpResPathList).range().sortUp();
				header.$headerAndResInfoSectSz = $Struct::Header::kSz + $SpecialStrBlock::calcMinSz(tmpResPathList) + (info.$res.$sz * $Struct::ResDataInfo::kSz) + (info.$res.calcAtlasResSum() * $Struct::ResAtlasInfo::kSz);
				ByteSection().ref(pkg.$data + header.$headerAndResInfoSectSz, calcEnoughSz(header.$headerAndResInfoSectSz, kPaddingSz) - header.$headerAndResInfoSectSz).range().set(kByteNull);
				header.$headerAndResInfoSectSz = calcEnoughSz(header.$headerAndResInfoSectSz, kPaddingSz);
				tmpResPathList.free();
			}
			{
				Path parentDir; $File::getCurDirThenChgDir(parentDir, resDir);
				Map<Str, $Struct::ResInfoSlot> resInfoList;
				resInfoList.alloc(info.$res.$sz);
				auto normalResDataSz = kSzN;
				auto atlasResDataSz = kSzN;
				for (auto const & res : info.$res) {
					auto resSz = $File::getFileSize(res.$path);
					auto & resInfo = resInfoList.appendX().last();
					resInfo.$k = res.$path.fullName($PathDelimiter::kOnWindows);
					resInfo.$v.$data.$ofs = normalResDataSz;
					resInfo.$v.$data.$sz = resSz;
					if (res.isNormalType()) {
						resInfo.$v.$data.$type = $Struct::$ResTypeIdx::kNormal;
						normalResDataSz += calcEnoughSz(resSz, kPaddingSz);
					} else {
						resInfo.$v.$data.$type = $Struct::$ResTypeIdx::kAtlas;
						resInfo.$v.$atlas.$idx = res.getAtlasInfo().$idx;
						resInfo.$v.$atlas.$w = res.getAtlasInfo().$sz.$w;
						resInfo.$v.$atlas.$h = res.getAtlasInfo().$sz.$h;
						atlasResDataSz += calcEnoughSz(resSz, kPaddingSz);
					}
				}
				auto const importResFunc = [](auto const & resList, auto const & filterFunc, ByteSection & dst) {
					auto curOfs = kSzN;
					for (auto const & res : resList) {
						if (filterFunc(res)) {
							ByteSection resData;
							resData.ref(dst, curOfs, kSzN, dst.$cap - curOfs);
							$File::readFile(res.$path, resData);
							ByteSection().ref(dst, curOfs + resData.$sz, calcEnoughSz(resData.$sz, kPaddingSz) - resData.$sz).range().set(kByteNull);
							curOfs += calcEnoughSz(resData.$sz, kPaddingSz);
						}
					}
				};
				{
					ByteSection normalResData;
					header.$normalResDataOfs = header.$headerAndResInfoSectSz;
					header.$normalResDataOriSz = normalResDataSz;
					if (!info.$resStoreMethod.$compressNormalRes) {
						normalResData.ref(pkg.$data + header.$normalResDataOfs, header.$normalResDataOriSz);
					} else {
						normalResData.allocThenUse(header.$normalResDataOriSz);
					}
					importResFunc(info.$res,
								  [](auto const & v) {
									  return v.isNormalType();
								  }, normalResData
					);
					if (!info.$resStoreMethod.$compressNormalRes) {
						header.$normalResDataSz = header.$normalResDataOriSz;
						normalResData.unref();
					} else {
						ByteSection tmp;
						tmp.ref(pkg.$data + header.$normalResDataOfs, kSzN, pkg.$cap - header.$normalResDataOfs);
						$Data::$Compressor::$ZLib::$Compress::compress(normalResData, tmp);
						ByteSection().ref(pkg.$data + header.$normalResDataOfs + tmp.$sz, calcEnoughSz(tmp.$sz, kPaddingSz) - tmp.$sz).range().set(kByteNull);
						header.$normalResDataSz = calcEnoughSz(tmp.$sz, kPaddingSz);
						tmp.unref();
						normalResData.free();
					}
				}
				{
					ByteSection atlasResData;
					header.$atlasResDataOfs = header.$headerAndResInfoSectSz + header.$normalResDataSz;
					header.$atlasResDataOriSz = atlasResDataSz;
					if (!info.$resStoreMethod.$compressAtlasRes) {
						atlasResData.ref(pkg.$data + header.$atlasResDataOfs, header.$atlasResDataOriSz);
					} else {
						atlasResData.allocThenUse(header.$atlasResDataOriSz);
					}
					importResFunc(info.$res,
								  [](auto const & v) {
									  return v.isAtlasType();
								  }, atlasResData
					);
					if (!info.$resStoreMethod.$compressAtlasRes) {
						header.$atlasResDataSz = header.$atlasResDataOriSz;
						atlasResData.unref();
					} else {
						if (header.$atlasResDataOriSz == kSzN) {
							header.$atlasResDataSz = header.$atlasResDataOriSz;
						} else {
							ByteSection tmp;
							tmp.ref(pkg.$data + header.$atlasResDataOfs, kSzN, pkg.$cap - header.$atlasResDataOfs);
							$Data::$Compressor::$ZLib::$Compress::compress(atlasResData, tmp);
							ByteSection().ref(pkg.$data + header.$atlasResDataOfs + tmp.$sz, calcEnoughSz(tmp.$sz, kPaddingSz) - tmp.$sz).range().set(kByteNull);
							header.$atlasResDataSz = calcEnoughSz(tmp.$sz, kPaddingSz);
							tmp.unref();
						}
						atlasResData.free();
					}
				}
				{
					header.$resInfoSectOfs = $Struct::Header::kSz;
					ByteSection resInfoSection;
					resInfoSection.ref(pkg.$data + header.$resInfoSectOfs, kSzN, pkg.$cap - header.$resInfoSectOfs);
					$Str::mapKeyUpper(resInfoList);
					resInfoList.sortUpByKey();
					$SpecialStrBlock::enc(resInfoList, resInfoSection);
					header.$resInfoSectSz = resInfoSection.$sz;
				}
				resInfoList.free();
				$File::chgDirThenFree(parentDir);
			}
			$StaticBlock::write(header, pkg);
			pkg.$sz = header.$headerAndResInfoSectSz + header.$normalResDataSz + header.$atlasResDataSz;
			return;
		}

		extern Void packExport(Path const & pkgFile, Path const & infoFile, Path const & resDir, Sz const & memSz) {
			ByteSection pkg;
			$Info::Info info;
			pkg.alloc(memSz);
			info.read($JSON::read(infoFile, crv($JSON::Val())));
			pack(pkg, info, resDir);
			$File::writeFile(pkgFile, pkg);
			pkg.free();
			info.free();
			return;
		}

	}

	namespace $UnPack {

		extern Void parseData(ByteSection const & data, $Struct::Header & header, Map<Str, $Struct::ResInfoSlot> & resInfo) {
			$StaticBlock::read(header, data);
			$SpecialStrBlock::dec(resInfo, ByteSection().ref(data, header.$resInfoSectOfs, header.$resInfoSectSz));
			return;
		}

		extern $Info::Info & parseInfo($Info::Info & info, Sz const & headerType, IU32 const & resStoreMethod, Map<Str, $Struct::ResInfoSlot> const & resInfoList) {
			info.$headerType = headerType;
			info.$resStoreMethod.parseInt(resStoreMethod);
			info.$res.alloc(resInfoList.$sz);
			for (auto const & resInfo : resInfoList) {
				auto & res = info.$res.appendX().last();
				res.$path.parse(resInfo.$k);
				switch (resInfo.$v.$data.$type) {
					default:
						throw VAMGException("RSGPUnpacker", "unknown res-type : %u", resInfo.$v.$data.$type);
						break;
					case $Struct::$ResTypeIdx::kNormal:
						res.setTypeNormal();
						break;
					case $Struct::$ResTypeIdx::kAtlas:
						res.setTypeAtlas();
						res.getAtlasInfo().$idx = resInfo.$v.$atlas.$idx;
						res.getAtlasInfo().$sz.set(resInfo.$v.$atlas.$w, resInfo.$v.$atlas.$h);
						break;
				}
			}
			return info;
		}

		extern Void exportRes(ByteSection const & normalResData, ByteSection const & atlasResData, Map<Str, $Struct::ResInfoSlot> const & resInfoList, Path const & dstDir) {
			Path parentDir; $File::getCurDirThenChgDirForce(parentDir, dstDir);
			for (auto const & resInfo : resInfoList)
				switch (resInfo.$v.$data.$type) {
					default:
						throw VAMGException("RSGPUnpacker", "unknown res-type : %u", resInfo.$v.$data.$type);
						break;
					case $Struct::$ResTypeIdx::kNormal:
						$File::writeFile(Path(resInfo.$k), ByteSection().ref(normalResData, resInfo.$v.$data.$ofs, resInfo.$v.$data.$sz));
						break;
					case $Struct::$ResTypeIdx::kAtlas:
						$File::writeFile(Path(resInfo.$k), ByteSection().ref(atlasResData, resInfo.$v.$data.$ofs, resInfo.$v.$data.$sz));
						break;
				}
			$File::chgDirThenFree(parentDir);
			return;
		}

		extern Void exportResEntry(ByteSection const & data, ResStoreMethod const & resStoreMethod,
								   Sz const & normalResDataOfs, Sz const & normalResDataOriSz, Sz const & normalResDataSz,
								   Sz const & atlasResDataOfs, Sz const & atlasResDataOriSz, Sz const & atlasResDataSz,
								   Map<Str, $Struct::ResInfoSlot> const & resInfo, Path const & dstDir
		) {
			ByteSection normalResData, atlasResData;
			if (!resStoreMethod.$compressNormalRes) {
				normalResData.ref(data, normalResDataOfs, normalResDataSz);
			} else {
				normalResData.alloc(normalResDataOriSz);
				$Data::$Compressor::$ZLib::$UnCompress::uncompress(ByteSection().ref(data, normalResDataOfs, normalResDataSz), normalResData);
			}
			if (!resStoreMethod.$compressAtlasRes) {
				atlasResData.ref(data, atlasResDataOfs, atlasResDataSz);
			} else {
				atlasResData.alloc(atlasResDataOriSz);
				$Data::$Compressor::$ZLib::$UnCompress::uncompress(ByteSection().ref(data, atlasResDataOfs, atlasResDataSz), atlasResData);
			}
			exportRes(normalResData, atlasResData, resInfo, dstDir);
			normalResData.free();
			atlasResData.free();
			return;
		}

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, Path const & resDir) {
			$Struct::Header header;
			Map<Str, $Struct::ResInfoSlot> resInfoList;
			{
				parseData(pkg, header, resInfoList);
				parseInfo(info, header.$headerType, header.$resStoreMethod, resInfoList);
				if (!resDir.isNull())
					exportResEntry(pkg, ResStoreMethod().parseInt(header.$resStoreMethod), header.$normalResDataOfs, header.$normalResDataOriSz, header.$normalResDataSz, header.$atlasResDataOfs, header.$atlasResDataOriSz, header.$atlasResDataSz, resInfoList, resDir);
			}
			resInfoList.free();
			return;
		}

		extern Void unpackExport(Path const & pkgFile, Path const & infoFile, Path const & resDir) {
			ByteSection pkg;
			$Info::Info info;
			$File::readFileForce(pkgFile, pkg);
			unpack(pkg, info, resDir);
			if (!infoFile.isNull())
				$JSON::write(infoFile, info.write(crv($JSON::Val())));
			pkg.free();
			info.free();
			return;
		}

	}

}
