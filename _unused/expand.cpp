#include "./Expand.hpp"

#include "../../Type/List.hpp"
#include "../../Type/JSON.hpp"

#include "../File/File.hpp"
#include "../Data/Compressor/ZLib.hpp"
#include "../Data/Hasher/MD5.hpp"

#include "../Info/RSBInfoX.hpp"

#include "../Anim/PopcapAnim/Info.hpp"
#include "../Anim/PopcapAnim/Struct.hpp"

#include "../ResPkg/RSB/Info.hpp"
#include "../ResPkg/RSB/Struct.hpp"
#include "../ResPkg/RSB/RSGP/Info.hpp"
#include "../ResPkg/RSB/RSGP/Struct.hpp"
#include "../ResPkg/RSB/RSGP/Packer.hpp"
#include "../ResPkg/RSB/Common.hpp"
#include "../ResPkg/RSB/Packer.hpp"

#include "../Other/Other.hpp"

namespace $TJ::$Util::$Expand {

	namespace $RSBDataDiff {

		Void diff(ByteSection & src, ByteSection & dst) {
			using namespace $ResPkg::$RSB;
			$Struct::Header header;
			Map<Str, $Struct::StrIdxInfoSlot> groupID, subGroupID, resPath;
			List<$Struct::GroupInfo> groupInfo;
			List<$Struct::SubGroupInfo> subGroupInfo;
			List<$Struct::AutoPoolInfo> autoPoolInfo;
			List<$Struct::AtlasResInfo> atlasResInfo;
			List<$Struct::AtlasResExpandInfo> atlasResExpandInfo;
			List<$RSGP::$Struct::Header> rsgpHeader;
			List<Map<Str, $RSGP::$Struct::ResInfoSlot>> rsgpResInfo;
			{
				//$Packer::$UnPack::parseData(pkg, header, groupID, subGroupID, resPath, groupInfo, subGroupInfo, autoPoolInfo, atlasResInfo, atlasResExpandInfo, rsgpHeader, rsgpResInfo, nullptr);
				ByteStream stm;
				//stm.ref(pkg);

			}
			{
				groupID.free();
				subGroupID.free();
				resPath.free();
				groupInfo.free();
				subGroupInfo.free();
				autoPoolInfo.free();
				atlasResInfo.free();
				atlasResExpandInfo.free();
				rsgpHeader.free();
				rsgpResInfo.free();
			}
			return;
		}

	}

	namespace $RSBDataDisturb {

		Void disturb(ByteSection & pkg) {
			using namespace $ResPkg::$RSB;
			$Struct::Header header;
			Map<Str, $Struct::StrIdxInfoSlot> groupID, subGroupID, resPath;
			List<$Struct::GroupInfo> groupInfo;
			List<$Struct::SubGroupInfo> subGroupInfo;
			List<$Struct::AutoPoolInfo> autoPoolInfo;
			List<$Struct::AtlasResInfo> atlasResInfo;
			List<$Struct::AtlasResExpandInfo> atlasResExpandInfo;
			List<$RSGP::$Struct::Header> rsgpHeader;
			List<Map<Str, $RSGP::$Struct::ResInfoSlot>> rsgpResInfo;
			{
				$Packer::$UnPack::parseData(pkg, header, groupID, subGroupID, resPath, groupInfo, subGroupInfo, autoPoolInfo, atlasResInfo, atlasResExpandInfo, rsgpHeader, rsgpResInfo, nullptr);
				ByteStream stm;
				stm.ref(pkg);
				{
					// modify group-info
					stm.setPosBeg(header.$groupInfoListOfs);
					for (auto const & e : groupInfo) {
						// modify id will crash
						if (false) {
							auto randS = $Str::printFmt("%u", randomNum32());
							Str randSH;
							$Data::$Hasher::$MD5::make(randS, randSH);
							stm << randSH;
							stm.posBack(randSH.$sz);
						}
						// modify subgroup-info-list free-space
						{
							ByteStream sgiStm;
							sgiStm.ref(stm, stm.$pos + kTSz<CID128>);
							sgiStm.posGo(e.$subGroup.$sz * TRR<decltype(e.$subGroup.first())>::kSz);
							auto newItemIdx = e.$subGroup.last().$idx;
							for (auto i = e.$subGroup.$sz; i < e.kSubGroupLimit; ++i) {
								$StaticBlock::write($Struct::SubGroupInfoItem{ newItemIdx, 0, 0 }, sgiStm);
								++newItemIdx;
							}
						}
						stm.posGo(header.$groupInfoListSz);
					}
				}
				{
					// modify subgroup-info
					stm.setPosBeg(header.$subGroupInfoListOfs);
					for (auto & e : subGroupInfo) {
						auto is384 = [&groupInfo, &subGroupInfo, &e]() {
							for (auto const & g : groupInfo) {
								for (auto const & sg : g.$subGroup) {
									if (e.$idx == sg.$idx) {
										if (sg.$res == 384) {
											return kBoT;
										}
									}
								}
							}
							return kBoF;
						}();
						// modify id
						{
							auto randS1 = $Str::printFmt("%u", randomNum32());
							auto randS2 = $Str::printFmt("%u", randomNum32());
							Str randSH1, randSH2;
							$Data::$Hasher::$MD5::make(randS1, randSH1);
							$Data::$Hasher::$MD5::make(randS2, randSH2);
							auto randSH = randSH1 + randSH2 + randSH2 + randSH1;
							setCID128(randSH, e.$id);
						}
						// modify rsgp
						{
							ByteStream rsgpStm;
							rsgpStm.ref(pkg, e.$ofs);
							rsgpStm << randomNum32() << randomNum32() << randomNum32() << IU32(0); // 00 ~ 10
							auto storeMode = rsgpStm.rdNoPos<IU32>();
							storeMode |= (randomNum32() & ~"11"_bin32);
							rsgpStm << storeMode;
							rsgpStm.posGo(92 - 0x10 - 4 - 12) << randomNum32();
						}
						// modify other
						e.$sz = randomNum32();
						if (is384) {
							e.$ofs = randomNum32();
							e.$resStoreMethod = randomNum32() & "00"_bin32;
							e.$normalResDataOfs = randomNum32();
							e.$normalResDataSz = randomNum32();
							e.$atlasResDataOriSz = randomNum32();
							e.$atlasResSum += 45;
						}
						$StaticBlock::write(e, stm);
						stm.posBack(e.kSz).posGo(header.$subGroupInfoListSz);
					}
				}
			}
			{
				groupID.free();
				subGroupID.free();
				resPath.free();
				groupInfo.free();
				subGroupInfo.free();
				autoPoolInfo.free();
				atlasResInfo.free();
				atlasResExpandInfo.free();
				rsgpHeader.free();
				rsgpResInfo.free();
			}
			return;
		}

		Void disturbExport(Path const & srcFile, Path const & dstFile) {
			ByteSection pkg;
			$File::readFileForce(srcFile, pkg);
			disturb(pkg);
			$File::writeFile(dstFile, pkg);
			pkg.free();
			return;
		}

	}

	extern Void renameFileCaseByRSBInfoX(Path const & infoFile, Path const & tgtDir) {
		$Info::$RSBInfoX::Info info;
		info.read($JSON::read(infoFile, crv($JSON::Val())));
		auto fileSum = info.$group.range().eachSum([](auto & v) { return v.$v.$subGroup.range().eachSum([](auto & v) { return v.$v.$res.$sz; }, kSzN); }, kSzN) + kSz1;
		List<Path> dirList(fileSum), fileList(fileSum);
		for (auto const & group : info.$group) {
			for (auto const & subGroup : group.$v.$subGroup) {
				for (auto const & res : subGroup.$v.$res) {
					if (res.$v.$path.first() != "!program"_r) {
						fileList.append(res.$v.$path);
						if (res.$v.$resType == $Info::$RSBInfoX::ResType::kAtlas)
							fileList.last().setExtension("ptx"_r);
					}
				}
			}
		}
		fileList.append("properties/resources.rton"_path);
		for (auto const & file : fileList) {
			Path mid(file.$sz);
			for (auto i = kNo1; i < file.lastIdx(); ++i) {
				mid.append(file[i]);
				if (!dirList.range().exist(mid))
					dirList.append(mid);
			}
		}
		{
			Path parentDir; $File::getCurDirThenChgDir(parentDir, tgtDir);
			for (auto const & e : dirList) {
				Path dir;
				dir.ref(e, kNo1, e.$sz - kSz1);
				$File::rename(dir, e.fileName(), e.fileName());
			}
			for (auto const & e : fileList) {
				Path dir;
				dir.ref(e, kNo1, e.$sz - kSz1);
				$File::rename(dir, e.fileName(), e.fileName());
			}
			$File::chgDirThenFree(parentDir);
		}
		return;
	}

	extern Void mixWwiseSoundBankHIRC(Path const & srcAFile, Path const & srcBFile, Path const & dstFile) {
		$JSON::Arr srcA, srcB, dst;
		$JSON::read(srcAFile, srcA);
		$JSON::read(srcBFile, srcB);
		dst.alloc(srcA.$sz + srcB.$sz);
		List<Sz> idList(srcA.$sz + srcB.$sz);
		for (auto idx = kNo1; idx < srcA.$sz; ++idx) {
			if (!idList.range().exist(srcA[idx]["id"_r].getIntU())) {
				idList.append(srcA[idx]["id"_r].getIntU());
				dst.add(srcA[idx]);
			}
		}
		for (auto idx = kNo1; idx < srcB.$sz; ++idx) {
			if (!idList.range().exist(srcB[idx]["id"_r].getIntU())) {
				idList.append(srcB[idx]["id"_r].getIntU());
				dst.add(srcB[idx]);
			}
		}
		$JSON::write(dstFile, dst);
		return;
	}

	extern Void getFileNameNoExtensionFromDir(Path const & srcDir, Path const & dstFile) {
		List<Str> list;
		List<Path> fileList;
		$File::getPathListOfFile(srcDir, fileList, kBoT);
		list.alloc();
		for (auto & e : fileList) {
			e.eraseExtension();
			if (!list.range().exist(e.fileName()))
				list.append(e.fileName());
		}
		$JSON::write(dstFile, $JSON::setListToArr(list, crv($JSON::Val())));
		return;
	}

	extern Void getAllEventIDFromPopcapAnimDir(Path const & srcDir, Path const & dstFile) {
		List<Str> idList;
		List<Path> fileList;
		$File::getPathListOfFile(srcDir, fileList, kBoT);
		idList.alloc();
		for (auto const & file : fileList) {
			if (file.cmpExtensionIgnoreCase("pam"_r)) {
				$Anim::$PopcapAnim::$Struct::Struct pam;
				ByteSection dat;
				$File::readFileForce(file, dat);
				pam.read(ByteStream().ref(dat).setPosBeg());
				dat.free();
				for (auto const & frame : pam.$main.$frame) {
					if (frame.$cmd.$state) {
						for (auto const & cmd : frame.$cmd.$data) {
							if (($Str::cmpIgnoreCase(cmd.$cmd.$str, "PlaySample"_r) ||
								 $Str::cmpIgnoreCase(cmd.$cmd.$str, "play_sound"_r)) &&
								!idList.range().exist(cmd.$arg.$str)
								) {
								idList.append(cmd.$arg.$str);
							}
						}
					}
				}
			}
		}
		$JSON::write(dstFile, $JSON::setListToArr(idList, crv($JSON::Val())));
		return;
	}

	extern Void removeUnityAssetHeader(Path const & srcFile, Path const & dstFile) {
		ByteSection data;
		$File::readFileForce(srcFile, data);
		$File::writeFile(dstFile, ByteSection().ref(data, 8));
		return;
	}

}
