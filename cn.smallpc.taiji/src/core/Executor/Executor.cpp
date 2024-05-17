#include "./Executor.hpp"

#include "./Config.hpp"
#include "../Entry/Validate.hpp"

#include "../Type.hpp"
#include "../Const.hpp"
#include "../Type/Str.hpp"
#include "../Type/Path.hpp"

#include "../PlatformSupport.hpp"

#include "../Util/File/File.hpp"
#include "../Util/Data/Compressor/ZLib.hpp"
#include "../Util/Data/Encryptor/XOR.hpp"
#include "../Util/Data/Encryptor/Base64.hpp"
#include "../Util/Data/Encryptor/Rijndael.hpp"
#include "../Util/RTON/Encoder.hpp"
#include "../Util/RTON/Encryptor.hpp"
#include "../Util/ResPkg/DZ/Packer.hpp"
#include "../Util/ResPkg/PAK/Packer.hpp"
#include "../Util/ResPkg/RSB/RSGP/Packer.hpp"
#include "../Util/ResPkg/RSB/Packer.hpp"
#include "../Util/Info/Info.hpp"
#include "../Util/Image/Spliter/Spliter.hpp"
#include "../Util/Image/Encoder/Encoder.hpp"
#include "../Util/Anim/PopcapAnim/Encoder.hpp"
#include "../Util/Anim/PopcapAnim/Converter.hpp"
#include "../Util/Aduio/WwiseEncodedMedia/Encoder.hpp"
#include "../Util/Aduio/WwiseSoundBank/Packer.hpp"
#include "../Util/Expand/Expand.hpp"
#include "../Util/Expand/JSONPatch.hpp"
#include "../Util/Expand/RSBPatchX.hpp"
#include "../Util/Expand/PvZ2Mod.hpp"

#include "../Util/Other/Other.hpp"

namespace $TJ::$Executor {

	static Map<Str, $Util::$Time::Clock> gClock;

	inline auto cnvIntListToImgSz(List<VTInt> const & t) {
		return ImgSz(t[kNo1], t[kNo2]);
	}

	InfoList<Script> & getScriptListFromJSON($JSON::Val const & src, InfoList<Script> & dst) {
		switch (src.$$ty()) {
			default:
				dst.setNull();
				break;
			case $JSON::ValType::kObj:
				dst.alloc(kSz1).appendX().last().read(src);
				break;
			case $JSON::ValType::kArr:
				dst.read(src);
				break;
		}
		return dst;
	}

	static auto & loadParmMap(FuncTmpl const & tmpl, VarSrcMap const & src, VarInstPkg & dst, VarInstPkg const & help) {
		for (auto const & e : tmpl.$parm) {
			auto & dstVal = dst.appendX().last().setKey(e.$k).$v.init(e.$v);
			if (help.existKey("="_r + e.$k)) {
				dstVal.cpy(help.getByKey("="_r + e.$k).$v);
			} else {
				auto & srcVal = src.getByKey(e.$k).$v;
				if (!srcVal.$msg.isNull())
					log.msg("%s", srcVal.$msg.makeInst<VTStr>(help).listLast<VTStr>().$data);
				makeVarInst(srcVal.$v, dstVal, help);
			}
		}
		return dst;
	}

	static auto runScriptFuncCall(List<Str> const & func, VarSrcMap const & parmSrc, List<FuncTmpl> const & funcList, VarInstPkg & varPkg) {
		Str funcName;
		func.concat<List<Str>::ConcatOption::kPrefix>(funcName, ':');
		for (auto const & funcTmpl : funcList) {
			if (funcName == funcTmpl.$func.concat<List<Str>::ConcatOption::kPrefix>(crv(Str()), ':')) {
				VarInstPkg parm; parm.alloc();
				loadParmMap(funcTmpl, parmSrc, parm, varPkg);
				if (false)
					log.msg("Func : %s", funcName.$data);
				switch (funcName.hashByPtr(operator""_sh)) {
					default:
						throw VAMGException("Executor", "function name error : %s", funcName.$data);
						break;
					case ":Setting:JSON:IOMem:realloc"_sh:
						$JSON::$IOMem::realloc(
							parm.getVar<VTInt>("Sz"_r)
						);
						break;
					case ":Setting:JSON:Style:set"_sh:
						$JSON::$Style::set(
							parm.getVar<VTBool>("NoLineFeedWhenElement"_r)
						);
						break;
					case ":CFunc:system"_sh:
						std::system(parm.getVar<VTStr>("Cmd"_r).$data);
						break;
					case ":Process:delay"_sh:
						$Util::$Process::delay(parm.getVar<VTInt>("Second"_r));
						break;
					case ":Process:pause"_sh:
						$Util::$Process::pause();
						break;
					case ":Process:exit"_sh:
						$Util::$Process::exit();
						break;
					case ":Clock:new"_sh:
						gClock(parm.getVar<VTStr>("ID"_r)).init();
						break;
					case ":Clock:delete"_sh:
						gClock.eraseByKey(parm.getVar<VTStr>("ID"_r));
						break;
					case ":Clock:start"_sh:
						gClock[parm.getVar<VTStr>("ID"_r)].start();
						break;
					case ":Clock:stop"_sh:
						gClock[parm.getVar<VTStr>("ID"_r)].stop();
						break;
					case ":Clock:show"_sh:
						log.txtln(("clock["_r + parm.getVar<VTStr>("ID"_r) + "] : "_r + parm.getVar<VTStr>("Msg"_r)).$data, gClock[parm.getVar<VTStr>("ID"_r)].durationSec());
						break;
					case ":File:cpy"_sh:
						$Util::$File::cpy(
							parm.getVar<VTPath>("Src"_r),
							parm.getVar<VTPath>("Dst"_r)
						);
						break;
					case ":File:rename"_sh:
						$Util::$File::rename(
							parm.getVar<VTPath>("Dir"_r),
							parm.getVar<VTStr>("OldName"_r),
							parm.getVar<VTStr>("NewName"_r)
						);
						break;
					case ":File:remove"_sh:
						$Util::$File::remove(
							parm.getVar<VTPath>("Path"_r)
						);
						break;
					case ":Data:Compressor:ZLib:Compress"_sh:
						$Util::$Data::$Compressor::$ZLib::$Compress::compressExport(
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":Data:Compressor:ZLib:UnCompress"_sh:
						$Util::$Data::$Compressor::$ZLib::$UnCompress::uncompressExport(
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r),
							parm.getVar<VTInt>("DstCap"_r)
						);
						break;
					case ":Data:Encryptor:XOR:Crypt"_sh:
						$Util::$Data::$Encryptor::$XOR::cryptExport(
							parm.getVar<VTPath>("PlainFile"_r),
							parm.getVar<VTPath>("CipherFile"_r),
							static_cast<Byte>(parm.getVar<VTInt>("Key"_r))
						);
						break;
					case ":Data:Encryptor:Base64:EnCrypt"_sh:
						$Util::$Data::$Encryptor::$Base64::$EnCrypt::encExport(
							parm.getVar<VTPath>("PlainFile"_r),
							parm.getVar<VTPath>("CipherFile"_r)
						);
						break;
					case ":Data:Encryptor:Base64:DeCrypt"_sh:
						$Util::$Data::$Encryptor::$Base64::$DeCrypt::decExport(
							parm.getVar<VTPath>("CipherFile"_r),
							parm.getVar<VTPath>("PlainFile"_r)
						);
						break;
					#if defined ENABLE_RTON_CRYPTOR
					case ":Data:Encryptor:Rijndael:EnCrypt"_sh:
						$Util::$Data::$Encryptor::$Rijndael::$EnCrypt::encExport(
							parm.getVar<VTPath>("PlainFile"_r),
							parm.getVar<VTPath>("CipherFile"_r),
							parm.getVar<VTInt>("BlockSz"_r),
							parm.getVar<VTInt>("KeySz"_r),
							parm.getVar<VTStr>("Key"_r),
							parm.getVar<VTStr>("IV"_r)
						);
						break;
					case ":Data:Encryptor:Rijndael:DeCrypt"_sh:
						$Util::$Data::$Encryptor::$Rijndael::$DeCrypt::decExport(
							parm.getVar<VTPath>("CipherFile"_r),
							parm.getVar<VTPath>("PlainFile"_r),
							parm.getVar<VTInt>("BlockSz"_r),
							parm.getVar<VTInt>("KeySz"_r),
							parm.getVar<VTStr>("Key"_r),
							parm.getVar<VTStr>("IV"_r)
						);
						break;
					#endif
					case ":JSON:Fmt"_sh:
						$JSON::fmtFile(
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":RTON:Encoder:EnCode"_sh:
						$Util::$RTON::$Encoder::$EnCode::encExport(
							parm.getVar<VTPath>("JSONFile"_r),
							parm.getVar<VTPath>("RTONFile"_r),
							parm.getVar<VTBool>("EnableStrIdx"_r),
							parm.getVar<VTBool>("EnableRTIDType"_r),
							parm.getVar<VTBool>("IsWhole"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":RTON:Encoder:DeCode"_sh:
						$Util::$RTON::$Encoder::$DeCode::decExport(
							parm.getVar<VTPath>("RTONFile"_r),
							parm.getVar<VTPath>("JSONFile"_r),
							parm.getVar<VTBool>("IsWhole"_r)
						);
						break;
					#if defined ENABLE_RTON_CRYPTOR
					case ":RTON:Encryptor:EnCrypt"_sh:
						$Util::$RTON::$Encryptor::$EnCrypt::encExport(
							parm.getVar<VTPath>("PlainFile"_r),
							parm.getVar<VTPath>("CipherFile"_r),
							parm.getVar<VTStr>("Key"_r)
						);
						break;
					case ":RTON:Encryptor:DeCrypt"_sh:
						$Util::$RTON::$Encryptor::$DeCrypt::decExport(
							parm.getVar<VTPath>("CipherFile"_r),
							parm.getVar<VTPath>("PlainFile"_r),
							parm.getVar<VTStr>("Key"_r)
						);
						break;
					#endif
					case ":ResPkg:DZ:Packer:Pack"_sh:
						$Util::$ResPkg::$DZ::$Packer::$Pack::packExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ResDir"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":ResPkg:DZ:Packer:UnPack"_sh:
						$Util::$ResPkg::$DZ::$Packer::$UnPack::unpackExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ResDir"_r)
						);
						break;
					case ":ResPkg:PAK:Packer:Pack"_sh:
						$Util::$ResPkg::$PAK::$Packer::$Pack::packExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ResDir"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":ResPkg:PAK:Packer:UnPack"_sh:
						$Util::$ResPkg::$PAK::$Packer::$UnPack::unpackExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ResDir"_r)
						);
						break;
					case ":ResPkg:RSB:RSGP:Packer:Pack"_sh:
						$Util::$ResPkg::$RSB::$RSGP::$Packer::$Pack::packExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ResDir"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":ResPkg:RSB:RSGP:Packer:UnPack"_sh:
						$Util::$ResPkg::$RSB::$RSGP::$Packer::$UnPack::unpackExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ResDir"_r)
						);
						break;
					case ":ResPkg:RSB:Packer:Pack"_sh:
						$Util::$ResPkg::$RSB::$Packer::$Pack::packExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("EmbeddedResInfoFile"_r),
							parm.getVar<VTPath>("PacketDir"_r),
							parm.getVar<VTPath>("ResDir"_r),
							parm.getVar<VTPath>("GroupDir"_r),
							parm.getVar<VTPath>("GroupPacketFile"_r),
							parm.getVar<VTPath>("GroupResDir"_r),
							parm.getVar<VTBool>("PackFromGroup"_r),
							parm.getVar<VTBool>("ExportNewPacket"_r),
							parm.getVar<VTBool>("UseReadyPacket"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":ResPkg:RSB:Packer:UnPack"_sh:
						$Util::$ResPkg::$RSB::$Packer::$UnPack::unpackExport(
							parm.getVar<VTPath>("PkgFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("EmbeddedResInfoFile"_r),
							parm.getVar<VTPath>("PacketDir"_r),
							parm.getVar<VTPath>("PacketInfoDir"_r),
							parm.getVar<VTPath>("ResDir"_r),
							parm.getVar<VTPath>("GroupDir"_r),
							parm.getVar<VTPath>("GroupPacketFile"_r),
							parm.getVar<VTPath>("GroupPacketInfoFile"_r),
							parm.getVar<VTPath>("GroupResDir"_r)
						);
						break;
					case ":Info:Purify"_sh:
						$Util::$Info::$Purify::purifyExport(
							parm.getVar<VTStr>("Type"_r),
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":Info:Spliter:Split"_sh:
						$Util::$Info::$Spliter::splitExport(
							parm.getVar<VTStr>("Type"_r),
							parm.getVar<VTStr>("Until"_r),
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstDir"_r)
						);
						break;
					case ":Info:Spliter:Cat"_sh:
						$Util::$Info::$Spliter::catExport(
							parm.getVar<VTStr>("Type"_r),
							parm.getVar<VTStr>("Until"_r),
							parm.getVar<VTPath>("SrcDir"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":Info:Cnv"_sh:
						$Util::$Info::$Cnv::cnvExport(
							parm.getVar<VTStr>("SrcType"_r),
							parm.getVar<VTStr>("DstType"_r),
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r),
							parm.getVar<VTPath>("ExtraFile"_r)
						);
						break;
					case ":Image:Spliter:Split"_sh:
						$Util::$Image::$Spliter::$Split::splitExport(
							cnvIntListToImgSz(parm.getVarList<IU64>("Sz"_r)),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("AtlasFile"_r),
							parm.getVar<VTPath>("ImgDir"_r)
						);
						break;
					case ":Image:Spliter:SplitByAtlasInfo"_sh:
						$Util::$Image::$Spliter::$Split::splitByAtlasInfoExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("AtlasDir"_r),
							parm.getVar<VTPath>("ImgDir"_r),
							parm.getVar<VTBool>("UseSingleDir"_r)
						);
						break;
					case ":Image:Spliter:Cat"_sh:
						$Util::$Image::$Spliter::$Cat::catExport(
							cnvIntListToImgSz(parm.getVarList<IU64>("Sz"_r)),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ImgDir"_r),
							parm.getVar<VTPath>("AtlasFile"_r)
						);
						break;
					case ":Image:Spliter:CatByAtlasInfo"_sh:
						$Util::$Image::$Spliter::$Cat::catByAtlasInfoExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ImgDir"_r),
							parm.getVar<VTPath>("AtlasDir"_r),
							parm.getVar<VTBool>("UseSingleDir"_r)
						);
						break;
					case ":Image:Encoder:EnCode"_sh:
						$Util::$Image::$Encoder::$EnCode::encExport(
							parm.getVar<VTPath>("ImgFile"_r),
							parm.getVar<VTPath>("DataFile"_r),
							cnvIntListToImgSz(parm.getVarList<IU64>("Sz"_r)),
							parm.getVar<VTStr>("Fmt"_r)
						);
						break;
					case ":Image:Encoder:EnCodeByAtlasInfo"_sh:
						$Util::$Image::$Encoder::$EnCode::encByAtlasInfoExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("ImgDir"_r),
							parm.getVar<VTPath>("DataDir"_r),
							parm.getVar<VTStr>("DataFileExtension"_r)
						);
						break;
					case ":Image:Encoder:DeCode"_sh:
						$Util::$Image::$Encoder::$DeCode::decExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("ImgFile"_r),
							cnvIntListToImgSz(parm.getVarList<IU64>("Sz"_r)),
							parm.getVar<VTStr>("Fmt"_r)
						);
						break;
					case ":Image:Encoder:DeCodeByAtlasInfo"_sh:
						$Util::$Image::$Encoder::$DeCode::decByAtlasInfoExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("DataDir"_r),
							parm.getVar<VTPath>("ImgDir"_r),
							parm.getVar<VTStr>("DataFileExtension"_r)
						);
						break;
					#if defined ENABLE_PAM_ENCODER_PLAIN
					case ":Anim:PopcapAnim:Encoder:EnCode"_sh:
						$Util::$Anim::$PopcapAnim::$Encoder::$EnCode::encExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":Anim:PopcapAnim:Encoder:DeCode"_sh:
						$Util::$Anim::$PopcapAnim::$Encoder::$DeCode::decExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("InfoFile"_r)
						);
						break;
					case ":Anim:PopcapAnim:Converter:ToXFL"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$ToXFL::cnvExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("XFLDir"_r),
							parm.getVar<VTInt>("ImgSz"_r)
						);
						break;
					case ":Anim:PopcapAnim:Converter:FromXFL"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$FromXFL::cnvExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("XFLDir"_r),
							parm.getVar<VTInt>("ImgSz"_r)
						);
						break;
					#endif
					case ":Anim:PopcapAnim:Converter:XFLScaleCnv"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$XFLScaleCnv::cnvExport(
							parm.getVar<VTPath>("XFLDir"_r),
							parm.getVar<VTInt>("SrcSz"_r),
							parm.getVar<VTInt>("DstSz"_r)
						);
						break;
					#if defined ENABLE_PAM_ENCODER_PLAIN
					case ":Anim:PopcapAnim:Converter:ToCSSInfo"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$ToCSSInfo::cnvExport(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("CSSInfoFile"_r)
						);
						break;
					#endif
					case ":Anim:PopcapAnim:Converter:ToXFL_OneStep"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$ToXFL::cnvOneStepExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("XFLDir"_r),
							parm.getVar<VTInt>("ImgSz"_r)
						);
						break;
					case ":Anim:PopcapAnim:Converter:FromXFL_OneStep"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$FromXFL::cnvOneStepExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("XFLDir"_r),
							parm.getVar<VTInt>("ImgSz"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":Anim:PopcapAnim:Converter:ToCSSInfo_OneStep"_sh:
						$Util::$Anim::$PopcapAnim::$Converter::$ToCSSInfo::cnvOneStepExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("CSSInfoFile"_r)
						);
						break;
					case ":Aduio:WwiseEncodedMedia:Encoder:EnCode"_sh:/*
						$Util::$Audio::$WwiseEncodedMedia::$Encoder::$EnCode::encExport(
							parm.getVar<VTPath>("AudioFile"_r),
							parm.getVar<VTPath>("EncodedFile"_r),
							parm.getVar<VTStr>("Option"_r)
						);*/
						break;
					case ":Aduio:WwiseEncodedMedia:Encoder:DeCode"_sh:
						$Util::$Audio::$WwiseEncodedMedia::$Encoder::$DeCode::decExport(
							parm.getVar<VTPath>("EncodedFile"_r),
							parm.getVar<VTPath>("AudioFile"_r)
						);
						break;
					case ":Aduio:WwiseSoundBank:Packer:Pack"_sh:
						$Util::$Audio::$WwiseSoundBank::$Packer::$Pack::packExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("MainDir"_r),
							parm.getVar<VTInt>("MemSz"_r)
						);
						break;
					case ":Aduio:WwiseSoundBank:Packer:UnPack"_sh:
						$Util::$Audio::$WwiseSoundBank::$Packer::$UnPack::unpackExport(
							parm.getVar<VTPath>("DataFile"_r),
							parm.getVar<VTPath>("MainDir"_r)
						);
						break;
					case ":Expand:JSONPatch:Apply"_sh:
						$Util::$Expand::$JSONPatch::$Apply::applyExport(
							parm.getVar<VTPath>("PatchFile"_r),
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":Expand:JSONPatch:Diff"_sh:
						$Util::$Expand::$JSONPatch::$Diff::diffExport(
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r),
							parm.getVar<VTPath>("PatchFile"_r)
						);
						break;
					case ":Expand:RSBPatchX:Apply"_sh:
						$Util::$Expand::$RSBPatchX::$Apply::applyExport(
							parm.getVar<VTPath>("PatchFile"_r),
							parm.getVar<VTPath>("SrcInfoXFile"_r),
							parm.getVar<VTPath>("SrcInfoFile"_r),
							parm.getVar<VTPath>("DstInfoXFile"_r),
							parm.getVar<VTPath>("DstInfoFile"_r),
							parm.getVar<VTBool>("ApplyToResFile"_r),
							parm.getVar<VTPath>("SrcResDir"_r),
							parm.getVar<VTPath>("DstPacketDir"_r),
							parm.getVar<VTPath>("DstResDir"_r)
						);
						break;
					case ":Expand:PvZ2Mod:Intro"_sh:
						$Util::$Expand::$PvZ2Mod::introExport(
							parm.getVar<VTPath>("EnvFile"_r)
						);
						break;
					case ":Expand:PvZ2Mod:Outro"_sh:
						$Util::$Expand::$PvZ2Mod::outroExport(
							parm.getVar<VTPath>("EnvFile"_r)
						);
						break;
					case ":Expand:PvZ2Mod:Apply"_sh:
						$Util::$Expand::$PvZ2Mod::applyExport(
							parm.getVar<VTPath>("EnvFile"_r),
							parm.getVar<VTPath>("ModDir"_r)
						);
						break;
					case ":Expand:PvZ2LawnStringText:UpGrade"_sh:
						$Util::$Expand::$PvZ2LawnStringText::upgradeExport(
							parm.getVar<VTPath>("OldStyleFile"_r),
							parm.getVar<VTPath>("NewStyleFile"_r)
						);
						break;
					case ":Expand:PvZ2LawnStringText:DownGrade"_sh:
						$Util::$Expand::$PvZ2LawnStringText::downgradeExport(
							parm.getVar<VTPath>("NewStyleFile"_r),
							parm.getVar<VTPath>("OldStyleFile"_r)
						);
						break;
					case ":Expand:RSBSMF:Compress"_sh:
						$Util::$Expand::$RSBSMF::compressExport(
							parm.getVar<VTPath>("RawFile"_r),
							parm.getVar<VTPath>("RipeFile"_r)
						);
						break;
					case ":Expand:RSBSMF:UnCompress"_sh:
						$Util::$Expand::$RSBSMF::uncompressExport(
							parm.getVar<VTPath>("RipeFile"_r),
							parm.getVar<VTPath>("RawFile"_r)
						);
						break;
					case ":Expand:rsbDataDisturb"_sh:
						$Util::$Expand::$RSBDataDisturb::disturbExport(
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":Expand:renameFileCaseByRSBInfoX"_sh:
						$Util::$Expand::renameFileCaseByRSBInfoX(
							parm.getVar<VTPath>("InfoFile"_r),
							parm.getVar<VTPath>("TgtDir"_r)
						);
						break;
					case ":Expand:mixWwiseSoundBankHIRC"_sh:
						$Util::$Expand::mixWwiseSoundBankHIRC(
							parm.getVar<VTPath>("SrcAFile"_r),
							parm.getVar<VTPath>("SrcBFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ":Expand:removeUnityAssetHeader"_sh:
						$Util::$Expand::removeUnityAssetHeader(
							parm.getVar<VTPath>("SrcFile"_r),
							parm.getVar<VTPath>("DstFile"_r)
						);
						break;
					case ""_sh:
						throw VAMGException("Executor", "null function");
						break;
				}
				funcName.free();
				parm.free();
				break;
			}
		}
		return;
	}

	static Void runScriptList(List<Script> const & list, List<FuncTmpl> const & funcList, VarInstPkg & varPkg);

	static Void runScript(Script const & script, List<FuncTmpl> const & funcList, VarInstPkg & varPkg) {
		Path parentDir; $Util::$File::getCurDir(parentDir);
		if (script.$enable.makeInst<VTBool>(varPkg).listGet<VTBool>()) {
			if (!script.$msg.isNull())
				log.cmt("%s", script.$msg.makeInst<VTStr>(varPkg).listGet<VTStr>().$data);
			if (!script.$workSpace.isNull())
				$Util::$File::chgDir(script.$workSpace.makeInst<VTPath>(varPkg).listGet<VTPath>());
			script.$reVarIntro.modTo(varPkg);
			while (kBoT) {
				if (!script.$loopCond.$onlyOnce) {
					auto beg = script.$loopCond.$beg.makeInst<VTInt>(varPkg).listGet<VTInt>();
					auto end = script.$loopCond.$end.makeInst<VTInt>(varPkg).listGet<VTInt>();
					if (script.$showLoopProg)
						log.cmt("script-loop : %u / %u", beg + kNo2, end);
					if (beg == end)
						break;
				}
				script.$var.loadTo(varPkg);
				script.$reVarBefore.modTo(varPkg);
				switch (script.$ty) {
					default:
						break;
					case ScriptType::kIdle:
						break;
					case ScriptType::kLink:
						runScript(Script().read($JSON::read(script.$linkScript.makeInst<VTPath>(varPkg).listGet<VTPath>(), crv($JSON::Obj()))), funcList, varPkg);
						break;
					case ScriptType::kList:
						runScriptList(script.$listScript, funcList, varPkg);
						break;
					case ScriptType::kCall:
						runScriptFuncCall(script.$callFunc, script.$callParm, funcList, varPkg);
						break;
				}
				script.$reVarAfter.modTo(varPkg);
				script.$var.eraseTo(varPkg);
				if (script.$loopCond.$onlyOnce)
					break;
			}
			script.$reVarOutro.modTo(varPkg);
		}
		$Util::$File::chgDirThenFree(parentDir);
		return;
	}

	static Void runScriptList(List<Script> const & list, List<FuncTmpl> const & funcList, VarInstPkg & varPkg) {
		for (auto i = kNo1; i < list.$sz; ++i) {
			if (false)
				log.cmt("Script-List : %u / %u", i + kNo2, list.$sz);
			runScript(list[i], funcList, varPkg);
		}
		return;
	}

	static auto getBatchChild(Path const & dir, Sz const & depth, List<Path> & dst) {
		if (depth == kSzN) {
			$Util::$File::getPathListOfFile(dir, dst, kBoF);
		} else {
			List<Path> tmp;
			$Util::$File::getPathList(dir, tmp, kBoF);
			dst.alloc(tmp.$sz);
			for (auto const & e : tmp) {
				if (e.$sz == depth)
					dst.append(e);
			}
		}
		return;
	}
	static auto selectPath(QEOption const & opt, Bool const & ignoreFilter, Path const & path) {
		return ignoreFilter || opt.isValidPath(path);
	}
	static auto selectPathForBatch(QEOption const & opt, Bool const & ignoreFilter, Path const & path, List<Path> const & childPath, List<Path> & validChildPath) {
		if (ignoreFilter) {
			validChildPath = childPath;
		} else {
			validChildPath.alloc(childPath.$sz);
			for (auto const & e : childPath) {
				if (opt.isValidPath(path + e))
					validChildPath.append(e);
			}
		}
		return !validChildPath.isEmpty();
	}

	static QEOption const * getQuickEnterScript(Path const & path, QEOptionGroupMap const & optGroupMap, Bool const & ignoreFilter, Bool const & isBatchMode, Sz const & batchDepth, List<Path> const & childPath) {
		Map<Str, List<MapE<Str, QEOption> const *>> validOpt;
		List<Sz> validChildPathCnt;
		validOpt.alloc();
		validChildPathCnt.alloc();
		for (auto const & optGroup : optGroupMap) {
			for (auto const & opt : optGroup.$v) {
				Bool thisIsValid;
				if (isBatchMode) {
					List<Path> tmp;
					if (thisIsValid = selectPathForBatch(opt.$v, ignoreFilter, path, childPath, tmp))
						validChildPathCnt.append(tmp.$sz);
				} else {
					thisIsValid = selectPath(opt.$v, ignoreFilter, path);
				}
				if (thisIsValid) {
					if (!validOpt.existKey(optGroup.$k))
						validOpt.appendX().last().setKey(optGroup.$k).$v.alloc();
					validOpt.getByKey(optGroup.$k).$v.append(&opt);
				}
			}
		}
		if (validOpt.isEmpty()) {
			log.war("can not found option for this");
			return kNullPtr;
		}
		List<QEOption const *> validOptList;
		validOptList.alloc();
		for (auto const & optGroup : validOpt) {
			log.cmt("%s", Str().alloc().append('-', 30 - 4 - (optGroup.$k.$sz) / 2).append(' ', 4).append(optGroup.$k).append(' ', 4).append('-', 30 - 4 - (optGroup.$k.$sz + 1) / 2).$data);
			for (auto const & opt : optGroup.$v) {
				validOptList.append(&(*opt).$v);
				log.msg("  %.2u : %s", validOptList.lastIdx(), (*opt).$k.$data);
				if (!((*opt).$v).$tip.isNull()) {
					for (auto const & e : (*opt).$v.$tip)
						log.cmt("  \t%.8s\t: %s", e.$k.$data, e.$v.$data);
				}
				if (isBatchMode) {
					log.cmt("\tvalid   : %u / %u", validChildPathCnt[validOptList.lastIdx()], childPath.$sz);
				}
			}
		}
		log.cmt("%s", Str().alloc().append('-', 60).$data);
		auto i = kNoX;
		while (kBoT) {
			i = getUserInput<Sz>("input option number"_r);
			if (i < validOptList.$sz)
				break;
			log.cmt("error number, please re-input");
		}
		return validOptList[i];
	}

	// $ by string
	// # by file
	// > QE-select
	// < QE-path
	static auto exec(Path const & initDir, Path const & cfgFile, Config const & cfg, List<Str> const & arg) {
		auto workSpace(initDir);
		gClock.alloc();
		VarInstPkg globalVar;
		globalVar.alloc();
		globalVar("!INIT_DIR"_r).initT<VTPath>().getList<VTPath>().alloc(kSz1).append(initDir);
		globalVar("!CFG_FILE"_r).initT<VTPath>().getList<VTPath>().alloc(kSz1).append(cfgFile);
		globalVar("!CFG_DIR"_r).initT<VTPath>().getList<VTPath>().alloc(kSz1).append(cfgFile.dir());
		cfg.$var.loadTo(globalVar);
		{
			runScriptList(cfg.$intro, cfg.$func, crv(VarInstPkg(globalVar)));
		}
		for (auto i = kNo1; i < arg.$sz; ++i) {
			if (arg[i].isEmpty())
				continue;
			Path parentDir; $Util::$File::getCurDirThenChgDir(parentDir, workSpace);
			Str argDataStr(Str().ref(arg[i], kNo2));
			InfoList<Script> script;
			VarInstPkg initVarPkg(globalVar);
			initVarPkg("!WORK_SPACE"_r).initT<VTPath>().getList<VTPath>().alloc(kSz1).append(workSpace);
			log.cmt("exec %u : %s\n", i + kNo2, arg[i].$data);
			switch (arg[i].first()) {
				default:
					throw VAMGException("Executor", "error argument type");
					break;
				case '@':
					workSpace.parse(argDataStr);
					$Util::$File::chgDirThenFree(parentDir);
					continue;
					break;
				case '$':
					getScriptListFromJSON($JSON::read(argDataStr, crv($JSON::Val())), script);
					runScriptList(script, cfg.$func, initVarPkg);
					break;
				case '#':
					getScriptListFromJSON($JSON::read(Path(argDataStr), crv($JSON::Val())), script);
					initVarPkg("!ENTER_PATH"_r).initT<VTPath>().getList<VTPath>().alloc(kSz1).appendX().last().parse(argDataStr);
					runScriptList(script, cfg.$func, initVarPkg);
					break;
				case '>':
					argDataStr.prepend(getUserInput<Path>("select QE-Path"_r).fullName());
					log.cmt("%s", argDataStr.$data);
					[[fallthrough]];
				case '<': {
					Path qePath;
					List<Path> qePathChild;
					Bool isBatchMode = kBoF;
					Sz batchDepth;
					if (Sz pos; argDataStr.range().find('?', pos)) {
						qePath.parse(Str().ref(argDataStr, kNo1, pos));
						if (!$Util::$File::existPath(qePath))
							throw VAMGException("Executor", "can not found QE-Path : %s", qePath.fullName().$data);
						Str optStr(Str().ref(argDataStr, pos + kNo2));
						QEOptionGroupMap const * scriptMap;
						if (optStr.first() == '%') {
							if (!$Util::$File::existDir(qePath))
								throw VAMGException("Executor", "QE-Path should a dir when batch-mode : %s", qePath.fullName().$data);
							optStr.eraseOne(kNo1);
							scriptMap = &cfg.$quickEnter.$optionForBatch;
							isBatchMode = kBoT;
							if (cfg.$quickEnter.$batchDepth.$state) {
								batchDepth = cfg.$quickEnter.$batchDepth.$data;
							} else {
								batchDepth = getUserInput<Sz>("batch-depth"_r);
							}
							getBatchChild(qePath, batchDepth, qePathChild);
						} else {
							scriptMap = &cfg.$quickEnter.$option;
						}
						List<Str> optID;
						$Str::split(optStr, optID, ';');
						if (!(*scriptMap).existKey(optID[kNo1]) || !(*scriptMap)[optID[kNo1]].existKey(optID[kNo2])) {
							log.war("can not found option by string : %s", optStr.$data);
							continue;
						}
						auto const & opt = (*scriptMap)[optID[kNo1]][optID[kNo2]];
						script = opt.$script;
						if (isBatchMode) {
							List<Path> tmp;
							selectPathForBatch(opt, cfg.$quickEnter.$ignoreFilter, qePath, qePathChild, tmp);
							qePathChild.free().moveFrom(tmp);
						} else {
							if (!cfg.$quickEnter.$ignoreFilter && !opt.isValidPath(qePath)) {
								log.war("filter.isValidPath(QE_PATH) == false");
								if (!getUserInput<Bool>("already use this option"_r))
									continue;
							}
						}
					} else {
						qePath.parse(argDataStr);
						if (!$Util::$File::existPath(qePath))
							throw VAMGException("Executor", "can not found QE-Path : %s", qePath.fullName().$data);
						if (qePath.cmpExtensionIgnoreCase("tjs"_r)) {
							getScriptListFromJSON($JSON::read(qePath, crv($JSON::Val())), script);
						} else {
							if ($Util::$File::existDir(qePath)) {
								if (cfg.$quickEnter.$enableBatchMode.$state) {
									isBatchMode = cfg.$quickEnter.$enableBatchMode.$data;
								} else {
									isBatchMode = getUserInput<Bool>("is batch-mode"_r);
								}
								if (isBatchMode) {
									if (cfg.$quickEnter.$batchDepth.$state) {
										batchDepth = cfg.$quickEnter.$batchDepth.$data;
									} else {
										batchDepth = getUserInput<Sz>("batch-depth"_r);
									}
									getBatchChild(qePath, batchDepth, qePathChild);
								}
							}
							auto qeOpt = getQuickEnterScript(qePath, isBatchMode ? cfg.$quickEnter.$optionForBatch : cfg.$quickEnter.$option, cfg.$quickEnter.$ignoreFilter, isBatchMode, batchDepth, qePathChild);
							if (isBatchMode) {
								List<Path> tmp;
								selectPathForBatch(*qeOpt, cfg.$quickEnter.$ignoreFilter, qePath, qePathChild, tmp);
								qePathChild.free().moveFrom(tmp);
							}
							if (qeOpt == kNullPtr) {
								script.setNull();
							} else {
								script = (*qeOpt).$script;
							}
						}
					}
					if (script.isNull()) {
						log.war("this script is null, so pass it");
						continue;
					}
					initVarPkg("!QE_PATH"_r).initT<VTPath>().getList<VTPath>().alloc(kSz1).appendX().last() = qePath;
					if (!isBatchMode) {
						runScriptList(script, cfg.$func, initVarPkg);
					} else {
						initVarPkg("!QE_PATH_CHILD"_r).initT<VTPath>().listAlloc(kSz1).listAppendX();
						for (auto const & e : qePathChild) {
							initVarPkg["!QE_PATH_CHILD"_r].listGet<VTPath>() = e;
							log.cmt("batch-item : %s", e.fullName().$data);
							runScriptList(script, cfg.$func, initVarPkg);
						}
					}
					break;
				}
			}
			initVarPkg.free();
			$Util::$File::chgDirThenFree(parentDir);
		}
		{
			runScriptList(cfg.$outro, cfg.$func, crv(VarInstPkg(globalVar)));
		}
		globalVar.free();
		gClock.free();
		log.txtln("");
		return;
	}

	Void exec(Path const & initDir, Str const & cfgSrc, List<Str> const & arg) {
		Path cfgFile;
		Config cfg;
		switch (cfgSrc.first()) {
			default:
				throw VAMGException("Executor", "error config argument type");
				break;
			case '$':
				cfgFile.setNull();
				cfg.read($JSON::read(Str().ref(cfgSrc, kNo2), crv($JSON::Val())));
				break;
			case '#':
				cfgFile.parse(Str().ref(cfgSrc, kNo2));
				cfg.read($JSON::read(cfgFile, crv($JSON::Val())));
				break;
		}
		return exec(initDir, cfgFile, cfg, arg);
	}

}
