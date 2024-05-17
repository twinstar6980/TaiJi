# include "./Config.h"

# include "../Type.h"
# include "../Const.h"

# include "../MyType/Str.h"
# include "../MyType/Path.h"
# include "../MyType/JSON.h"

# include "../SomethingElse.h"
# include "../UtilKit/DevUtil.h"
# include "../ImageUtil/ImageType.h"
# include "../ImageUtil/FileUtil.h"

# if defined PLATFORM_ANDROID
# include <jni.h>
# endif

# include <iostream>

namespace $TJ::$Executor {

	tmpl_st(T) static auto & GetUserInput(T & val) {
	# if defined PLATFORM_WINDOWS
		goifcexp(cmpT<T, Bool>()) std::cin >> val;
		elifcexp(cmpT<T, UI64>()) std::cin >> val;
		elifcexp(cmpT<T, Str>()) {
			static Ch tmpChArr[1024] = { '\0' };
			std::cin >> tmpChArr;
			val.cpy(tmpChArr);
		} elifcexp(cmpT<T, Path>()) getFilePathWithValidateByWindow(val, kStorageDir);
	# elif defined PLATFORM_ANDROID
		goifcexp(cmpT<T, Bool>()) {

		} elifcexp(cmpT<T, UI64>()) {

		} elifcexp(cmpT<T, Str>()) {

		} elifcexp(cmpT<T, Path>()) {

		}
	# endif
		return val;
	}

	static auto SplitCmdStr(Str const & src, StrList & dst) {
		dst.setNull();
		Str * ptr(kNullPtr);
		for_criter(ch, src)
			if (ch == ' ')
				ptr = &dst.add().getLast();
			else
				(*ptr).cat(ch);
		return;
	}
	static Void AnalysisCmdStrListOnce(StrList const & strList, SzI & strIdx, VarInst & inst, VarInstPkg const & varPkg) {
		VarInst tmpVar;
		auto & sub = strList[strIdx++];
		switch (sub[0]) {
			default: wErr("UnKnown Cmd Type"); break;
			case '$': {
				auto & id = strList[strIdx++];
				if (sub._len == kSzOne) {
					inst.cpy(varPkg.getValByKey(id));
				} else {
					goif(sub[1] == ']')
						inst.listGetLast<UI64>() = varPkg.getValByKey(id).getListSum();
					elif(sub[1] == '[') {
						VarInst elementIdx; elementIdx.initInt().listAdd();
						switch (sub[2]) {
							default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
							case '*': AnalysisCmdStrListOnce(strList, strIdx, elementIdx, varPkg); break;
							case '.': strList[strIdx++].scanFmt("%llu", &elementIdx.listGet<UI64>()); break;
						}
						switch (inst._type._base) {
							default: break;
							case VarBaseType::kBool: inst.listGetLast<Bool>() = varPkg.getVar<Bool>(id, elementIdx.listGet<UI64>()); break;
							case VarBaseType::kInt: inst.listGetLast<UI64>() = varPkg.getVar<UI64>(id, elementIdx.listGet<UI64>()); break;
							case VarBaseType::kStr: inst.listGetLast<Str>() = varPkg.getVar<Str>(id, elementIdx.listGet<UI64>()); break;
							case VarBaseType::kPath: inst.listGetLast<Path>() = varPkg.getVar<Path>(id, elementIdx.listGet<UI64>()); break;
						}
					}
				}
				break;
			}
			case '#': {
				auto & cmd = strList[strIdx++];
				goif(cmd == "Bool_Inverse") {
					inst.listGetLast<Bool>() = !inst.listGetLast<Bool>();
				} elif(cmd == "Bool_And") {
					tmpVar.initBool().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': strList[strIdx++].scanFmt("%hhu", &tmpVar.listGetLast<Bool>()); break;
					}
					inst.listGetLast<Bool>() &= tmpVar.listGetLast<Bool>();
				} elif(cmd == "Bool_Or") {
					tmpVar.initBool().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': strList[strIdx++].scanFmt("%hhu", &tmpVar.listGetLast<Bool>()); break;
					}
					inst.listGetLast<Bool>() |= tmpVar.listGetLast<Bool>();
				} elif(cmd == "Int_Add") {
					tmpVar.initInt().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': strList[strIdx++].scanFmt("%llu", &tmpVar.listGetLast<UI64>()); break;
					}
					inst.listGetLast<UI64>() += tmpVar.listGetLast<UI64>();
				} elif(cmd == "Int_Sub") {
					tmpVar.initInt().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': strList[strIdx++].scanFmt("%llu", &tmpVar.listGetLast<UI64>()); break;
					}
					inst.listGetLast<UI64>() -= tmpVar.listGetLast<UI64>();
				} elif(cmd == "Str_Ins") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					inst.listGetLast<Str>().ins(tmpVar.listGetLast<Str>());
				} elif(cmd == "Str_Cat") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					inst.listGetLast<Str>().cat(tmpVar.listGetLast<Str>());
				} elif(cmd == "Str_Erase") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					inst.listGetLast<Str>().eraseSub(tmpVar.listGetLast<Str>());
				} elif(cmd == "Path_CatLevel") {
					tmpVar.initPath().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Path>().read(strList[strIdx++]); break;
					}
					inst.listGetLast<Path>().add(tmpVar.listGetLast<Path>());
				} elif(cmd == "Path_AddLevel") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					inst.listGetLast<Path>().add(tmpVar.listGetLast<Str>());
				} elif(cmd == "Path_EraseLastLevel") {
					inst.listGetLast<Path>().eraseLast();
				} elif(cmd == "Path_SetPrefix") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					inst.listGetLast<Path>().setPrefix(tmpVar.listGetLast<Str>());
				} elif(cmd == "Path_SetSuffix") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					inst.listGetLast<Path>().setSuffix(tmpVar.listGetLast<Str>());
				} elif(cmd == "Path_EraseSuffix") {
					inst.listGetLast<Path>().eraseSuffix();
				} elif(cmd == "PathList_FilterSuffix") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					ListP<Path> tmpList;
					pathFilterBySuffix(inst.getList<Path>(), tmpList, tmpVar.listGetLast<Str>());
					inst.getList<Path>() = tmpList;
					wCmt("PathList_FilterSuffix : %u Of %s", inst.getListSum(), tmpVar.listGetLast<Str>()._str);
				} elif(cmd == "PathList_FilterStr") {
					tmpVar.initStr().listAdd();
					switch (sub[1]) {
						default: wErr("ERROR WHEN ANALYSIS CMD-STRLIST"); break;
						case '*': AnalysisCmdStrListOnce(strList, strIdx, tmpVar, varPkg); break;
						case '.': tmpVar.listGetLast<Str>() = strList[strIdx++]; break;
					}
					ListP<Path> tmpList;
					pathFilterByStr(inst.getList<Path>(), tmpList, tmpVar.listGetLast<Str>());
					inst.getList<Path>() = tmpList;
					wCmt("PathList_FilterStr : %u Of %s", inst.getListSum(), tmpVar.listGetLast<Str>()._str);
				} else wErr("UnKnown # Type");
				break;
			}
			case '!': {
				auto & inputType = strList[strIdx++];
				auto & valStr = strList[strIdx++];
				wLog("# %s : ", inputType._str);
				goif(inputType == "Bool") {
					goif(valStr == "false") inst.listGetLast<Bool>() = kFalse;
					elif(valStr == "true") inst.listGetLast<Bool>() = kTrue;
					else wErr("Bool Str Error At \"! Bool\"");
					wLogLn("%s", valStr._str);
				} elif(inputType == "Int") {
					valStr.scanFmt("%llu", &inst.listGetLast<UI64>());
					wLogLn("%s", valStr._str);
				} elif(inputType == "Int_Hex") {
					valStr.scanFmt("%X", &inst.listGetLast<UI64>());
					wLogLn("%s", valStr._str);
				} elif(inputType == "ByteSize_Str") {
					inst.listGetLast<UI64>() = cnvByteSizeStr(valStr);
					wLogLn("%s", valStr._str);
				} elif(inputType == "IntList_ImageSize") {
					$ImageUtil::ImageSize imgSz;
					VarInst imgFile; imgFile.initPath().listAdd();
					AnalysisCmdStrListOnce(strList, --strIdx, imgFile, varPkg);
					$ImageUtil::$FileUtil::$PNGUtil::getSz(imgFile.listGetLast<Path>(), imgSz);
					inst.getList<UI64>().setNull().add(imgSz._w).add(imgSz._h);
					wLogLn("[ %u, %u ] From File : %s", imgSz._w, imgSz._h, imgFile.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} elif(inputType == "Str") {
					inst.listGetLast<Str>().cpy(valStr);
					wLogLn("%s", valStr._str);
				} elif(inputType == "Path_Str") {
					inst.listGetLast<Path>().read(valStr);
					wLogLn("%s", valStr._str);
				} elif(inputType == "PathList_Dir") {
					VarInst parentDir; parentDir.initPath().listAdd();
					AnalysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
					$DevUtil::getDirPathList(parentDir.listGetLast<Path>(), inst.getList<Path>(), kFalse);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} elif(inputType == "PathList_File") {
					VarInst parentDir; parentDir.initPath().listAdd();
					AnalysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
					$DevUtil::getFilePathList(parentDir.listGetLast<Path>(), inst.getList<Path>(), kFalse);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} elif(inputType == "PathList_Dir_Abs") {
					VarInst parentDir; parentDir.initPath().listAdd();
					AnalysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
					$DevUtil::getDirPathList(parentDir.listGetLast<Path>(), inst.getList<Path>(), kTrue);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} elif(inputType == "PathList_File_Abs") {
					VarInst parentDir; parentDir.initPath().listAdd();
					AnalysisCmdStrListOnce(strList, --strIdx, parentDir, varPkg);
					$DevUtil::getFilePathList(parentDir.listGetLast<Path>(), inst.getList<Path>(), kTrue);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} else wErr("UnKnown Type");
				break;
			}
			case '?': {
				auto & inputType = strList[strIdx++];
				wLog("> %s : ", inputType._str);
				goif(inputType == "Bool") {
					GetUserInput(inst.listGetLast<Bool>());
				} elif(inputType == "Int") {
					GetUserInput(inst.listGetLast<UI64>());
				} elif(inputType == "Int_Hex") {
					Str HexStr;
					GetUserInput(HexStr);
					HexStr.scanFmt("%llX", &inst.listGetLast<UI64>());
					HexStr.tfree();
				} elif(inputType == "ByteSize_Str") {
					Str byteSzStr;
					GetUserInput(byteSzStr);
					inst.listGetLast<UI64>() = cnvByteSizeStr(byteSzStr);
					byteSzStr.tfree();
				} elif(inputType == "Str") {
					GetUserInput(inst.listGetLast<Str>());
				} elif(inputType == "Path_Str") {
					Str tmpPath;
					GetUserInput(tmpPath);
					inst.listGetLast<Path>().read(tmpPath);
				} elif(inputType == "Path_Dir") {
					Path tmpPath;
					GetUserInput(tmpPath);
					inst.listGetLast<Path>().cpy(kStorageDir).add(tmpPath, kThFirst, tmpPath._sz - kThSecond);
					tmpPath.tfree();
					wLog("%s\n", inst.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} elif(inputType == "Path_File") {
					Path tmpPath;
					GetUserInput(tmpPath);
					inst.listGetLast<Path>().cpy(kStorageDir).add(tmpPath);
					tmpPath.tfree();
					wLog("%s\n", inst.listGetLast<Path>().getFullPath(Str().setNull())._str);
				} elif(inputType == "PathList_Dir") {
					Path tmpPath, parentDir;
					GetUserInput(tmpPath);
					parentDir.cpy(kStorageDir).add(tmpPath, kThFirst, tmpPath._sz - kThSecond);
					tmpPath.tfree();
					$DevUtil::getDirPathList(parentDir, inst.getList<Path>(), kFalse);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.getFullPath(Str().setNull())._str);
					parentDir.tfree();
				} elif(inputType == "PathList_File") {
					Path tmpPath, parentDir;
					GetUserInput(tmpPath);
					parentDir.cpy(kStorageDir).add(tmpPath, kThFirst, tmpPath._sz - kThSecond);
					tmpPath.tfree();
					$DevUtil::getFilePathList(parentDir, inst.getList<Path>(), kFalse);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.getFullPath(Str().setNull())._str);
					parentDir.tfree();
				} elif(inputType == "PathList_Dir_Abs") {
					Path tmpPath, parentDir;
					GetUserInput(tmpPath);
					parentDir.cpy(kStorageDir).add(tmpPath, kThFirst, tmpPath._sz - kThSecond);
					tmpPath.tfree();
					$DevUtil::getDirPathList(parentDir, inst.getList<Path>(), kTrue);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.getFullPath(Str().setNull())._str);
					parentDir.tfree();
				} elif(inputType == "PathList_File_Abs") {
					Path tmpPath, parentDir;
					GetUserInput(tmpPath);
					parentDir.cpy(kStorageDir).add(tmpPath, kThFirst, tmpPath._sz - kThSecond);
					tmpPath.tfree();
					$DevUtil::getFilePathList(parentDir, inst.getList<Path>(), kTrue);
					wLogLn("Found %u Under Dir : %s", inst.getListSum(), parentDir.getFullPath(Str().setNull())._str);
					parentDir.tfree();
				} else wErr("UnKnown Type");
				break;
			}
		}
		return;
	}
	static auto AnalysisCmdStrListFull(StrList const & strList, SzI & strIdx, VarInst & inst, VarInstPkg const & varPkg) {
		while (strIdx < strList._sz)
			AnalysisCmdStrListOnce(strList, strIdx, inst, varPkg);
		return;
	}
	static auto AnalysisCmdStrListFullEnter(Str const & str, SzI const & beginAt, VarInst & inst, VarInstPkg const & varPkg) {
		StrList cmdList;
		SplitCmdStr(StrRef(str, beginAt), cmdList);
		auto strIdx = kThFirst;
		AnalysisCmdStrListFull(cmdList, strIdx, inst, varPkg);
		return;
	}
	static auto & MakeVarInstSingle(VarContainer const & src, VarInst & dst, VarInstPkg const & varPkg) {
		if (src.isTypeStr() && src.getStr()._len > kSzZero && src.getStr()[0] == '@')
			AnalysisCmdStrListFullEnter(src.getStr(), kThSecond, dst, varPkg);
		else
			switch (dst._type._base) {
				case VarBaseType::kBool: dst.listGetLast<Bool>() = src.getBool(); break;
				case VarBaseType::kInt: dst.listGetLast<UI64>() = src.getUInt(); break;
				case VarBaseType::kStr: dst.listGetLast<Str>() = src.getStr(); break;
				case VarBaseType::kPath: {
					dst.listGetLast<Path>().setNull();
					for_criter(e, src.getArr()) {
						auto & str = e.getStr();
						if (str._len > kSzZero && str[kThFirst] == '@') {
							switch (str[kThSecond]) {
								default: wErr(""); break;
								case ' ': {
									VarInst tmp; tmp.initStr().listAdd();
									AnalysisCmdStrListFullEnter(str, kThSecond, tmp, varPkg);
									dst.listGetLast<Path>().add(tmp.listGetLast<Str>());
									tmp.tfree();
									break;
								}
								case 'P': {
									VarInst tmp; tmp.initPath().listAdd();
									AnalysisCmdStrListFullEnter(str, kThThird, tmp, varPkg);
									dst.listGetLast<Path>().add(tmp.listGetLast<Path>());
									tmp.tfree();
								}
							}
						} else dst.listGetLast<Path>().add(str);
					}
					break;
				}
			}
		return dst;
	}
	VarInst & makeVarInst(VarContainer const & src, VarInst & dst, VarInstPkg const & varPkg) {
		if (dst._type._isArr) {
			dst.listSetNull();
			if (src.isTypeStr())
				AnalysisCmdStrListFullEnter(src.getStr(), kThSecond, dst, varPkg);
			else
				for_cnt(idx, kThFirst, src.getArr()._sz) {
				MakeVarInstSingle(VarContainer(src[idx]), dst.listAdd(), varPkg);
			}
		} else {
			dst.listAlloc(kSzOne).listAdd();
			MakeVarInstSingle(src, dst, varPkg);
		}
		return dst;
	}
}
