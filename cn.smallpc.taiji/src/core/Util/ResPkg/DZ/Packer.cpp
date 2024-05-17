#include "./Packer.hpp"

#include "./Struct.hpp"
#include "./Info.hpp"

#include "../../../Type/BlockFunc.hpp"
#include "../../../Type/List.hpp"
#include "../../../Type/JSON.hpp"
#include "../../File/File.hpp"

#include "../../Data/Compressor/LZMA.hpp"

namespace $TJ::$Util::$ResPkg::$DZ::$Packer {

	namespace $Pack {

		static Void parseInfo($Info::Info const & info, List<$Struct::ResPathInfo> & resPathInfo, List<Str> & resDir, List<Str> & resName) {
			resDir.alloc(kSz1 + info.$res.$sz).append(kEmptyStr);
			resName.alloc(info.$res.$sz);
			resPathInfo.alloc(info.$res.$sz);
			for (auto const & res : info.$res) {
				auto & pathInfo = resPathInfo.appendX().last();
				if (Sz i; resDir.range().find(res.$path.dirName(), i)) {
					pathInfo.$dirIdx = static_cast<IU16>(i);
				} else {
					resDir.append(res.$path.dirName());
					pathInfo.$dirIdx = static_cast<IU16>(resDir.lastIdx());
				}
				resName.append(res.$path.fileName());
				pathInfo.$nameIdx = static_cast<IU16>(resName.lastIdx());
				pathInfo.$end = $Struct::$Const::$ResPathInfo::kEnd;
			}
			return;
		}

		extern Void pack(ByteSection & pkg, $Info::Info const & info, Path const & resDir) {
			assert(pkg.isEmpty());
			ByteStream stm;
			stm.ref(pkg.$data, pkg.$cap).setPosBeg();
			List<$Struct::ResPathInfo> resPathInfoList;
			List<$Struct::ResDataInfo> resDataInfoList;
			List<Str> resDirList, resNameList;
			parseInfo(info, resPathInfoList, resDirList, resNameList);
			{
				$Struct::Header header;
				header.$id = $Struct::$Const::$Header::kID;
				header.$dirSum = static_cast<IU16>(resDirList.$sz);
				header.$nameSum = static_cast<IU16>(resNameList.$sz);
				$StaticBlock::write(header, stm);
			}
			stm << kByteNull;
			$Str::writeStrToRaw(resNameList, stm);
			$Str::writeStrToRaw(List<Str>().ref(resDirList, kNo2), stm);
			$StaticBlock::write(resPathInfoList, stm);
			{
				$Struct::UnKnownBlock unKnownBlock;
				unKnownBlock.$unk1 = $Struct::$Const::$UnKnownBlock::kUnKnown1;
				unKnownBlock.$unkE = static_cast<IU16>(info.$res.$sz);
				$StaticBlock::write(unKnownBlock, stm);
			}
			{
				Path parentDir; $File::getCurDirThenChgDir(parentDir, resDir);
				auto posOfDataInfo = stm.$pos;
				stm.posGo($Struct::ResDataInfo::kSz * resPathInfoList.$sz);
				resDataInfoList.alloc(info.$res.$sz);
				for (auto const & res : info.$res) {
					auto & dataInfo = resDataInfoList.appendX().last();
					dataInfo.$flag = res.$flag;
					switch (res.$flag) {
						default:
							throw VAMGException("DZPacker", "unknown res-data store format : %u", res.$flag);
							break;
						case 0x100: {
							ByteSection data;
							data.ref(stm, stm.$pos, kSzN, stm.idleSz());
							dataInfo.$ofs = stm.$pos;
							$File::readFile(res.$path, data);
							dataInfo.$sz = data.$sz;
							dataInfo.$sz2 = data.$sz;
							stm.posGo(data.$sz);
							data.unref();
							break;
						}
						case 0x200: {
							// some bug in here
							ByteSection data;
							ByteSection compressData;
							$Struct::CompressHeader512 compressHeader;
							dataInfo.$ofs = stm.$pos;
							$File::readFileForce(res.$path, data);
							dataInfo.$sz = data.$sz;
							dataInfo.$sz2 = data.$sz;
							compressHeader.$sz = data.$sz;
							compressHeader.$null = 0;
							$StaticBlock::write(compressHeader, stm);
							compressData.ref(stm, stm.$pos, kSzN, stm.idleSz());
							$Data::$Compressor::$LZMA::$Compress::compress(
								ByteSection().ref(compressHeader.$props, 5),
								data, compressData
							);
							stm.posGo(compressData.$sz);
							compressData.unref();
							data.free();
							break;
						}
						case 0x8:
							// TODO unknown format on cn ver
							throw VAMGException("DZPacker", "unknown way to compress res-data store format : 8");
							break;
					}
				}
				$File::chgDirThenFree(parentDir);
				auto wholeSz = stm.$pos;
				stm.setPosBeg(posOfDataInfo);
				$StaticBlock::write(resDataInfoList, stm);
				stm.setPosBeg(wholeSz);
			}
			{
				resPathInfoList.free();
				resDataInfoList.free();
				resDirList.free();
				resNameList.free();
			}
			pkg.setSz(stm.$pos);
			stm.unref();
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

		static Void parseData(ByteSection const & data, $Struct::Header & header, List<$Struct::ResPathInfo> & resPathInfo, List<$Struct::ResDataInfo> & resDataInfo, List<Str> & resDir, List<Str> & resName) {
			ByteStream stm;
			stm.ref(data).setPosBeg();
			$StaticBlock::read(header, stm);
			{
				resName.allocThenUse(header.$nameSum);
				resDir.allocThenUse(header.$dirSum);
				resDir.first().cpy(kEmptyStr);
				resPathInfo.allocThenUse(header.$nameSum);
				resDataInfo.allocThenUse(header.$nameSum);
			}
			stm.posGo<Byte>();
			$Str::readStrFromRaw(resName, stm);
			$Str::readStrFromRaw(List<Str>().ref(resDir, kNo2), stm);
			$StaticBlock::read(resPathInfo, stm);
			stm.posGo<Byte>($Struct::UnKnownBlock::kSz);
			$StaticBlock::read(resDataInfo, stm);
			stm.unref();
			return;
		}

		static Void parseInfo($Info::Info & info, List<$Struct::ResPathInfo> const & resPathInfo, List<$Struct::ResDataInfo> & resDataInfo, List<Str> const & resDir, List<Str> const & resName) {
			info.init();
			info.$res.alloc(resPathInfo.$sz);
			for (auto i = kNo1; i < resPathInfo.$sz; ++i) {
				auto & res = info.$res.appendX().last();
				res.$path.parse(resDir[resPathInfo[i].$dirIdx]).append(resName[resPathInfo[i].$nameIdx]);
				res.$flag = resDataInfo[i].$flag;
			}
			return;
		}

		static Void exportRes(ByteSection const & resData, List<$Struct::ResPathInfo> const & resPathInfo, List<$Struct::ResDataInfo> const & resDataInfo, List<Str> const & resDir, List<Str> const & resName, Path const & dstDir) {
			Path parentDir; $File::getCurDirThenChgDirForce(parentDir, dstDir);
			for (auto i = kNo1; i < resPathInfo.$sz; ++i) {
				auto & pathInfo = resPathInfo[i];
				auto & dataInfo = resDataInfo[i];
				Path path;
				path.parse(resDir[pathInfo.$dirIdx]).append(resName[pathInfo.$nameIdx]);
				switch (dataInfo.$flag) {
					default:
						throw VAMGException("DZUnpacker", "unknown res-data store format : %u", dataInfo.$flag);
						break;
					case 0x100:
						$File::writeFile(path, ByteSection().ref(resData, dataInfo.$ofs, dataInfo.$sz));
						break;
					case 0x200: {
						$Struct::CompressHeader512 compressHeader;
						ByteSection uncompressData;
						$StaticBlock::read(compressHeader, ByteSection().ref(resData, dataInfo.$ofs, $Struct::CompressHeader512::kSz));
						uncompressData.alloc(compressHeader.$sz);
						$Data::$Compressor::$LZMA::$UnCompress::uncompress(
							ByteSection().ref(compressHeader.$props, 5),
							ByteSection().ref(resData, dataInfo.$ofs + $Struct::CompressHeader512::kSz),
							uncompressData
						);
						$File::writeFile(path, uncompressData);
						uncompressData.free();
						break;
					}
					case 0x8: {
						// TODO unknown format on cn ver
						throw VAMGException("DZUnpacker", "unknown way to decompress res-data store format : 8");
						/*
						log.war("format is 0x8");
						// unknown flag with LZMA, found on ver.cn, non-found props
						$Struct::$CompressHeader_8::Member compressHeader;
						Byte * tmpData(kNullPtr);
						newPtr(tmpData, resDataInfo._sz);
						$DataBlockUtil::read(resData + resDataInfo._ofs + kThFirst, compressHeader);
						//$DataUtil::$LZMACompressor::UnCompress(compressHeader._props, resData + resDataInfo._ofs + compressHeader.getSz(), $DataUtil::$LZMACompressor::kSrcSize_IgnoreActualSize, tmpData, resDataInfo._sz);
						$DevUtil::WriteData(path, tmpData, resDataInfo._sz);
						delPtrArr(tmpData);*/
						break;
					}
				}
				path.free();
			}
			$File::chgDirThenFree(parentDir);
			return;
		}

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, Path const & resDir) {
			List<$Struct::ResPathInfo> resPathInfoList;
			List<$Struct::ResDataInfo> resDataInfoList;
			List<Str> resDirList, resNameList;
			{
				$Struct::Header header;
				parseData(pkg, header, resPathInfoList, resDataInfoList, resDirList, resNameList);
				parseInfo(info, resPathInfoList, resDataInfoList, resDirList, resNameList);
				if (!resDir.isNull())
					exportRes(pkg, resPathInfoList, resDataInfoList, resDirList, resNameList, resDir);
			}
			{
				resPathInfoList.free();
				resDataInfoList.free();
				resDirList.free();
				resNameList.free();
			}
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
