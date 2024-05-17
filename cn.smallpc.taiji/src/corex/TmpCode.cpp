# include "./Macro.h"

# if defined PLATFORM_WINDOWS

# include "./TmpCode.h"

# include "./Type.h"
# include "./SomethingElse.h"

# include "./MyType/Str.h"
# include "./MyType/StrList.h"
# include "./AnimUtil/PopAnimUtil/Converter.h"
# include "./AudioUtil/WEMEncoder/WEMEncoder.h"

//# include "./SpecialMod/PvZ-2/PvZ2JSON.h"
# include "./SpecialMod/PvZ-2/RSBPatch.h"
# include "NonStdFunc.h"

namespace $TJ {
	int cnvInteger(int const & src) {
		int block[] = {
			0xD, 0xC, 0xF, 0xE,
			0x9, 0x8, 0xB, 0xA,
			0x5, 0x4, 0x7, 0x6,
			0x1, 0x0, 0x3, 0x2
		};
		int result = 8192 * (src / 16 * 16 + block[src % 16]);
		return result;
	}
/*

	Void testobj() {
		$SpecialMod::$PvZ_2::$PvZ2JSON::_PvZ2JSON data;
		$JSON::Arr Fmt;
		$JSON::Obj Src;
		$JSON::read(Path("\\proto.json"), Fmt);
		$JSON::read(Path("\\src.json"), Src);
		data.read(Fmt, Src);
		wLogLn("a");
		$JSON::write(Path("\\dst.json"), data.write(Fmt, $JSON::Obj().setNull()));
		return;
	}*/
	Void DeCodeWEMToOGG(Path const & SrcFilePath, Path const & DstFilePath) {
		$AudioUtil::$WEMEncoder::WEMDeCodeToOGG(SrcFilePath, DstFilePath);
		return;
	}
#if false
#include "./ResPkgUtil/RSBUtil/Struct.h"
#include "./ResPkgUtil/RSBUtil/Packer.h"
# include "./ResPkgUtil/RSGPUtil/StructInfo.h"
# include "./ResPkgUtil/RSGPUtil/Struct.h"
# include "./ResPkgUtil/RSGPUtil/Packer.h"
# include "./ResPkgUtil/RSGPUtil/Packer_Export.h"

	void getkw(Path const & srcPath) {
		StrList strList;
		$JSONTool::ReadAllKeyStrFromVal($JSONTool::ReadVal(srcPath, _JSONVal().setNull()), strList);
		_JSONVal dstVal;
		dstVal.SetArray();
		strList.WriteArrAllStr(dstVal);
		strList.tfree();
		$JSONTool::WriteVal(Path(srcPath).setSuffix("key.json"), dstVal);
		return;
	}
	void wavt() {
		Path SrcPath, DstPath;
		getFilePathByWindow(SrcPath);
		$AudioUtil::RWave(SrcPath);
		return;
	}
#endif
	void testx() {
		wLogLn("start testx");
		//$NonStdFunc::XSplitRSBResInfoByPart(_Path("\\SPCWork\\~main.rsb\\Res\\properties"), _Path("\\SPCWork\\~main.rsb\\ResInfoPartInfoList.json"), _Path("\\SPCWork\\~main.rsb\\ResInfo.json"), 100 * $ByteSizeDef::kSzBinMega);
		//$NonStdFunc::XSplitRSBResInfoByPart(_Path("\\PvZResLib\\Test\\~main.rsb\\Res\\properties"), _Path("\\PvZResLib\\Test\\~main.rsb\\ResInfoPartInfoList.json"), _Path("\\PvZResLib\\Test\\~main.rsb\\ResInfo.json"), 100 * $ByteSizeDef::kSzBinMega);
		//$NonStdFunc::XCatRSBResInfoByPart(_Path("\\PvZResLib\\Test\\~main.rsb\\Res\\properties"), _Path("\\PvZResLib\\Test\\~main.rsb\\ResInfoPartIDList.json"), _Path("\\PvZResLib\\Test\\~main.rsb\\ResInfoPartInfoList.json"), _Path("\\PvZResLib\\Test\\~main.rsb\\ResInfo.json"));
		//$NonStdFunc::XCatRSBResInfoByPart(_Path("\\PvZResLib\\CN_RSB_2.4.5\\~main.rsb\\Res\\properties"), _Path("\\PvZResLib\\CN_RSB_2.4.5\\~main.rsb\\ResInfoPartIDList.json"), _Path("\\PvZResLib\\CN_RSB_2.4.5\\~main.rsb\\ResInfoPartInfoList.json"), _Path("\\PvZResLib\\CN_RSB_2.4.5\\~main.rsb\\ResInfo.json"));
		//$NonStdFunc::MixRSBResInfoToHD(_Path("\\PvZResLib\\Test\\~main.rsb\\ResInfo.json"), _Path("\\PvZResLib\\CN_RSB_2.4.5\\~main.rsb\\ResInfo.json"), _Path("\\PvZResLib\\Test\\~main.rsb\\ResInfo2.json"), 640, 1536);
		//$NonStdFunc::MixRSBStructInfoToHD(_Path("\\PvZResLib\\Test\\~main.rsb\\StructInfo.json"), _Path("\\PvZResLib\\CN_RSB_2.4.5\\~main.rsb\\StructInfo.json"), _Path("\\PvZResLib\\Test\\~main.rsb\\StructInfo2.json"), 640, 1536);
		//wavt();
		//getkw(_Path("\\~o\\Res\\__MANIFESTGROUP___CompositeShell\\__MANIFESTGROUP__\\~__MANIFESTGROUP__\\Res\\PROPERTIES\\~resources.rton"));
		wLogLn("finish testx");
		return;
	}
}
# endif
