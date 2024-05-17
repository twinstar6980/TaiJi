# include "./Executor.h"

# include "./Config.h"

# include "../Entry/Validate.h"

# include "../Type.h"
# include "../TypeOfTime.h"
# include "../Const.h"
# include "../MyType/Str.h"
# include "../MyType/Path.h"
# include "../UtilKit/DevUtil.h"

# include "../DataUtil/Compressor/Compressor_Export.h"
# include "../DataUtil/Encryptor/Encryptor_Export.h"
# include "../DataUtil/Hash/Hash.h"

# include "../InfoUtil/InfoUtil_Export.h"

# include "../RTONUtil/Encoder_Export.h"
# include "../RTONUtil/Encryptor_Export.h"

# include "../ResPkgUtil/RSGPUtil/Packer_Export.h"
# include "../ResPkgUtil/RSBUtil/Packer_Export.h"
# include "../ResPkgUtil/DZUtil/Packer_Export.h"
# include "../ResPkgUtil/PAKUtil/Packer_Export.h"

# include "../ImageUtil/PopTexEncoder/PopTexEncoder_Export.h"
# include "../ImageUtil/Spliter/Spliter_Export.h"

# include "../AudioUtil/SoundBankUtil/Packer_Export.h"

# include "../AnimUtil/PopAnimUtil/Encoder_Export.h"
# include "../AnimUtil/PopAnimUtil/Converter_Export.h"

//# include "../AudioUtil/WEMCnver/WEMCnver.h"

# include "../WorkLog/WorkLog.h"

# include "../SpecialMod/PvZ-2/LawnStrTextUtil.h"
# include "../SpecialMod/PvZ-2/Mod.h"
# include "../SpecialMod/SpecialMod_Export.h"

# include "../NonStdFunc.h"
# include "../UtilKit/JSONPatch.h"

# include <iostream>
# include <string>
# include <functional>

namespace $TJ::$Executor {

	static LogConfig gLogCfg;

	ListOfInfo<Script> & getScriptListFromJSON($JSON::Val const & src, ListOfInfo<Script> & dst) {
		dst.setNull();
		switch (src._type) {
			default: break;
			case $JSON::ValType::kObj:
				dst.add().getLast().read(src.getObj());
				break;
			case $JSON::ValType::kArr:
				dst.read(src.getArr());
				break;
		}
		return dst;
	}

	static auto & loadParmMap(FuncTmpl const & tmpl, VarSrcMapForSimple const & src, VarInstPkg & dst, VarInstPkg const & help) {
		for_criter(e, tmpl._parm) {
			auto & dstVal = dst.addByKey(e._key).getLastVal().init(e._val);
			if (help.existKey(Str(e._key).ins("="))) {
				dstVal.cpy(help.getValByKey(Str(e._key).ins("=")));
			} else {
				if (gLogCfg._varName)
					wCmt("! Var : %s", e._key._str);
				auto & srcVal = src.getValByKey(e._key);
				if (!srcVal._cmt.isTypeNull())
					wCmt("! Var-Cmt : %s", srcVal._cmt.makeInst<Str>(help).listGetLast<Str>()._str);
				makeVarInst(srcVal._val, dstVal, help);
			}
		}
		return dst;
	}

	enum class FuncInt : UI8 {

	};

	constexpr Ch sss[]("ss");

	static auto runScriptFuncCall(StrList const & func, VarSrcMapForSimple const & parmSrc, FuncTmplList const & funcList, VarInstPkg & varPkg) {
		auto clockStart = getCurClockInt();
		Str funcName; func.write(funcName, kThFirst, kSzNull, ':'); funcName.eraseFromEnd(kSzOne).ins(":");
		for_criter(funcTmpl, funcList) {
			Str thisFuncName; funcTmpl._func.write(thisFuncName, kThFirst, kSzNull, ':'); thisFuncName.eraseFromEnd(kSzOne).ins(":");
			if (funcName == thisFuncName) {
				VarInstPkg parm;
				loadParmMap(funcTmpl, parmSrc, parm, varPkg);
				if (gLogCfg._funcName)
					wCmt("Func : %s", funcName._str);
				if (funcName == ":Pause") {
					Pause();
				} else if (funcName == ":Delay") {
					auto sec = parm.getVar<UI64>("Second");
					wCmt("Delay Second %u", sec);
					Delay(sec);
				} else if (funcName == ":Setting:JSONIOBuf:ModSize") {
					$JSON::$IOBuf::modBufSz(
						parm.getVar<UI64>("Size")
					);
				} else if (funcName == ":Setting:JSONStyle:SetStyle") {
					$JSON::$StyleCtrl::setStyle(
						parm.getVar<Bool>("OutNewLineIfElement")
					);
				} else if (funcName == ":DevUtil:Cpy:File") {
					$DevUtil::CpyFile(
						parm.getVar<Path>("SrcPath"),
						parm.getVar<Path>("DstPath")
					);
				} else if (funcName == ":DevUtil:Cpy:Dir") {
					$DevUtil::CpyDir(
						parm.getVar<Path>("SrcPath"),
						parm.getVar<Path>("DstPath")
					);
				} else if (funcName == ":DevUtil:ReName:File") {
					$DevUtil::ReNameFile(
						parm.getVar<Path>("SrcPath"),
						parm.getVar<Path>("DstPath")
					);
				} else if (funcName == ":DevUtil:ReName:Dir") {
					$DevUtil::ReNameDir(
						parm.getVar<Path>("SrcPath"),
						parm.getVar<Path>("DstPath")
					);
				} else if (funcName == ":DevUtil:ReMov:File") {
					$DevUtil::ReMovFile(parm.getVar<Path>("Path"));
				} else if (funcName == ":DevUtil:ReMov:Dir") {
					$DevUtil::ReMovDir(parm.getVar<Path>("Path"));
				} else if (funcName == ":DataUtil:Compressor:ZLIB:Compress") {
					$DataUtil::$Compressor::$ZLIB::E_Compress(
						parm.getVar<Path>("RawFile"),
						parm.getVar<Path>("RipeFile"),
						parm.getVar<UI64>("Ofs"),
						parm.getVar<UI64>("Lv")
					);
				} else if (funcName == ":DataUtil:Compressor:ZLIB:UnCompress") {
					$DataUtil::$Compressor::$ZLIB::E_UnCompress(
						parm.getVar<Path>("RipeFile"),
						parm.getVar<Path>("RawFile"),
						parm.getVar<UI64>("Ofs"),
						parm.getVar<UI64>("BufRate")
					);
				} else if (funcName == ":DataUtil:Encryptor:XOR:Crypt") {
					$DataUtil::$Encryptor::$XOR::E_Crypt(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile"),
						static_cast<Byte>(parm.getVar<UI64>("Key"))
					);
				} else if (funcName == ":DataUtil:Encryptor:XOR:CryptByStrKey") {
					$DataUtil::$Encryptor::$XOR::E_CryptByStrKey(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile"),
						parm.getVar<Str>("Key")
					);
				} else if (funcName == ":DataUtil:Encryptor:Base64:EnCrypt") {
					$DataUtil::$Encryptor::$Base64::expEnCrypt(
						parm.getVar<Path>("PlainFile"),
						parm.getVar<Path>("CipherFile")
					);
				} else if (funcName == ":DataUtil:Encryptor:Base64:DeCrypt") {
					$DataUtil::$Encryptor::$Base64::expDeCrypt(
						parm.getVar<Path>("CipherFile"),
						parm.getVar<Path>("PlainFile")
					);
				} else if (funcName == ":DataUtil:Hash:MD5:Make") {
					//wLogLn("Hash : %s", $DataUtil::$Hash::$MD5::make(parm.GetVar<Str>("Data"), Str().getThis()));
				}
			#if defined VER_ULTIMATE
				else if (funcName == ":DataUtil:Encryptor:PvZ2_CHS_IRE:EnCrypt") {
					$DataUtil::$Encryptor::$PvZ2_CHS_IRE::$EnCrypt::expEnCrypt(
						parm.getVar<Path>("PlainFile"),
						parm.getVar<Path>("CipherFile"),
						parm.getVar<UI64>("DataBlockSum"),
						parm.getVar<UI64>("BufBlockSum"),
						parm.getVar<Str>("Key"),
						parm.getVar<Str>("IV")
					);
				} else if (funcName == ":DataUtil:Encryptor:PvZ2_CHS_IRE:DeCrypt") {
					$DataUtil::$Encryptor::$PvZ2_CHS_IRE::$DeCrypt::expDeCrypt(
						parm.getVar<Path>("CipherFile"),
						parm.getVar<Path>("PlainFile"),
						parm.getVar<UI64>("DataBlockSum"),
						parm.getVar<UI64>("BufBlockSum"),
						parm.getVar<Str>("Key"),
						parm.getVar<Str>("IV")
					);
				}
			#endif
				else if (funcName == ":JSONUtil:FmtFile") {
					$JSON::fmtFile(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":JSONPatch:Apply") {
					$JSON::$Patch::apply(
						parm.getVar<Path>("PatchFile"),
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":InfoUtil:Cnv") {
					$InfoUtil::$Cnv::expCnv(
						parm.getVar<Str>("SrcInfoType"),
						parm.getVar<Str>("DstInfoType"),
						parm.getVar<Path>("SrcInfoFile"),
						parm.getVar<Path>("DstInfoFile"),
						parm.getVar<Path>("AppendInfoFile")
					);
				} else if (funcName == ":InfoUtil:Cpy") {
					$InfoUtil::$Cpy::expCpy(
						parm.getVar<Str>("InfoType"),
						parm.getVar<Path>("SrcInfoFile"),
						parm.getVar<Path>("DstInfoFile")
					);
				} else if (funcName == ":InfoUtil:Split") {
					$InfoUtil::$Spliter::expSplit(
						parm.getVar<Str>("InfoType"),
						parm.getVar<Path>("InfoFile"),
						parm.getVar<Path>("MainDir")
					);
				} else if (funcName == ":InfoUtil:Cat") {
					$InfoUtil::$Spliter::expCat(
						parm.getVar<Str>("InfoType"),
						parm.getVar<Path>("InfoFile"),
						parm.getVar<Path>("MainDir")
					);
				} else if (funcName == ":InfoUtil:Lite") {
					$InfoUtil::$Lite::expLite(
						parm.getVar<Str>("InfoType"),
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile"),
						CnvListPToB(parm.getVarList<UI64>(StrRef("ResID")), ListB<SzI>().setNull())
					);
				} else if (funcName == ":RTONUtil:Encoder:EnCode") {
					$RTONUtil::$Encoder::$EnCode::expEnCodeRTON(
						parm.getVar<Path>("JSONFile"),
						parm.getVar<Path>("RTONFile"),
						parm.getVar<Bool>("UseCacheStr"),
						parm.getVar<Bool>("UseSpecialTypeForRTID"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":RTONUtil:Encoder:DeCode") {
					$RTONUtil::$Encoder::$DeCode::expDeCodeRTON(
						parm.getVar<Path>("RTONFile"),
						parm.getVar<Path>("JSONFile"),
						parm.getVar<UI64>("RTONDataOfs")
					);
				}
			#if defined VER_ULTIMATE
				else if (funcName == ":RTONUtil:Encryptor:EnCrypt") {
					$RTONUtil::$Encryptor::$EnCrypt::expEnCrypt(
						parm.getVar<Path>("PlainFile"),
						parm.getVar<Path>("CipherFile"),
						parm.getVar<Str>("Key")
					);
				} else if (funcName == ":RTONUtil:Encryptor:DeCrypt") {
					$RTONUtil::$Encryptor::$DeCrypt::expDeCrypt(
						parm.getVar<Path>("CipherFile"),
						parm.getVar<Path>("PlainFile"),
						parm.getVar<Str>("Key")
					);
				}
			#endif
				else if (funcName == ":ResPkgUtil:RSGPUtil:Packer:Pack_Literal") {
					$ResPkgUtil::$RSGPUtil::$Packer::$Pack::expPackLiteral(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("StructInfoFile"),
						parm.getVar<Path>("ResDir"),
						parm.getVar<Bool>("CipherTextUpperCase"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":ResPkgUtil:RSGPUtil:Packer:Pack") {
					$ResPkgUtil::$RSGPUtil::$Packer::$Pack::expPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<Bool>("CipherTextUpperCase"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":ResPkgUtil:RSGPUtil:Packer:UnPack") {
					$ResPkgUtil::$RSGPUtil::$Packer::$UnPack::expUnPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir")
					);
				} else if (funcName == ":ResPkgUtil:RSBUtil:Packer:Pack") {
					$ResPkgUtil::$RSBUtil::$Packer::$Pack::expPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<Bool>("PackFromGroup"),
						parm.getVar<Bool>("PackRSGPByMainInfo"),
						parm.getVar<Bool>("OutPutNewPacket"),
						parm.getVar<Bool>("UseReadyPacket"),
						parm.getVar<Bool>("CipherTextUpperCase"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":ResPkgUtil:RSBUtil:Packer:UnPack") {
					$ResPkgUtil::$RSBUtil::$Packer::$UnPack::expUnPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<Bool>("OutPut_Res"),
						parm.getVar<Bool>("OutPut_Packet"),
						parm.getVar<Bool>("OutPut_PacketInfo"),
						parm.getVar<Bool>("OutPut_Group"),
						parm.getVar<Bool>("OutPut_Group_Res"),
						parm.getVar<Bool>("OutPut_Group_Packet")
					);
				} else if (funcName == ":ResPkgUtil:RSBUtil:Packer:PackToPart") {
					$ResPkgUtil::$RSBUtil::$Packer::$Pack::expPackToPartDflt(
						parm.getVar<Path>("PartDir"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<Bool>("PackFromGroup"),
						parm.getVar<Bool>("PackRSGPByMainInfo"),
						parm.getVar<Bool>("OutPutNewPacket"),
						parm.getVar<Bool>("UseReadyPacket"),
						parm.getVar<Bool>("CipherTextUpperCase"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":ResPkgUtil:RSBUtil:Packer:UnPackFromPart") {
					$ResPkgUtil::$RSBUtil::$Packer::$UnPack::expUnPackFromPartDflt(
						parm.getVar<Path>("PartDir"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<Bool>("OutPut_Res"),
						parm.getVar<Bool>("OutPut_Packet"),
						parm.getVar<Bool>("OutPut_PacketInfo"),
						parm.getVar<Bool>("OutPut_Group"),
						parm.getVar<Bool>("OutPut_Group_Res"),
						parm.getVar<Bool>("OutPut_Group_Packet")
					);
				} else if (funcName == ":ResPkgUtil:DZUtil:Packer:Pack") {
					$ResPkgUtil::$DZUtil::$Packer::$Pack::expPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":ResPkgUtil:DZUtil:Packer:UnPack") {
					$ResPkgUtil::$DZUtil::$Packer::$UnPack::expUnPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir")
					);
				} else if (funcName == ":ResPkgUtil:PAKUtil:Packer:Pack") {
					$ResPkgUtil::$PAKUtil::$Packer::$Pack::expPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":ResPkgUtil:PAKUtil:Packer:UnPack") {
					$ResPkgUtil::$PAKUtil::$Packer::$UnPack::expUnPackDflt(
						parm.getVar<Path>("PkgFile"),
						parm.getVar<Path>("MainDir")
					);
				} else if (funcName == ":ImageUtil:Spliter:Cat") {
					$ImageUtil::$Spliter::$Cat::expCat(
						CnvListPToB(parm.getVarList<UI64>(StrRef("AtlasSize")), ListB<SzI>().setNull()),
						parm.getVar<Path>("ImageInfoListFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":ImageUtil:Spliter:CatByAtlasInfo") {
					$ImageUtil::$Spliter::$Cat::E_CatByAtlasInfo(
						parm.getVar<Path>("AtlasInfoFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstDir"),
						parm.getVar<Bool>("UseSingleDir")
					);
				} else if (funcName == ":ImageUtil:Spliter:CatByAtlasInfoList") {
					$ImageUtil::$Spliter::$Cat::E_CatByAtlasInfoList(
						parm.getVar<Path>("AtlasInfoListFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstDir"),
						parm.getVar<Bool>("UseSingleDir")
					);
				} else if (funcName == ":ImageUtil:Spliter:Split") {
					$ImageUtil::$Spliter::$Split::expSplit(
						parm.getVar<Path>("ImageInfoListFile"),
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstDir")
					);
				} else if (funcName == ":ImageUtil:Spliter:SplitByAtlasInfo") {
					$ImageUtil::$Spliter::$Split::E_SplitByAtlasInfo(
						parm.getVar<Path>("AtlasInfoFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstDir"),
						parm.getVar<Bool>("UseSingleDir")
					);
				} else if (funcName == ":ImageUtil:Spliter:SplitByAtlasInfoList") {
					$ImageUtil::$Spliter::$Split::E_SplitByAtlasInfoList(
						parm.getVar<Path>("AtlasInfoListFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstDir"),
						parm.getVar<Bool>("UseSingleDir")
					);
				} else if (funcName == ":ImageUtil:PopTexEncoder:EnCode") {
					$ImageUtil::$PopTexEncoder::$EnCode::E_EnCode(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile"),
						CnvListPToB(parm.getVarList<UI64>(StrRef("Size")), ListB<SzI>().setNull()),
						parm.getVar<Str>("TexFmt")
					);
				} else if (funcName == ":ImageUtil:PopTexEncoder:DeCode") {
					$ImageUtil::$PopTexEncoder::$DeCode::E_DeCode(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile"),
						CnvListPToB(parm.getVarList<UI64>(StrRef("Size")), ListB<SzI>().setNull()),
						parm.getVar<Str>("TexFmt")
					);
				} else if (funcName == ":ImageUtil:PopTexEncoder:EnCodeAtlasList") {
					$ImageUtil::$PopTexEncoder::$EnCode::E_EnCodeAtlasList(
						parm.getVar<Path>("AtlasInfoListFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstDir")
					);
				} else if (funcName == ":ImageUtil:PopTexEncoder:DeCodeAtlasList") {
					$ImageUtil::$PopTexEncoder::$DeCode::E_DeCodeAtlasList(
						parm.getVar<Path>("AtlasInfoListFile"),
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstDir")
					);
				} else if (funcName == ":AudioUtil:SoundBankUtil:Packer:Pack") {
					$AudioUtil::$SoundBankUtil::$Packer::$Pack::expPackDflt(
						parm.getVar<Path>("SoundBankFile"),
						parm.getVar<Path>("MainDir"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":AudioUtil:SoundBankUtil:Packer:UnPack") {
					$AudioUtil::$SoundBankUtil::$Packer::$UnPack::expUnPackDflt(
						parm.getVar<Path>("SoundBankFile"),
						parm.getVar<Path>("MainDir")
					);
				} else if (funcName == ":AudioUtil:SoundBankUtil:NameMapperMod:Load") {
					$AudioUtil::$SoundBankUtil::$NameMapperMod::E_Load(
						parm.getVar<Path>("NameMapperFile")
					);
				} else if (funcName == ":AudioUtil:SoundBankUtil:NameMapperMod:Clear") {
					$AudioUtil::$SoundBankUtil::$NameMapperMod::E_Clear();
				} else if (funcName == ":AudioUtil:SoundBankUtil:CalcID32") {
					wMsg("ID : %u", $AudioUtil::$SoundBankUtil::calcID32(
						parm.getVar<Str>("Name")
					));
				}
			#if defined VER_ULTIMATE
				else if (funcName == ":AnimUtil:PopAnimUtil:Encoder:EnCode") {
					$AnimUtil::$PopAnimUtil::$Encoder::$EnCode::E_EnCode_Dflt(
						parm.getVar<Path>("JSONFile"),
						parm.getVar<Path>("PopAnimFile"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Encoder:DeCode") {
					$AnimUtil::$PopAnimUtil::$Encoder::$DeCode::E_DeCode_Dflt(
						parm.getVar<Path>("PopAnimFile"),
						parm.getVar<Path>("JSONFile")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Converter:CnvToXFL") {
					$AnimUtil::$PopAnimUtil::$Converter::$CnvToXFL::expCnv(
						parm.getVar<Path>("JSONFile"),
						parm.getVar<Path>("XFLDir"),
						parm.getVar<UI64>("ImageSize")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Converter:CnvFromXFL") {
					$AnimUtil::$PopAnimUtil::$Converter::$CnvFromXFL::expCnv(
						parm.getVar<Path>("XFLDir"),
						parm.getVar<Path>("JSONFile"),
						parm.getVar<UI64>("ImageSize")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Converter:XFLScaleCnv") {
					$AnimUtil::$PopAnimUtil::$Converter::$XFLScaleCnv::E_cnv(
						parm.getVar<Path>("XFLDir"),
						parm.getVar<UI64>("SrcSize"),
						parm.getVar<UI64>("DstSize")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Converter:CnvToRipe") {
					$AnimUtil::$PopAnimUtil::$Converter::$CnvToRipe::expCnv(
						parm.getVar<Path>("JSONFile"),
						parm.getVar<Path>("RipeJSONFile")
					);
				}
			#endif
				else if (funcName == ":AnimUtil:PopAnimUtil:Converter:CnvToXFL_OneStep") {
					$AnimUtil::$PopAnimUtil::$Converter::$CnvToXFL::E_Cnv_OneStep(
						parm.getVar<Path>("PopAnimFile"),
						parm.getVar<Path>("XFLDir"),
						parm.getVar<UI64>("ImageSize")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Converter:CnvFromXFL_OneStep") {
					$AnimUtil::$PopAnimUtil::$Converter::$CnvFromXFL::E_Cnv_OneStep(
						parm.getVar<Path>("XFLDir"),
						parm.getVar<Path>("PopAnimFile"),
						parm.getVar<UI64>("ImageSize"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":AnimUtil:PopAnimUtil:Converter:CnvToRipe_OneStep") {
					$AnimUtil::$PopAnimUtil::$Converter::$CnvToRipe::E_Cnv_OneStep(
						parm.getVar<Path>("PopAnimFile"),
						parm.getVar<Path>("RipeJSONFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:LawnStrTextUtil:JSONToText") {
					$SpecialMod::$PvZ_2::$LawnStrTextUtil::CnvJSONToText_ByFile(
						parm.getVar<Path>("JSONFile"),
						parm.getVar<Path>("TextFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:LawnStrTextUtil:TextToJSON") {
					$SpecialMod::$PvZ_2::$LawnStrTextUtil::CnvTextToJSON_ByFile(
						parm.getVar<Path>("TextFile"),
						parm.getVar<Path>("JSONFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:RSBPatch:PatchResInfo") {
					$SpecialMod::$PvZ_2::$RSBPatch::patchResInfo(
						parm.getVar<Path>("PatchFile"),
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:RSBPatch:PatchStructInfo") {
					$SpecialMod::$PvZ_2::$RSBPatch::patchStructInfo(
						parm.getVar<Path>("PatchFile"),
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:RSBPatch:PatchRes") {
					$SpecialMod::$PvZ_2::$RSBPatch::patchRes(
						parm.getVar<Path>("PatchFile"),
						parm.getVar<Path>("StructInfoFile"),
						parm.getVar<Path>("SrcResDir"),
						parm.getVar<Path>("DstGroupDir")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:Mod:Apply") {
					$SpecialMod::$PvZ_2::$Mod::expApply(
						parm.getVar<Path>("ModDir"),
						parm.getVar<Path>("EnvFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:Mod:EnvIntro") {
					$SpecialMod::$PvZ_2::$Mod::expIntro(
						parm.getVar<Path>("EnvFile")
					);
				} else if (funcName == ":SpecialMod:PvZ-2:Mod:EnvOutro") {
					$SpecialMod::$PvZ_2::$Mod::expOutro(
						parm.getVar<Path>("EnvFile")
					);
				} else if (funcName == ":NonStdFunc:GetFileNameNoPathNoSuffixFromDir") {
					$NonStdFunc::getFileNameNoPathNoSuffixFromDir(
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":NonStdFunc:GetEventIDFromPAMDir") {
					$NonStdFunc::getEventIDFromPAMDir(
						parm.getVar<Path>("SrcDir"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":NonStdFunc:BatchCpyLevelFile") {
					$NonStdFunc::BatchCpyLevelFile(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstDir"),
						parm.getVar<Str>("Prefix"),
						parm.getVar<Str>("Suffix"),
						parm.getVar<UI64>("Sum")
					);
				} else if (funcName == ":NonStdFunc:ReNameFileCase") {
					$NonStdFunc::ReNameFileCase(
						parm.getVar<Path>("OriListDataFile"),
						parm.getVar<Path>("TargetDir")
					);
				} else if (funcName == ":NonStdFunc:MixBNKSubList") {
					$NonStdFunc::MixBNKSubList(
						parm.getVar<Path>("SrcAFile"),
						parm.getVar<Path>("SrcBFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":NonStdFunc:CompressRawRSBSMF") {
					$NonStdFunc::CompressRawRSBSMF(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":NonStdFunc:UnCompressRawRSBSMF") {
					$NonStdFunc::UnCompressRawRSBSMF(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":NonStdFunc:CnvWEM") {
					$NonStdFunc::CnvWEM(
						parm.getVar<Path>("SrcFile"),
						parm.getVar<Path>("DstFile")
					);
				} else if (funcName == ":NonStdFunc:XCatRSBResInfoByPart") {
					$NonStdFunc::XCatRSBResInfoByPart(
						parm.getVar<Path>("RawResInfoPartDir"),
						parm.getVar<Path>("PartIDListFile"),
						parm.getVar<Path>("PartInfoListFile"),
						parm.getVar<Path>("ResInfoFile")
					);
				} else if (funcName == ":NonStdFunc:XSplitRSBResInfoByPart") {
					$NonStdFunc::XSplitRSBResInfoByPart(
						parm.getVar<Path>("RawResInfoPartDir"),
						parm.getVar<Path>("PartInfoListFile"),
						parm.getVar<Path>("ResInfoFile"),
						parm.getVar<UI64>("MemorySize")
					);
				} else if (funcName == ":NonStdFunc:MixRSBResInfoToHD") {
					$NonStdFunc::MixRSBResInfoToHD(
						parm.getVar<Path>("BaseInfoFile"),
						parm.getVar<Path>("HDInfoFile"),
						parm.getVar<Path>("DstInfoFile"),
						parm.getVar<UI64>("LowResID"),
						parm.getVar<UI64>("HighResID")
					);
				} else if (funcName == ":NonStdFunc:MixRSBStructInfoToHD") {
					$NonStdFunc::MixRSBStructInfoToHD(
						parm.getVar<Path>("BaseInfoFile"),
						parm.getVar<Path>("HDInfoFile"),
						parm.getVar<Path>("DstInfoFile"),
						parm.getVar<UI64>("LowResID"),
						parm.getVar<UI64>("HighResID")
					);
				} else wErr("UnKnown FuncName \"%s\"", funcName._str);
				funcName.tfree();
				parm.tfree();
				break;
			}
			thisFuncName.tfree();
		}
		auto clockFinish = getCurClockInt();
		if (gLogCfg._funcTimer)
			wCmt("Func-Timer : %.4lf s", (static_cast<F64>(clockFinish - clockStart)) / static_cast<F64>(CLOCKS_PER_SEC));
		return;
	}

	static Void runScriptList(ListP<Script> const & list, FuncTmplList const & funcList, VarInstPkg & varPkg);

	static Void runScript(Script const & script, FuncTmplList const & funcList, VarInstPkg & varPkg) {
		auto clockStart = getCurClockInt();
		Path parentDir; $DevUtil::getCurWorkDir(parentDir);
		VarInst tmpInst;
		if (script._enable.makeInst<Bool>(varPkg).listGet<Bool>()) {
			if (!script._cmt.isTypeNull())
				wCmt("Script-Cmt : %s", script._cmt.makeInst<Str>(varPkg).listGet<Str>()._str);
			if (!script._workSpace.isTypeNull())
				$DevUtil::chgDir(script._workSpace.makeInst<Path>(varPkg).listGet<Path>());
			script._reVarIntro.modTo(varPkg);
			while_nonstop{
				if (!script._loopCond._onlyOnce) {
					auto begin = script._loopCond._begin.makeInst<UI64>(varPkg).listGet<UI64>();
					auto end = script._loopCond._end.makeInst<UI64>(varPkg).listGet<UI64>();
					if (gLogCfg._scriptLoopProg)
						wCmt("Script-Loop : %u / %u", begin + kThSecond, end);
					if (begin == end)
						break;
				}
				script._localVar.loadTo(varPkg);
				script._reVarBefore.modTo(varPkg);
				switch (script._type) {
					default: break;
					case ScriptType::kIdle: break;
					case ScriptType::kLink:
						runScript(Script().read($JSON::read(script._linkScript.makeInst<Path>(varPkg).listGet<Path>(), $JSON::Obj().setNull())), funcList, varPkg);
						break;
					case ScriptType::kList:
						runScriptList(script._listScript, funcList, varPkg);
						break;
					case ScriptType::kCall:
						runScriptFuncCall(script._callFunc, script._callParm, funcList, varPkg);
						break;
				}
				script._reVarAfter.modTo(varPkg);
				script._localVar.eraseTo(varPkg);
				if (script._loopCond._onlyOnce)
					break;
			}
			script._reVarOutro.modTo(varPkg);
		}
		$DevUtil::chgDirThenFree(parentDir);
		auto clockFinish = getCurClockInt();
		if (gLogCfg._scriptTimer)
			wCmt("Script-Timer : %.4lf s", (static_cast<F64>(clockFinish - clockStart)) / static_cast<F64>(CLOCKS_PER_SEC));
		return;
	}

	static Void runScriptList(ListP<Script> const & list, FuncTmplList const & funcList, VarInstPkg & varPkg) {
		for_cnt(i, kThFirst, list._sz) {
			if (gLogCfg._scriptListProg)
				wCmt("Script-List : %u / %u", i + kThSecond, list._sz);
			runScript(list[i], funcList, varPkg);
		}
		return;
	}

	static auto & getQuickEnterScript(QuickEnterConfig const & cfg, Path const & enterPath, ListOfInfo<Script> & dst, Bool const & isBatchMode, ListP<Path> & childPathList) {
		Map<Str, QEOptionGroup> validOption;
		ListP<ListP<Path>> validChildPathListList;
		for_criter(optionGroup, isBatchMode ? cfg._optionForBatch : cfg._option) {
			for_criter(option, optionGroup._val) {
				auto thisIsValid = cfg._ignoreFilter;
				if (!thisIsValid) {
					if (isBatchMode) {
						ListP<Path> curChildPathList; curChildPathList.alloc(childPathList._sz);
						for_criter(e, childPathList) {
							if (option._val.isValidPath(e, cfg._ignoreSuffixCase))
								curChildPathList.add(e);
						}
						if (thisIsValid = (curChildPathList._sz > kSzZero)) {
							validChildPathListList.add(curChildPathList);
						}
					} else {
						thisIsValid = option._val.isValidPath(enterPath, cfg._ignoreSuffixCase);
					}
				}
				if (thisIsValid) {
					if (!validOption.existKey(optionGroup._key))
						validOption.addByKey(optionGroup._key);
					validOption.getValByKey(optionGroup._key).addByKV(option._key, option._val);
				}
			}
		}
		if (validOption._sz == kSzZero) {
			wMsg("Can Not Found Option-Script For this");
			dst.setNull();
		} else {
			ListB<ListOfInfo<Script> const *> optonSctiptList;
			auto optionIdx = kThFirst;
			for_criter(optionGroup, validOption) {
				wCmt("%s", Str().cat('-', 20).cat(' ', 4).cat(optionGroup._key).cat(' ', 4).cat('-', 30 - optionGroup._key._len)._str);
				for_criter(option, optionGroup._val) {
					wMsg("  %.2u : %s", optionIdx, option._key._str);
					if (!option._val._tip.isNull()) {
						for_criter(e, option._val._tip)
							wCmt("  \t%.8s\t: %s", e._key._str, e._val._str);
					}
					optonSctiptList.add(&option._val._script);
					if (isBatchMode) {
						wCmt("\tValid   : %u / %u", validChildPathListList[optionIdx]._sz, childPathList._sz);
					}
					++optionIdx;
				}
			}
			auto selectOptionIdx = kThNull;
			while_nonstop{
				wLog("> InPut Option Number : ");
				std::cin >> selectOptionIdx;
				if (selectOptionIdx < optionIdx)
					break;
				wCmt("Error Number , Please Re-InPut");
			}
			dst.cpy(*optonSctiptList[selectOptionIdx]);
			if (isBatchMode)
				childPathList = validChildPathListList[selectOptionIdx];
		}
		return dst;
	}

	// $ by string
	// # by file
	// < QE-selectFile
	// > QE-inputPath
	static auto exec(Config const & cfg, StrList const & arg) {
		Path workSpace(kStorageDir);
		gLogCfg.cpy(cfg._logConfig);
		VarInstPkg globalVar;
		globalVar.addByKey(StrRef("!STORAGE_DIR")).getLastVal().initT<Path>().getList<Path>().add(kStorageDir);
		cfg._globalVar.loadTo(globalVar);
		{
			VarInstPkg initVarPkg(globalVar);
			runScriptList(cfg._introScript, cfg._func, initVarPkg);
		}
		for_cnt(idx, kThFirst, arg._sz) {
			Path parentDir; $DevUtil::getCurWorkDirThenChgDir(parentDir, workSpace);
			Str actualArgStr(arg[idx], kThSecond);
			ListOfInfo<Script> script;
			VarInstPkg initVarPkg(globalVar);
			initVarPkg.addByKey(StrRef("!WORK_SPACE")).getLastVal().initT<Path>().getList<Path>().add(workSpace);
			wCmt("exec %u : %s\n", idx + kThSecond, arg[idx]._str);
			switch (arg[idx][kThFirst]) {
				default:
					wErr("ERROR ARG");
					break;
				case '@':
					workSpace.read(actualArgStr);
					$DevUtil::chgDirThenFree(parentDir);
					continue;
					break;
				case '$':
					getScriptListFromJSON($JSON::read(actualArgStr, $JSON::Val().setNull()), script);
					runScriptList(script, cfg._func, initVarPkg);
					break;
				case '#':
					getScriptListFromJSON($JSON::read(Path(actualArgStr), $JSON::Val().setNull()), script);
					initVarPkg.addByKey(StrRef("!ENTER_PATH")).getLastVal().initT<Path>().getList<Path>().add(Path(actualArgStr));
					runScriptList(script, cfg._func, initVarPkg);
					break;
				case '>':
				# if defined PLATFORM_WINDOWS
					wLog("$ select file : ");
					getPathStrByWindows(actualArgStr, kStorageDir);
					wLogLn("%s", actualArgStr._str);
				# endif
				case '<': {
					ListP<Path> childFileList;
					auto isBatchMode = kFalse;
					Path actualPath(actualArgStr);
					if (actualPath.cmpSuffix("script"))
						getScriptListFromJSON($JSON::read(actualPath, $JSON::Val().setNull()), script);
					else {
						if (cfg._quickEnterConfig._enableBatchMode && $DevUtil::isDir(actualPath)) {
							if (!(isBatchMode = cfg._quickEnterConfig._forceBatchIfDir)) {
								wMsg("Is Batch Mode ?  ");
								std::cin >> isBatchMode;
							}
						}
						if (isBatchMode)
							$DevUtil::getPathList(actualPath, childFileList, kFalse);
						getQuickEnterScript(cfg._quickEnterConfig, actualPath, script, isBatchMode, childFileList);
						if (script.isNull()) {
							wMsg("this script is null, so pass it");
							continue;
						}
					}
					if (!isBatchMode) {
						initVarPkg.addByKey(StrRef("!ENTER_PATH")).getLastVal().initT<Path>().getList<Path>().add(Path(actualArgStr));
						runScriptList(script, cfg._func, initVarPkg);
					} else {
						Path actualPathRev; actualPathRev.cpy(actualPath).setPrefix("~");
						initVarPkg.addByKey(StrRef("!ENTER_PATH_IN")).getLastVal().initT<Path>().getList<Path>().add();
						initVarPkg.addByKey(StrRef("!ENTER_PATH_OUT")).getLastVal().initT<Path>().getList<Path>().add();
						for_criter(file, childFileList) {
							initVarPkg.getVar<Path>("!ENTER_PATH_IN").cpy(actualPath).add(file);
							initVarPkg.getVar<Path>("!ENTER_PATH_OUT").cpy(actualPathRev).add(file);
							wCmt("batch-item : %s", file.getFullPath()._str);
							runScriptList(script, cfg._func, initVarPkg);
						}
					}
					break;
				}
			}
			initVarPkg.tfree();
			$DevUtil::chgDirThenFree(parentDir);
		}
		{
			VarInstPkg initVarPkg(globalVar);
			runScriptList(cfg._outroScript, cfg._func, initVarPkg);
		}
		wLogLn("");
		return;
	}

	Void exec(Str const & cfgSrc, StrList const & arg) {
		Config cfg;
		switch (cfgSrc[kThFirst]) {
			default: wErr("ERROR CONFIG-ARG"); break;
			case '$': cfg.read($JSON::read(StrRef(cfgSrc, kThSecond), $JSON::Obj().setNull())); break;
			case '#': cfg.read($JSON::read(Path(StrRef(cfgSrc, kThSecond)), $JSON::Obj().setNull())); break;
		}
		return exec(cfg, arg);
	}

}
