#include "./Config.hpp"

#include "../Type.hpp"
#include "../Const.hpp"
#include "../Type/List.hpp"
#include "../Type/Map.hpp"
#include "../Type/Str.hpp"
#include "../Type/Path.hpp"
#include "../Type/JSON.hpp"
#include "../Type/InfoType.hpp"
#include "../UserLiteral.hpp"

#include "../Util/File/File.hpp"

#include "../Util/Image/Type.hpp"
#include "../Util/Image/ImgFile.hpp"

#include "../PlatformSupport.hpp"

namespace $TJ::$Executor {

	static Void analysisCmdStrListOnce(List<Str> const & strList, Sz & strIdx, VarInst & inst, VarInstPkg const & varPkg) {
		VarInst tmpVar;
		auto & sub = strList[strIdx++];
		if (sub.isEmpty())
			throw VAMGException("Executor:VarReader", "empty cmd-type");
		switch (sub.first()) {
			default:
				throw VAMGException("Executor:VarReader", "unknown cmd-type");
				break;
			case '$': {
				auto & id = strList[strIdx++];
				if (sub.$sz == kSz1) {
					inst.cpy(varPkg.getByKey(id).$v);
				} else {
					if (sub[kNo2] == ']') {
						inst.listLast<VTInt>() = varPkg.getByKey(id).$v.getListSz();
					} else if (sub[kNo2] == '[') {
						VarInst elementIdx; elementIdx.initInt().listAlloc(kSz1).listAppendX();
						switch (sub[kNo3]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, elementIdx, varPkg);
								break;
							case '.':
								$Str::scanFmt(strList[strIdx++], "%llu"_r.$data, &elementIdx.listGet<VTInt>());
								break;
						}
						switch (inst.$ty.$base) {
							default:
								break;
							case VarBaseType::kBool:
								inst.listLast<VTBool>() = varPkg.getVar<VTBool>(id, elementIdx.listGet<VTInt>());
								break;
							case VarBaseType::kInt:
								inst.listLast<VTInt>() = varPkg.getVar<VTInt>(id, elementIdx.listGet<VTInt>());
								break;
							case VarBaseType::kStr:
								inst.listLast<VTStr>() = varPkg.getVar<VTStr>(id, elementIdx.listGet<VTInt>());
								break;
							case VarBaseType::kPath:
								inst.listLast<VTPath>() = varPkg.getVar<VTPath>(id, elementIdx.listGet<VTInt>());
								break;
						}
					}
				}
				break;
			}
			case '#': {
				auto & cmd = strList[strIdx++];
				switch (cmd.hashByPtr(operator""_sh)) {
					default:
						break;
					case "Bool_Inverse"_sh:
						inst.listLast<VTBool>() = !inst.listLast<VTBool>();
						break;
					case "Bool_And"_sh:
						tmpVar.initBool().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								$Str::scanFmt(strList[strIdx++], "%hhu"_r.$data, &tmpVar.listLast<VTBool>());
								break;
						}
						inst.listLast<VTBool>() &= tmpVar.listLast<VTBool>();
						break;
					case "Bool_Or"_sh:
						tmpVar.initBool().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								$Str::scanFmt(strList[strIdx++], "%hhu"_r.$data, &tmpVar.listLast<VTBool>());
								break;
						}
						inst.listLast<VTBool>() |= tmpVar.listLast<VTBool>();
						break;
					case "Int_Add"_sh:
						tmpVar.initInt().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								$Str::scanFmt(strList[strIdx++], "%llu"_r.$data, &tmpVar.listLast<VTInt>());
								break;
						}
						inst.listLast<VTInt>() += tmpVar.listLast<VTInt>();
						break;
					case "Int_Sub"_sh:
						tmpVar.initInt().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								$Str::scanFmt(strList[strIdx++], "%llu"_r.$data, &tmpVar.listLast<VTInt>());
								break;
						}
						inst.listLast<VTInt>() -= tmpVar.listLast<VTInt>();
						break;
					case "Str_Prepend"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTStr>().prepend(tmpVar.listLast<VTStr>());
						break;
					case "Str_Append"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTStr>().append(tmpVar.listLast<VTStr>());
						break;
					case "Str_EraseSub"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTStr>().eraseSub(tmpVar.listLast<VTStr>());
						break;
					case "Path_AppendLevelByPath"_sh:
						tmpVar.initPath().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTPath>().parse(strList[strIdx++]);
								break;
						}
						inst.listLast<VTPath>().append(tmpVar.listLast<VTPath>());
						break;
					case "Path_AppendLevel"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTPath>().append(tmpVar.listLast<VTStr>());
						break;
					case "Path_EraseLastLevel"_sh:
						inst.listLast<VTPath>().eraseFromEnd();
						break;
					case "Path_SetPrefix"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTPath>().setPrefix(tmpVar.listLast<VTStr>());
						break;
					case "Path_SetSuffix"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTPath>().setSuffix(tmpVar.listLast<VTStr>());
						break;
					case "Path_SetExtension"_sh:
						tmpVar.initStr().listAlloc(kSz1).listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						inst.listLast<VTPath>().setExtension(tmpVar.listLast<VTStr>());
						break;
					case "Path_EraseExtension"_sh:
						inst.listLast<VTPath>().eraseExtension();
						break;
					case "PathList_Filter_Extension"_sh: {
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						List<VTPath> tmp;
						$Path::filterByExtension(inst.getList<VTPath>(), tmp, tmpVar.listLast<VTStr>());
						inst.getList<VTPath>().free().moveFrom(tmp);
						log.cmt("PathList_Filter_Extension : %u Of %s", inst.getListSz(), tmpVar.listLast<VTStr>().$data);
						break;
					}
					case "PathList_Filter_SubStr"_sh: {
						tmpVar.initStr().listAlloc(kSz1).listAppendX();
						switch (sub[kNo2]) {
							default:
								throw VAMGException("Executor:VarReader", "error when analysis cmd-str-list");
								break;
							case '*':
								analysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg);
								break;
							case '.':
								tmpVar.listLast<VTStr>() = strList[strIdx++];
								break;
						}
						List<VTPath> tmp;
						$Path::filterBySubStr(inst.getList<VTPath>(), tmp, tmpVar.listLast<VTStr>());
						inst.getList<VTPath>().free().moveFrom(tmp);
						log.cmt("PathList_Filter_SubStr : %u Of %s", inst.getListSz(), tmpVar.listLast<VTStr>().$data);
						break;
					}
				}
				break;
			}
			case '!': {
				auto & inputType = strList[strIdx++];
				auto & valStr = strList[strIdx++];
				//log.txt("# %s : ", inputType.$data);
				switch (inputType.hashByPtr(operator""_sh)) {
					default:
						throw VAMGException("Executor:VarReader", "unknown type");
						break;
					case "Bool"_sh:
						if (valStr == "false"_r)
							inst.listLast<VTBool>() = kBoF;
						else if (valStr == "true"_r)
							inst.listLast<VTBool>() = kBoT;
						else
							throw VAMGException("Executor:VarReader", "bool string error");
						//log.txtln("%s", valStr.$data);
						break;
					case "Int"_sh:
						$Str::scanFmt(valStr, "%llu"_r.$data, &inst.listLast<VTInt>());
						//log.txtln("%s", valStr.$data);
						break;
					case "Int_Hex"_sh:
						$Str::scanFmt(valStr, "%X"_r.$data, &inst.listLast<VTInt>());
						//log.txtln("%s", valStr.$data);
						break;
					case "ByteSize_Str"_sh:
						inst.listLast<VTInt>() = $Str::cnvByteSzStr(valStr);
						//log.txtln("%s", valStr.$data);
						break;
					case "IntList_ImageSize"_sh: {
						VarInst imgFile; imgFile.initPath().listAlloc(kSz1).listAppendX();
						analysisCmdStrListOnce(strList, --strIdx, imgFile, varPkg);
						auto imgSz = $Util::$Image::$ImgFile::$PNG::getSz(imgFile.listLast<VTPath>());
						inst.getList<VTInt>().alloc(kSz2).append(imgSz.$w).append(imgSz.$h);
						//log.txtln("[ %u, %u ] from file : %s", imgSz.$w, imgSz.$h, imgFile.listLast<Path>().fullName().$data);
						break;
					}
					case "Str"_sh:
						inst.listLast<VTStr>().cpy(valStr);
						//log.txtln("%s", valStr.$data);
						break;
					case "Path_Str"_sh:
						inst.listLast<VTPath>().parse(valStr);
						//log.txtln("%s", valStr.$data);
						break;
					case "PathList_Dir"_sh: {
						VarInst parentDir; parentDir.initPath().listAlloc(kSzN).listAppendX();
						analysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
						$Util::$File::getPathListOfDir(parentDir.listLast<VTPath>(), inst.getList<VTPath>(), kBoF);
						log.txtln("Found %u Under Dir : %s", inst.getListSz(), parentDir.listLast<VTPath>().fullName().$data);
						break;
					}
					case "PathList_File"_sh: {
						VarInst parentDir; parentDir.initPath().listAlloc(kSzN).listAppendX();
						analysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
						$Util::$File::getPathListOfFile(parentDir.listLast<VTPath>(), inst.getList<VTPath>(), kBoF);
						log.txtln("Found %u Under Dir : %s", inst.getListSz(), parentDir.listLast<VTPath>().fullName().$data);
						break;
					}
					case "PathList_Dir_Abs"_sh: {
						VarInst parentDir; parentDir.initPath().listAlloc(kSzN).listAppendX();
						analysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
						$Util::$File::getPathListOfDir(parentDir.listLast<VTPath>(), inst.getList<VTPath>(), kBoT);
						log.txtln("Found %u Under Dir : %s", inst.getListSz(), parentDir.listLast<VTPath>().fullName().$data);
						break;
					}
					case "PathList_File_Abs"_sh: {
						VarInst parentDir; parentDir.initPath().listAlloc(kSzN).listAppendX();
						analysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
						$Util::$File::getPathListOfFile(parentDir.listLast<VTPath>(), inst.getList<VTPath>(), kBoT);
						log.txtln("Found %u Under Dir : %s", inst.getListSz(), parentDir.listLast<VTPath>().fullName().$data);
						break;
					}
				}
				break;
			}
			case '?': {
				auto & inputType = strList[strIdx++];
				log.txt("> %s : ", inputType.$data);
				switch (inputType.hashByPtr(operator""_sh)) {
					default:
						throw VAMGException("Executor:VarReader", "unknown type");
						break;
					case "Bool"_sh:
						inst.listLast<VTBool>() = getUserInput<VTBool>();
						break;
					case "Int"_sh:
						inst.listLast<VTInt>() = getUserInput<VTInt>();
						break;
					case "Int_Hex"_sh:
						$Str::scanFmt(getUserInput<VTStr>(), "%llX"_r.$data, &inst.listLast<VTInt>());
						break;
					case "ByteSize_Str"_sh:
						inst.listLast<VTInt>() = $Str::cnvByteSzStr(getUserInput<VTStr>());
						break;
					case "Str"_sh:
						inst.listLast<VTStr>() = getUserInput<VTStr>();
						break;
					case "Path_Str"_sh:
						inst.listLast<VTPath>().parse(getUserInput<VTStr>());
						break;
					case "Path_File"_sh:
						inst.listLast<VTPath>() = getUserInput<VTPath>();
						log.txt("%s\n", inst.listLast<VTPath>().fullName().$data);
						break;
					case "Path_Dir"_sh:
						inst.listLast<VTPath>() = getUserInput<VTPath>();
						inst.listLast<VTPath>().eraseFromEnd();
						log.txt("%s\n", inst.listLast<VTPath>().fullName().$data);
						break;
					case "PathList_File"_sh: {
						VTPath dir = getUserInput<VTPath>();
						dir.eraseFromEnd();
						$Util::$File::getPathListOfFile(dir, inst.getList<VTPath>(), kBoF);
						log.txtln("Found %u Under File : %s", inst.getListSz(), dir.fullName().$data);
						dir.free();
						break;
					}
					case "PathList_Dir"_sh: {
						VTPath dir = getUserInput<VTPath>();
						dir.eraseFromEnd();
						$Util::$File::getPathListOfDir(dir, inst.getList<VTPath>(), kBoF);
						log.txtln("Found %u Under Dir : %s", inst.getListSz(), dir.fullName().$data);
						dir.free();
						break;
					}
					case "PathList_File_Abs"_sh: {
						VTPath dir = getUserInput<VTPath>();
						dir.eraseFromEnd();
						$Util::$File::getPathListOfFile(dir, inst.getList<VTPath>(), kBoT);
						log.txtln("Found %u Under File : %s", inst.getListSz(), dir.fullName().$data);
						dir.free();
						break;
					}
					case "PathList_Dir_Abs"_sh: {
						VTPath dir = getUserInput<VTPath>();
						dir.eraseFromEnd();
						$Util::$File::getPathListOfDir(dir, inst.getList<VTPath>(), kBoT);
						log.txtln("Found %u Under Dir : %s", inst.getListSz(), dir.fullName().$data);
						dir.free();
						break;
					}
				}
				break;
			}
		}
		return;
	}
	static auto analysisCmdStrListFull(List<Str> const & strList, Sz & strIdx, VarInst & inst, VarInstPkg const & varPkg) {
		while (strIdx < strList.$sz)
			analysisCmdStrListOnce(strList, strIdx, inst, varPkg);
		return;
	}
	static auto analysisCmdStrListFullEnter(Str const & str, Sz const & beginAt, VarInst & inst, VarInstPkg const & varPkg) {
		return analysisCmdStrListFull($Str::splitWithPrefix(Str().ref(str, beginAt), crv(List<Str>()), ' '), crv(Sz(kNo1)), inst, varPkg);
	}
	static auto isCmdStr(Str const & s) {
		return !s.isNull() && !s.isEmpty() && s.first() == '@';
	}
	static auto & makeVarInstSingle(VarContainer const & src, VarInst & dst, VarInstPkg const & varPkg) {
		if (src.isStr() && isCmdStr(src.getStr())) {
			analysisCmdStrListFullEnter(src.getStr(), kNo2, dst, varPkg);
		} else {
			switch (dst.$ty.$base) {
				case VarBaseType::kBool:
					dst.listLast<VTBool>() = src.getBool();
					break;
				case VarBaseType::kInt:
					dst.listLast<VTInt>() = src.getIntU();
					break;
				case VarBaseType::kStr:
					dst.listLast<VTStr>() = src.getStr();
					break;
				case VarBaseType::kPath: {
					dst.listLast<VTPath>().alloc();
					for (auto const & e : src.getArr()) {
						auto & str = e.getStr();
						if (isCmdStr(str)) {
							switch (str[kNo2]) {
								default:
									throw VAMGException("Executor:VarReader", "error when get path var");
									break;
								case ' ': {
									VarInst tmp; tmp.initStr().listAlloc(kSz1).listAppendX();
									analysisCmdStrListFullEnter(str, kNo2, tmp, varPkg);
									dst.listLast<VTPath>().append(tmp.listLast<VTStr>());
									tmp.free();
									break;
								}
								case 'P': {
									VarInst tmp; tmp.initPath().listAlloc(kSz1).listAppendX();
									analysisCmdStrListFullEnter(str, kNo3, tmp, varPkg);
									dst.listLast<VTPath>().append(tmp.listLast<VTPath>());
									tmp.free();
								}
							}
						} else
							dst.listLast<VTPath>().append(str);
					}
					break;
				}
			}
		}
		return dst;
	}
	VarInst & makeVarInst(VarContainer const & src, VarInst & dst, VarInstPkg const & varPkg) {
		if (dst.$ty.$isArr) {
			dst.listAlloc(kSzN);
			if (src.isStr()) {
				analysisCmdStrListFullEnter(src.getStr(), kNo2, dst, varPkg);
			} else {
				for (auto const & e : src.getArr()) {
					makeVarInstSingle(VarContainer() = e, dst.listAppendX(), varPkg);
				}
			}
		} else {
			dst.listAlloc(kSz1).listAppendX();
			if (!src.isNull())
				makeVarInstSingle(src, dst, varPkg);
			else
				dst.listSetItemNull();
		}
		return dst;
	}

}
