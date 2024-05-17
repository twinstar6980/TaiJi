#include "./Packer.hpp"

#include "./Info.hpp"
#include "./Struct.hpp"

#include "../../../Type/JSON.hpp"
#include "../../../Type/BlockFunc.hpp"
#include "../../File/File.hpp"

namespace $TJ::$Util::$ResPkg::$PAK::$Packer {

	namespace $Pack {

		extern Void pack(ByteSection & pkg, $Info::Info const & info, Path const & resDir) {
			assert(pkg.isEmpty());
			ByteStream stm;
			stm.ref(pkg.$data, pkg.$cap).setPosBeg();
			$Struct::Header header;
			List<$Struct::ResInfo> resInfoList;
			{
				header.$magic = info.$magic;
				header.$ver = info.$ver;
				$StaticBlock::write(header, stm);
			}
			auto posOfResInfo = stm.$pos;
			{
				resInfoList.alloc(info.$res.$sz);
				for (auto const & res : info.$res) {
					auto & resInfo = resInfoList.appendX().last();
					resInfo.$path.$str = res.$path.fullName();
					resInfo.$time = res.$time;
					stm.posGo<Byte>().posGo(resInfo.sz());
				}
			}
			stm.posGo<Byte>();
			{
				Path parentDir; $File::getCurDirThenChgDir(parentDir, resDir);
				for (auto i = kNo1; i < info.$res.$sz; ++i) {
					ByteSection resData;
					resData.ref(stm, stm.$pos, kSzN, stm.idleSz());
					$File::readFile(info.$res[i].$path, resData);
					resInfoList[i].$sz = resData.$sz;
					stm.posGo(resData.$sz);
				}
				$File::chgDirThenFree(parentDir);
			}
			auto wholeSz = stm.$pos;
			{
				stm.setPosBeg(posOfResInfo);
				for (auto const & resInfo : resInfoList) {
					stm << $Struct::$EndFlag::kNext;
					resInfo.write(stm);
				}
				stm << $Struct::$EndFlag::kStop;
				stm.setPosBeg(wholeSz);
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

		extern Void unpack(ByteSection const & pkg, $Info::Info & info, Path const & resDir) {
			ByteStream stm;
			stm.ref(pkg).setPosBeg();
			$Struct::Header header;
			List<$Struct::ResInfo> resInfo;
			$StaticBlock::read(header, stm);
			resInfo.alloc();
			while (kBoT) {
				auto shouldBreak = kBoF;
				switch (stm.rd<Byte>()) {
					default:
						stm.posBack<Byte>();
						throw VAMGException("PAKUnpacker", "[0x%X] unknown flag : 0x%08X", stm.$pos, stm.rdNoPos<Byte>());
						break;
					case $Struct::$EndFlag::kStop:
						shouldBreak = kBoT;
						break;
					case $Struct::$EndFlag::kNext:
						resInfo.appendX().last().read(stm);
						break;
				}
				if (shouldBreak)
					break;
			}
			{
				info.$magic = header.$magic;
				info.$ver = header.$ver;
				info.$res.alloc(resInfo.$sz);
				for (auto const & e : resInfo) {
					auto & res = info.$res.appendX().last();
					res.$path.parse(e.$path.$str);
					res.$time = e.$time;
				}
			}
			if (!resDir.isNull()) {
				Path parentDir; $File::getCurDirThenChgDirForce(parentDir, resDir);
				for (auto const & e : resInfo) {
					$File::writeFile(Path(e.$path.$str), ByteSection().ref(stm, stm.$pos, e.$sz));
					stm.posGo(e.$sz);
				}
				$File::chgDirThenFree(parentDir);
			}
			stm.unref();
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
