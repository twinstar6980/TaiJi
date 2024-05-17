#pragma once

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
#include "../Util/Other/Other.hpp"

#include <iostream>

namespace $TJ::$Executor {

	inline namespace $VT {

		using VTBool = Bool;
		using VTInt = IU64;
		using VTStr = Str;
		using VTPath = Path;

	}

	enum class VarBaseType : IU8 { kNull, kBool, kInt, kStr, kPath };

	struct VarType;
	struct VarContainer;
	struct VarInst;
	struct VarInstPkg;

	VarInst & makeVarInst(VarContainer const & src, VarInst & dst, VarInstPkg const & varPkg);

	struct VarType {

	public:

		VarBaseType $base;
		Bool $isArr;

		auto & set(VarBaseType const & base, Bool const & isArr) {
			$base = base;
			$isArr = isArr;
			return thiz;
		}
		auto & reset() {
			return set(VarBaseType::kNull, kBoF);
		}

	public:

		implicit VarType() :
			$base(VarBaseType::kNull), $isArr(kBoF) {
		}
		explicit VarType(VarType const & t) :
			VarType() {
			thiz = t;
		}
		explicit VarType(VarBaseType const & base, Bool const & isArr) :
			$base(base), $isArr(isArr) {
		}

	public:

		static auto fromStr(Str const & s) {
			VarBaseType base;
			Str type;
			auto isArr = s.first() == '[';
			type.ref(s, isArr ? kNo2 : kNo1);
			switch (type.hashByPtr(operator""_sh)) {
				default:
					throw VAMGException("Executor-Config-ParmTypeParser", "unknown parm-type : %s", type.$data);
					break;
				case "Bool"_sh:
					base = VarBaseType::kBool;
					break;
				case "Int"_sh:
					base = VarBaseType::kInt;
					break;
				case "Str"_sh:
					base = VarBaseType::kStr;
					break;
				case "Path"_sh:
					base = VarBaseType::kPath;
					break;
			}
			type.unref();
			return VarType(base, isArr);
		}

	};

	struct VarInst {

		VarType $ty;
		Void * $data;

		auto & init(VarBaseType const & base, Bool const & isArr = kBoF) {
			$ty.set(base, isArr);
			switch (base) {
				default:
					break;
				case VarBaseType::kNull:
					$data = kNullPtr;
					break;
				case VarBaseType::kBool:
					$data = new List<VTBool>();
					break;
				case VarBaseType::kInt:
					$data = new List<VTInt>();
					break;
				case VarBaseType::kStr:
					$data = new List<VTStr>();
					break;
				case VarBaseType::kPath:
					$data = new List<VTPath>();
					break;
			}
			return thiz;
		}
		auto & init(VarType const & type) {
			return init(type.$base, type.$isArr);
		}
		auto & init(Str const & type) {
			return init(VarType::fromStr(type));
		}

		auto & initBool(Bool const & isArr = kBoF) {
			return init(VarBaseType::kBool, isArr);
		}
		auto & initInt(Bool const & isArr = kBoF) {
			return init(VarBaseType::kInt, isArr);
		}
		auto & initStr(Bool const & isArr = kBoF) {
			return init(VarBaseType::kStr, isArr);
		}
		auto & initPath(Bool const & isArr = kBoF) {
			return init(VarBaseType::kPath, isArr);
		}

		template <typename T>
		auto & initT(Bool const & isArr = kBoF) {
			if constexpr (kTSame<T, VTBool>)
				init(VarBaseType::kBool, isArr);
			else if constexpr (kTSame<T, VTInt>)
				init(VarBaseType::kInt, isArr);
			else if constexpr (kTSame<T, VTStr>)
				init(VarBaseType::kStr, isArr);
			else if constexpr (kTSame<T, VTPath>)
				init(VarBaseType::kPath, isArr);
			else
				init(VarBaseType::kNull, isArr);
			return thiz;
		}

		auto & free() {
			if ($data != kNullPtr) {
				switch ($ty.$base) {
					default:
						break;
					case VarBaseType::kNull:
						break;
					case VarBaseType::kBool:
						pvFreeS<List<VTBool>>($data);
						break;
					case VarBaseType::kInt:
						pvFreeS<List<VTInt>>($data);
						break;
					case VarBaseType::kStr:
						pvFreeS<List<VTStr>>($data);
						break;
					case VarBaseType::kPath:
						pvFreeS<List<VTPath>>($data);
						break;
				}
			}
			$ty.reset();
			return thiz;
		}

		template <typename T>
		auto & getList() {
			return *static_cast<List<T> *>($data);
		}
		template <typename T>
		auto & getList() const {
			return *static_cast<List<T> const *>($data);
		}

		auto & getListSz() const {
			return getList<NullPtr>().$sz;
		}

		auto & cpy(VarInst const & t) {
			free();
			init(t.$ty);
			switch ($ty.$base) {
				default:
					break;
				case VarBaseType::kNull:
					break;
				case VarBaseType::kBool:
					getList<VTBool>() = t.getList<VTBool>();
					break;
				case VarBaseType::kInt:
					getList<VTInt>() = t.getList<VTInt>();
					break;
				case VarBaseType::kStr:
					getList<VTStr>() = t.getList<VTStr>();
					break;
				case VarBaseType::kPath:
					getList<VTPath>() = t.getList<VTPath>();
					break;
			}
			return thiz;
		}

		auto & listSetNull() {
			switch ($ty.$base) {
				default:
					break;
				case VarBaseType::kNull:
					break;
				case VarBaseType::kBool:
					getList<VTBool>().setNull();
					break;
				case VarBaseType::kInt:
					getList<VTInt>().setNull();
					break;
				case VarBaseType::kStr:
					getList<VTStr>().setNull();
					break;
				case VarBaseType::kPath:
					getList<VTPath>().setNull();
					break;
			}
			return thiz;
		}

		auto & listAlloc(Sz const & cap) {
			switch ($ty.$base) {
				default:
					break;
				case VarBaseType::kNull:
					break;
				case VarBaseType::kBool:
					getList<VTBool>().alloc(cap);
					break;
				case VarBaseType::kInt:
					getList<VTInt>().alloc(cap);
					break;
				case VarBaseType::kStr:
					getList<VTStr>().alloc(cap);
					break;
				case VarBaseType::kPath:
					getList<VTPath>().alloc(cap);
					break;
			}
			return thiz;
		}

		auto & listAppendX() {
			switch ($ty.$base) {
				default:
					break;
				case VarBaseType::kNull:
					break;
				case VarBaseType::kBool:
					getList<VTBool>().appendX();
					break;
				case VarBaseType::kInt:
					getList<VTInt>().appendX();
					break;
				case VarBaseType::kStr:
					getList<VTStr>().appendX();
					break;
				case VarBaseType::kPath:
					getList<VTPath>().appendX();
					break;
			}
			return thiz;
		}

		template <typename T>
		auto & listGet(Sz const & i = kNo1) {
			return getList<T>().get(i);
		}
		template <typename T>
		auto & listGet(Sz const & i = kNo1) const {
			return getList<T>().get(i);
		}

		template <typename T>
		auto & listLast() {
			return getList<T>().last();
		}
		template <typename T>
		auto & listLast() const {
			return getList<T>().last();
		}

		auto & listSetItemNull(Sz const & i = kNo1) {
			switch ($ty.$base) {
				default:
					break;
				case VarBaseType::kNull:
					break;
				case VarBaseType::kBool:
					getList<VTBool>()[i] = VTBool();
					break;
				case VarBaseType::kInt:
					getList<VTInt>()[i] = VTInt();
					break;
				case VarBaseType::kStr:
					getList<VTStr>()[i] = VTStr();
					break;
				case VarBaseType::kPath:
					getList<VTPath>()[i] = VTPath();
					break;
			}
			return thiz;
		}

		auto & fromContainer(VarContainer const & src, VarInstPkg const & varPkg) {
			makeVarInst(src, thiz, varPkg);
			return thiz;
		}

		auto & operator=(VarInst const & t) {
			return cpy(t);
		}

		~VarInst() {
			free();
		}
		implicit VarInst() :
			$ty(), $data(kNullPtr) {
		}
		explicit VarInst(VarInst const & t) :
			VarInst() {
			cpy(t);
		}
		explicit VarInst(VarBaseType const & type, Bool const & isArr = kBoF) :
			VarInst() {
			$ty.set(type, isArr);
		}

	};

	struct VarContainer : $JSON::Val {

		template <typename T>
		auto & makeInst(VarInstPkg const & varPkg, VarInst & dst) const {
			return makeVarInst(thiz, dst.initT<T>(), varPkg);
		}
		template <typename T>
		auto makeInst(VarInstPkg const & varPkg) const {
			return VarInst(makeVarInst(thiz, VarInst().initT<T>(), varPkg));
		}

		auto & operator=(Val const & t) {
			Val::operator=(t);
			return thiz;
		};

	};

	struct VarInstPkg : Map<Str, VarInst> {

		auto & addVar(Str const & id, Str const & type) {
			appendX().last().setKey(id).$v.init(type);
			return thiz;
		}

		template <typename T>
		auto & getVarList(Str const & id) {
			return getByKey(id).$v.getList<T>();
		}
		template <typename T>
		auto & getVarList(Str const & id) const {
			return getByKey(id).$v.getList<T>();
		}

		template <typename T>
		auto & getVar(Str const & id, Sz const & i = kNo1) {
			return getByKey(id).$v.listGet<T>(i);
		}
		template <typename T>
		auto & getVar(Str const & id, Sz const & i = kNo1) const {
			return getByKey(id).$v.listGet<T>(i);
		}

		auto & getVarType(Str const & id) const {
			return getByKey(id).$v.$ty;
		}

	};

	struct FuncTmpl {

		List<Str> $func;
		Map<Str, VarType> $parm;

		auto & read($JSON::Obj const & t) {
			$JSON::getListFromArr(t["func"_r], $func);
			$parm.alloc(t["parm"_r].getObj().$sz);
			for (auto const & e : t["parm"_r].getObj())
				$parm.appendX().last().set(e.$k.getStr(), VarType::fromStr(e.$v.getStr()));
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

	struct LoopCond {

		Bool $onlyOnce;
		VarContainer $beg;
		VarContainer $end;

		auto & setNull() {
			$onlyOnce = kBoT;
			$beg.setNull();
			$end.setNull();
			return thiz;
		}
		auto & setOnce() {
			$onlyOnce = kBoT;
			$beg.set(kNo1);
			$end.set(kNo2);
			return thiz;
		}

		auto & read($JSON::Arr const & t) {
			$onlyOnce = kBoF;
			$beg = t[kNo1];
			$end = t[kNo2];
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getArr());
		}

	};

	struct VarSrc {

		VarContainer	$enable;
		VarContainer	$msg;
		VarType			$ty;
		VarContainer	$v;

		auto & init() {
			$enable.set(kBoT);
			$msg.setNull();
			$ty.reset();
			$v.setNull();
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			init();
			if (t.isObj()) {
				if (t.getObj().exist("enable"_r)) {
					$enable = t["enable"_r];
				}
				if (t.getObj().exist("msg"_r)) {
					$msg = t["msg"_r];
				}
				if (t.getObj().exist("type"_r)) {
					$ty = VarType::fromStr(t["type"_r].getStr());
				}
				$v = t["val"_r];
			} else {
				$v = t;
			}
			return thiz;
		}

	};

	struct VarSrcMap : InfoMap<VarSrc> {

		auto & loadTo(VarInstPkg & dst, VarInstPkg const & help) const {
			for (auto const & e : thiz) {
				if (e.$v.$enable.template makeInst<VTBool>(help).template listGet<VTBool>()) {
					auto & dstVal = dst.appendX().last().setKey(e.$k).$v.init(e.$v.$ty);
					if (help.existKey("="_r + e.$k)) {
						dstVal.cpy(help.getByKey("="_r + e.$k).$v);
					} else {
						if (!e.$v.$msg.isNull())
							log.msg("%s", e.$v.$msg.template makeInst<VTStr>(help).template listLast<VTStr>().$data);
						makeVarInst(e.$v.$v, dstVal, help);
					}
				}
			}
			return thiz;
		}
		auto & loadTo(VarInstPkg & dst) const {
			for (auto const & e : thiz) {
				if (e.$v.$enable.template makeInst<VTBool>(dst).template listGet<VTBool>()) {
					auto & dstVal = dst.appendX().last().setKey(e.$k).$v.init(e.$v.$ty);
					if (dst.existKey("="_r + e.$k)) {
						dstVal.cpy(dst.getByKey("="_r + e.$k).$v);
					} else {
						if (!e.$v.$msg.isNull())
							log.msg("%s", e.$v.$msg.template makeInst<VTStr>(dst).template listLast<VTStr>().$data);
						makeVarInst(e.$v.$v, dstVal, dst);
					}
				}
			}
			return thiz;
		}
		auto & eraseTo(VarInstPkg & dst) const {
			for (auto const & e : thiz.rrange()) {
				if (e.$v.$enable.template makeInst<VTBool>(dst).template listGet<VTBool>())
					dst.eraseByKey(e.$k);
			}
			return thiz;
		}
		auto & modTo(VarInstPkg & dst) const {
			for (auto const & e : thiz) {
				if (e.$v.$enable.template makeInst<VTBool>(dst).template listGet<VTBool>()) {
					if (!e.$v.$msg.isNull())
						log.msg("%s", e.$v.$msg.template makeInst<VTStr>(dst).template listLast<VTStr>().$data);
					makeVarInst(e.$v.$v, dst.getByKey(e.$k).$v, dst);
				}
			}
			return thiz;
		}

	};

	enum class ScriptType : IU8 { kIdle, kLink, kList, kCall };

	struct Script {

		VarContainer $enable;
		VarContainer $msg;
		VarContainer $workSpace;
		LoopCond $loopCond;
		Bool $showLoopProg;
		VarSrcMap $reVarIntro;
		VarSrcMap $reVarOutro;
		VarSrcMap $var;
		VarSrcMap $reVarBefore;
		VarSrcMap $reVarAfter;
		ScriptType $ty;
		VarContainer $linkScript;
		InfoList<Script> $listScript;
		Bool $showListProg;
		List<Str> $callFunc;
		VarSrcMap $callParm;

		auto & init() {
			$enable.set(kBoT);
			$msg.setNull();
			$workSpace.setNull();
			$loopCond.setOnce();
			$showLoopProg = kBoF;
			$reVarIntro.alloc();
			$reVarOutro.alloc();
			$var.alloc();
			$reVarBefore.alloc();
			$reVarAfter.alloc();
			$ty = ScriptType::kIdle;
			$linkScript.setNull();
			$listScript.alloc();
			$showListProg = kBoF;
			$callFunc.alloc();
			$callParm.alloc();
			return thiz;
		}

		auto & read($JSON::Obj const & t) {
			init();
			if (t.exist("enable"_r)) {
				$enable = t["enable"_r];
			}
			if (t.exist("msg"_r)) {
				$msg = t["msg"_r];
			}
			if (t.exist("workSpace"_r)) {
				$workSpace = t["workSpace"_r];
			}
			if (t.exist("loop"_r)) {
				$loopCond.read(t["loop"_r]);
			}
			if (t.exist("showLoopProg"_r)) {
				$showLoopProg = t["showLoopProg"_r].getBool();
			}
			if (t.exist("reVarIntro"_r)) {
				$reVarIntro.read(t["reVarIntro"_r]);
			}
			if (t.exist("reVarOutro"_r)) {
				$reVarOutro.read(t["reVarOutro"_r]);
			}
			if (t.exist("var"_r)) {
				$var.read(t["var"_r]);
			}
			if (t.exist("reVarBefore"_r)) {
				$reVarBefore.read(t["reVarBefore"_r]);
			}
			if (t.exist("reVarAfter"_r)) {
				$reVarAfter.read(t["reVarAfter"_r]);
			}
			if (t.exist("type"_r)) {
				auto & type = t["type"_r].getStr();
				switch (type.hashByPtr(operator""_sh)) {
					default:
						throw VAMGException("Executor-Config-ScriptParser", "error script type : %s", type.$data);
						break;
					case "idle"_sh:
						$ty = ScriptType::kIdle;
						break;
					case "link"_sh:
						$ty = ScriptType::kLink;
						$linkScript = t["script"_r];
						break;
					case "list"_sh:
						$ty = ScriptType::kList;
						$listScript.read(t["script"_r]);
						if (t.exist("showListProg"_r)) {
							$showListProg = t["showListProg"_r].getBool();
						}
						break;
					case "call"_sh:
						$ty = ScriptType::kCall;
						$JSON::getListFromArr(t["func"_r], $callFunc);
						$callParm.read(t["parm"_r]);
						break;
				}
			}
			return thiz;
		}

		Script & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

	InfoList<Script> & getScriptListFromJSON($JSON::Val const & src, InfoList<Script> & dst);

	inline auto getScriptListFromJSON(Str const & src, InfoList<Script> & dst) {
		return getScriptListFromJSON($JSON::read(src, crv($JSON::Val())), dst);
	}
	inline auto getScriptListFromJSON(Path const & src, InfoList<Script> & dst) {
		return getScriptListFromJSON($JSON::read(src, crv($JSON::Val())), dst);
	}

	struct QEOption {

		List<List<Str>> $filter;
		Map<Str, Str> $tip;
		InfoList<Script> $script;

		auto & init() {
			$filter.alloc();
			$tip.alloc();
			$script.alloc();
			return thiz;
		}

		auto & read($JSON::Obj const & t) {
			init();
			if (t.exist("filter"_r)) {
				$filter.alloc(t["filter"_r].getArr().$sz);
				for (auto const & e : t["filter"_r].getArr()) {
					$filter.appendX().last().alloc(e.getArr().$sz);
					for (auto const & ee : e.getArr()) {
						$filter.last().append(ee.getStr());
					}
				}
			}
			if (t.exist("tip"_r)) {
				$tip.alloc(t["tip"_r].getObj().$sz);
				for (auto const & e : t["tip"_r].getObj())
					$tip.appendByKV(e.$k.getStr(), e.$v.getStr());
			}
			if (t.exist("script"_r)) {
				getScriptListFromJSON(t["script"_r], $script);
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

		auto isValidPath(Path const & path) const {
			auto result = kBoF;
			for (auto const & oneFilter : $filter) {
				auto curIsValid = kBoT;
				for (auto const & filterStr : oneFilter) {
					if (filterStr.isEmpty())
						throw VAMGException("Executor:QEOption:isValidPath", "empty filter command");
					Str s, cmd, arg;
					auto invResult = filterStr.first() == '!';
					s.ref(filterStr, invResult ? kNo2 : kNo1);
					cmd.ref(s, kNo1, s.range().find(':'));
					arg.ref(s, s.range().find(':') + kNo2);
					auto thisFilterIsValid = kBoF;
					switch (cmd.hashByPtr(operator""_sh)) {
						default:
							throw VAMGException("Executor:QEOption:isValidPath", "unknown filter command");
							break;
						case ""_sh:
							thisFilterIsValid = kBoT;
							break;
						case "type"_sh:
							switch (arg.hashByPtr(operator""_sh)) {
								default:
									throw VAMGException("Executor:QEOption:isValidPath", "unknown path type");
									break;
								case "file"_sh:
									thisFilterIsValid = $Util::$File::existFile(path);
									break;
								case "dir"_sh:
									thisFilterIsValid = $Util::$File::existDir(path);
									break;
							}
							break;
						case "szeq"_sh:
							thisFilterIsValid = $Util::$File::existFile(path) && $Util::$File::getFileSize(path) == $Str::cnvByteSzStr(arg);
							break;
						case "szlt"_sh:
							thisFilterIsValid = $Util::$File::existFile(path) && $Util::$File::getFileSize(path) < $Str::cnvByteSzStr(arg);
							break;
						case "szgt"_sh:
							thisFilterIsValid = $Util::$File::existFile(path) && $Util::$File::getFileSize(path) > $Str::cnvByteSzStr(arg);
							break;
						case "extension"_sh:
							thisFilterIsValid = arg == "*"_r || path.cmpExtensionIgnoreCase(arg);
							break;
						case "substr"_sh:
							thisFilterIsValid = path.fileName().range().existSub(arg.range());
							break;
					}
					curIsValid &= invResult ? !thisFilterIsValid : thisFilterIsValid;
				}
				result |= curIsValid;
			}
			return result;
		}

	};

	using QEOptionGroup = InfoMap<QEOption>;
	using QEOptionGroupMap = InfoMap<QEOptionGroup>;

	struct QuickEnterConfig {

		Bool $ignoreFilter;
		ExistShell<Bool> $enableBatchMode;
		ExistShell<Sz> $batchDepth;
		QEOptionGroupMap $option;
		QEOptionGroupMap $optionForBatch;

		auto & init() {
			$ignoreFilter = kBoF;
			$enableBatchMode.set(kBoF, kBoF);
			$batchDepth.set(kBoF, kBoF);
			$option.alloc();
			$optionForBatch.alloc();
			return thiz;
		}

		auto & read($JSON::Obj const & t) {
			init();
			if (t.exist("ignoreFilter"_r)) {
				$ignoreFilter = t["ignoreFilter"_r].getBool();
			}
			if ($enableBatchMode.$state = t.exist("enableBatchMode"_r) && !t["enableBatchMode"_r].isNull()) {
				$enableBatchMode.$data = t["enableBatchMode"_r].getBool();
			}
			if ($batchDepth.$state = t.exist("batchDepth"_r) && !t["batchDepth"_r].isNull()) {
				$batchDepth.$data = t["batchDepth"_r].getIntU();
			}
			if (t.exist("option"_r)) {
				$option.read(t["option"_r].getObj());
			}
			if (t.exist("optionForBatch"_r)) {
				$optionForBatch.read(t["optionForBatch"_r].getObj());
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

	struct Config {

		InfoList<FuncTmpl> $func;
		VarSrcMap $var;
		InfoList<Script> $intro;
		InfoList<Script> $outro;
		QuickEnterConfig $quickEnter;

		auto & init() {
			$func.alloc();
			$var.alloc();
			$intro.alloc();
			$outro.alloc();
			$quickEnter.init();
			return thiz;
		}

		auto & read($JSON::Obj const & t) {
			init();
			if (t.exist("func"_r)) {
				$func.read(t["func"_r].getArr());
			}
			if (t.exist("var"_r)) {
				$var.read(t["var"_r]);
			}
			if (t.exist("intro"_r)) {
				$intro.read(t["intro"_r].getArr());
			}
			if (t.exist("outro"_r)) {
				$outro.read(t["outro"_r].getArr());
			}
			if (t.exist("quickEnter"_r)) {
				$quickEnter.read(t["quickEnter"_r].getObj());
			}
			return thiz;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}

	};

}
