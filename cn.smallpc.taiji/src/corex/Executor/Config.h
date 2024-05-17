# pragma once

# include "../Type.h"
# include "../Const.h"
# include "../MyType/ListB.h"
# include "../MyType/Str.h"
# include "../MyType/Path.h"
# include "../MyType/ListOfOther.h"
# include "../MyType/Map.h"
# include "../UtilKit/JSONUtil.h"
# include "../UtilKit/DevUtil.h"

namespace $TJ::$Executor {

	enum class VarBaseType : UI8 { kNull, kBool, kInt, kStr, kPath };

	struct VarType;
	struct VarContainer;
	struct VarInst;
	struct VarInstPkg;

	VarInst & makeVarInst(VarContainer const & src, VarInst & dst, VarInstPkg const & varPkg);

	struct VarType {
		VarBaseType _base;
		Bool _isArr;

		auto & set(VarBaseType const & base, Bool const & isArr) {
			_base = base;
			_isArr = isArr;
			return *this;
		}
		auto & reset() { return set(VarBaseType::kNull, kFalse); }

		auto & cpy(VarType const & src) {
			_base = src._base;
			_isArr = src._isArr;
			return *this;
		}

		static auto analysisStr(Str const & str) {
			VarBaseType base;
			StrRef strRef(str);
			auto isArr = strRef[kThFirst] == '[';
			if (isArr) strRef += kSzOne;
			goif(strRef.cmp("Bool")) base = VarBaseType::kBool;
			elif(strRef.cmp("Int")) base = VarBaseType::kInt;
			elif(strRef.cmp("Str")) base = VarBaseType::kStr;
			elif(strRef.cmp("Path")) base = VarBaseType::kPath;
			else wErr("UnKnown ParmType Name \"%s\"", strRef._str);
			strRef.clear();
			return VarType(base, isArr);
		}

		VarType() : _base(VarBaseType::kNull), _isArr(kFalse) { return; }
		explicit VarType(VarBaseType const & base, Bool & isArr) : _base(base), _isArr(isArr) { return; }
		explicit VarType(VarType const & src) { cpy(src); return; }

		auto & operator=(VarType const & src) { return cpy(src); }

	};

	struct VarInst {
		VarType _type;
		Void * _list;

		auto & getThis() { return *this; }

		auto & fromContainer(VarContainer const & src, VarInstPkg const & varPkg) {
			makeVarInst(src, *this, varPkg);
			return *this;
		}

		tmpl_st(T) auto & getList() { return *static_cast<ListP<T> *>(_list); }
		tmpl_st(T) auto & getList() const { return *static_cast<ListP<T> const *>(_list); }

		auto & getListSum() const { return getList<Byte>()._sz; }

		auto & init(VarBaseType const & base, Bool const & isArr = kFalse) {
			_type.set(base, isArr);
			switch (base) {
				default: break; // TODO
				case VarBaseType::kNull: _list = kNullPtr; break;
				case VarBaseType::kBool: _list = new ListP<Bool>(); break;
				case VarBaseType::kInt: _list = new ListP<UI64>(); break;
				case VarBaseType::kStr: _list = new ListP<Str>(); break;
				case VarBaseType::kPath: _list = new ListP<Path>(); break;
			}
			return *this;
		}
		auto & init(VarType const & type) { return init(type._base, type._isArr); }
		auto & init(Str const & type) { return init(VarType::analysisStr(type)); }

		auto & initBool(Bool const & isArr = kFalse) { return init(VarBaseType::kBool, isArr); }
		auto & initInt(Bool const & isArr = kFalse) { return init(VarBaseType::kInt, isArr); }
		auto & initStr(Bool const & isArr = kFalse) { return init(VarBaseType::kStr, isArr); }
		auto & initPath(Bool const & isArr = kFalse) { return init(VarBaseType::kPath, isArr); }
		tmpl_st(T) auto & initT(Bool const & isArr = kFalse) {
			goifcexp(cmpT<T, Bool>())
				init(VarBaseType::kBool, isArr);
			elifcexp(cmpT<T, UI64>())
				init(VarBaseType::kInt, isArr);
			elifcexp(cmpT<T, Str>())
				init(VarBaseType::kStr, isArr);
			elifcexp(cmpT<T, Path>())
				init(VarBaseType::kPath, isArr);
			else
			init(VarBaseType::kNull, isArr);
			return *this;
		}

		auto & tfree() {
			if (_list != kNullPtr)
				switch (_type._base) {
					default: break; // TODO
					case VarBaseType::kNull: break;
					case VarBaseType::kBool: delete static_cast<ListP<Bool> *>(_list); break;
					case VarBaseType::kInt: delete static_cast<ListP<UI64> *>(_list); break;
					case VarBaseType::kStr: delete static_cast<ListP<Str> *>(_list); break;
					case VarBaseType::kPath: delete static_cast<ListP<Path> *>(_list); break;
				}
			_type.reset();
			return *this;
		}

		auto & cpy(VarInst const & src) {
			tfree();
			init(src._type);
			switch (_type._base) {
				default: break; // TODO
				case VarBaseType::kNull: break;
				case VarBaseType::kBool: getList<Bool>().cpyByOper(src.getList<Bool>()); break;
				case VarBaseType::kInt: getList<UI64>().cpyByOper(src.getList<UI64>()); break;
				case VarBaseType::kStr: getList<Str>().cpy(src.getList<Str>()); break;
				case VarBaseType::kPath: getList<Path>().cpy(src.getList<Path>()); break;
			}
			return *this;
		}

		auto & listSetNull() {
			switch (_type._base) {
				default: break; // TODO
				case VarBaseType::kNull: break;
				case VarBaseType::kBool: getList<Bool>().setNull(); break;
				case VarBaseType::kInt: getList<UI64>().setNull(); break;
				case VarBaseType::kStr: getList<Str>().setNull(); break;
				case VarBaseType::kPath: getList<Path>().setNull(); break;
			}
			return *this;
		}
		auto & listAlloc(SzI const & cap) {
			switch (_type._base) {
				default: break; // TODO
				case VarBaseType::kNull: break;
				case VarBaseType::kBool: getList<Bool>().alloc(cap); break;
				case VarBaseType::kInt: getList<UI64>().alloc(cap); break;
				case VarBaseType::kStr: getList<Str>().alloc(cap); break;
				case VarBaseType::kPath: getList<Path>().alloc(cap); break;
			}
			return *this;
		}
		auto & listAdd() {
			switch (_type._base) {
				default: break; // TODO
				case VarBaseType::kNull: break;
				case VarBaseType::kBool: getList<Bool>().add(); break;
				case VarBaseType::kInt: getList<UI64>().add(); break;
				case VarBaseType::kStr: getList<Str>().add(); break;
				case VarBaseType::kPath: getList<Path>().add(); break;
			}
			return *this;
		}

		tmpl_st(T) auto & listGet(SzI const & idx = kThFirst) { return getList<T>().get(idx); }
		tmpl_st(T) auto & listGet(SzI const & idx = kThFirst) const { return getList<T>().get(idx); }
		tmpl_st(T) auto & listGetLast() { return getList<T>().getLast(); }
		tmpl_st(T) auto & listGetLast() const { return getList<T>().getLast(); }

		~VarInst() { tfree(); return; }
		VarInst() : _type(), _list(kNullPtr) { return; }
		explicit VarInst(VarInst const & src) { cpy(src); return; }
		explicit VarInst(VarBaseType const & type) : _type(), _list(kNullPtr) {
			_type.set(type, kFalse);
			return;
		}

		auto & operator=(VarInst const & src) { return cpy(src); }
	};

	struct VarContainer : $JSON::Val {

		tmpl_st(T) auto & makeInst(VarInstPkg const & varPkg, VarInst & dst) const {
			return makeVarInst(*this, dst.initT<T>(), varPkg);
		}
		tmpl_st(T) auto makeInst(VarInstPkg const & varPkg) const {
			return VarInst(makeVarInst(*this, VarInst().initT<T>(), varPkg));
		}

		VarContainer() { return; }
		explicit VarContainer($JSON::Val const & src) { cpy(src); return; }

		auto & operator=($JSON::Val const & src) { return cpy(src); };

	};

	struct VarInstPkg : Map<Str, VarInst> {

		auto & addVar(Str const & id, Str const & type) {
			addByKey(id).getLast()._val.init(type);
			return *this;
		}

		tmpl_st(T) auto & getVarList(Str const & id) {
			return getValByKey(id).getList<T>();
		}
		tmpl_st(T) auto & getVarList(Str const & id) const {
			return getValByKey(id).getList<T>();
		}
		tmpl_st(T) auto & getVar(Str const & id, SzI const & elementIdx = kThFirst) {
			return getValByKey(id).listGet<T>(elementIdx);
		}
		tmpl_st(T) auto & getVar(Str const & id, SzI const & elementIdx = kThFirst) const {
			return getValByKey(id).listGet<T>(elementIdx);
		}
		tmpl_st(T) auto & getVar(Ch const * const & id, SzI const & elementIdx = kThFirst) {
			return getValByKey(StrRef(id)).listGet<T>(elementIdx);
		}
		tmpl_st(T) auto & getVar(Ch const * const & id, SzI const & elementIdx = kThFirst) const {
			return getValByKey(StrRef(id)).listGet<T>(elementIdx);
		}
		auto & getVarType(Str const & id) const {
			return getValByKey(id)._type;
		}
	};

	struct FuncTmpl {
		StrList _func;
		Map<Str, VarType> _parm;

		auto & read($JSON::Obj const & src) {
			_func.setNull(); _parm.setNull();
			$JSON::getArrStr(src["Func"].getArr(), _func);
			for_criter(e, src["Parm"].getObj())
				_parm.addByKV(e._key.get(), VarType::analysisStr(e._val.getStr()));
			return *this;
		}
	};
	using FuncTmplList = ListOfInfo<FuncTmpl>;

	struct LoopCond {
		Bool _onlyOnce;
		VarContainer _begin;
		VarContainer _end;

		auto & setNull() {
			_onlyOnce = kTrue;
			_begin.setNull();
			_end.setNull();
			return *this;
		}
		auto & setOnce() {
			_onlyOnce = kTrue;
			_begin.set(kThFirst);
			_end.set(kThSecond);
			return *this;
		}
		auto & read($JSON::Arr const & src) {
			_onlyOnce = kFalse;
			_begin = src[kThFirst];
			_end = src[kThSecond];
			return *this;
		}

		LoopCond() { return; }
		auto & operator=($JSON::Arr const & src) { return read(src); }
	};

	enum class VarSrcType : UI8 { kDefine, kSimple, kWithState };

	namespace $VarSrcType {
		constexpr UI8 kVal(1);
		constexpr UI8 kType(3);
		constexpr UI8 kEnAble(7);
		constexpr UI8 kCmt(15);
		constexpr UI8 kSimple(kVal);
		constexpr UI8 kWithState(kVal | kEnAble | kCmt);
		constexpr UI8 kDefine(kType | kVal | kEnAble | kCmt);

	}

	template <VarSrcType _srcType>
	struct VarSrc {
		VarContainer	_enable;
		VarContainer	_cmt;
		VarType			_type;
		VarContainer	_val;

		auto & cpy(VarSrc const & src) {
			_enable = src._enable;
			_type = src._type;
			_val = src._val;
			return *this;
		}
		auto & init() {
			_enable.set(kTrue);
			_cmt.setNull();
			_type.reset();
			_val.setNull();
			return *this;
		}
		auto & read($JSON::Val const & src) {
			init();
			switch (_srcType) {
				default: break;
				case VarSrcType::kDefine:
					if (src.getObj().exist("EnAble")) {
						_enable = src["EnAble"];
					}
					if (src.getObj().exist("Cmt")) {
						_cmt = src["Cmt"];
					}
					_type = VarType::analysisStr(src["Type"].getStr());
					_val = src["Val"];
					break;
				case VarSrcType::kWithState:
					if (src.getObj().exist("EnAble")) {
						_enable = src["EbAble"];
					}
					if (src.getObj().exist("Cmt")) {
						_cmt = src["Cmt"];
					}
					_val = src["Val"];
					break;
				case VarSrcType::kSimple:
					_val = src;
					break;
			}
			return *this;
		}

		auto & operator=(VarSrc const & src) { return cpy(src); }
	};

	template <VarSrcType _srcType>
	struct VarSrcMap : MapOfInfo<VarSrc<_srcType>> {
		auto & read($JSON::Obj const & src) {
			for_criter(e, src)
				(*this).addByKey(e._key.get()).getLast()._val.read(e._val);
			return *this;
		}

		auto & loadTo(VarInstPkg & dst, VarInstPkg const & help) const {
			for_criter(e, *this) {
				if (e._val._enable.template makeInst<Bool>(help).template listGet<Bool>()) {
					auto & dstVal = dst.addByKey(e._key).getLastVal().init(e._val._type);
					if (help.existKey(Str(e._key).ins("="))) {
						dstVal.cpy(help.getValByKey(Str(e._key).ins("=")));
					} else {
						if (!e._val._cmt.isTypeNull())
							wCmt("Var-Cmt : %s", e._val._cmt.template makeInst<Str>(help).template listGetLast<Str>()._str);
						makeVarInst(e._val._val, dstVal, help);
					}
				}
			}
			return *this;
		}
		auto & loadTo(VarInstPkg & dst) const {
			for_criter(e, *this) {
				if (e._val._enable.template makeInst<Bool>(dst).template listGet<Bool>()) {
					auto & dstVal = dst.addByKey(e._key).getLastVal().init(e._val._type);
					if (dst.existKey(Str(e._key).ins("="))) {
						dstVal.cpy(dst.getValByKey(Str(e._key).ins("=")));
					} else {
						if (!e._val._cmt.isTypeNull())
							wCmt("Var-Cmt : %s", e._val._cmt.template makeInst<Str>(dst).template listGetLast<Str>()._str);
						makeVarInst(e._val._val, dstVal, dst);
					}
				}
			}
			return *this;
		}
		auto & eraseTo(VarInstPkg & dst) const {
			for_criter(e, *this)
				if (e._val._enable.template makeInst<Bool>(dst).template listGet<Bool>())
					dst.eraseByKey(e._key);
			return *this;
		}
		auto & modTo(VarInstPkg & dst) const {
			for_criter(e, *this) {
				if (e._val._enable.template makeInst<Bool>(dst).template listGet<Bool>()) {
					if (!e._val._cmt.isTypeNull())
						wCmt("Var-Cmt : %s", e._val._cmt.template makeInst<Str>(dst).template listGetLast<Str>()._str);
					makeVarInst(e._val._val, dst.getValByKey(e._key), dst);
				}
			}
			return *this;
		}

		VarSrcMap & operator=(VarSrcMap const & src) {
			(*this).cpy(src);
			return *this;
		}
		auto & operator=($JSON::Obj const & src) { return read(src); }
	};

	using VarSrcMapForSimple = VarSrcMap<VarSrcType::kSimple>;
	using VarSrcMapForDefine = VarSrcMap<VarSrcType::kDefine>;

	enum class ScriptType : UI8 { kIdle, kLink, kList, kCall };
	struct Script {
		VarContainer _enable;
		VarContainer _cmt;
		VarContainer _workSpace;
		LoopCond _loopCond;
		VarSrcMapForSimple _reVarIntro;
		VarSrcMapForSimple _reVarOutro;
		VarSrcMapForDefine _localVar;
		VarSrcMapForSimple _reVarBefore;
		VarSrcMapForSimple _reVarAfter;
		ScriptType _type;
		VarContainer _linkScript;
		ListOfInfo<Script> _listScript;
		StrList _callFunc;
		VarSrcMapForSimple _callParm;

		auto & cpy(Script const & src) {
			_enable = src._enable;
			_cmt = src._cmt;
			_workSpace = src._cmt;
			_loopCond = src._loopCond;
			_reVarIntro = src._reVarIntro;
			_reVarOutro = src._reVarOutro;
			_localVar = src._localVar;
			_reVarBefore = src._reVarBefore;
			_reVarAfter = src._reVarAfter;
			_type = src._type;
			_linkScript = src._linkScript;
			_listScript = src._listScript;
			_callFunc = src._callFunc;
			_callParm = src._callParm;
			return *this;
		}
		auto & init() {
			_enable.set(kTrue);
			_cmt.setNull();
			_workSpace.setNull();
			_loopCond.setOnce();
			_reVarIntro.setNull();
			_reVarOutro.setNull();
			_localVar.setNull();
			_reVarBefore.setNull();
			_reVarAfter.setNull();
			_type = ScriptType::kIdle;
			_linkScript.setNull();
			_listScript.setNull();
			_callFunc.setNull();
			_callParm.setNull();
			return *this;
		}
		Script & read($JSON::Obj const & src) {
			init();
			if (src.exist("EnAble")) {
				_enable = src["EnAble"];
			}
			if (src.exist("Cmt")) {
				_cmt = src["Cmt"];
			}
			if (src.exist("WorkSpace")) {
				_workSpace = src["WorkSpace"];
			}
			if (src.exist("Loop")) {
				_loopCond = src["Loop"].getArr();
			}
			if (src.exist("ReVarIntro")) {
				_reVarIntro = src["ReVarIntro"].getObj();
			}
			if (src.exist("ReVarOutro")) {
				_reVarOutro = src["ReVarOutro"].getObj();
			}
			if (src.exist("Var")) {
				_localVar = src["Var"].getObj();
			}
			if (src.exist("ReVarBefore")) {
				_reVarBefore = src["ReVarBefore"].getObj();
			}
			if (src.exist("ReVarAfter")) {
				_reVarAfter = src["ReVarAfter"].getObj();
			}
			if (src.exist("Type")) {
				auto & type = src["Type"].getStr();
				if (type == "Idle") {
					_type = ScriptType::kIdle;
				}
				elif(type == "Link") {
					_type = ScriptType::kLink;
					_linkScript = src["Script"];
				}
				elif(type == "List") {
					_type = ScriptType::kList;
					_listScript.read(src["Task"].getArr());
				}
				elif(type == "Call") {
					_type = ScriptType::kCall;
					$JSON::getArrStr(src["Func"].getArr(), _callFunc);
					_callParm = src["Parm"].getObj();
				} else
					_type = ScriptType::kIdle;
			}
			return *this;
		}

		auto & operator=(Script const & src) { return cpy(src); }
	};

	ListOfInfo<Script> & getScriptListFromJSON($JSON::Val const & src, ListOfInfo<Script> & dst);

	inline auto getScriptListFromJSON(Str const & src, ListOfInfo<Script> & dst) {
		return getScriptListFromJSON($JSON::read(src, $JSON::Val().setNull()), dst);
	}
	inline auto getScriptListFromJSON(Path const & src, ListOfInfo<Script> & dst) {
		return getScriptListFromJSON($JSON::read(src, $JSON::Val().setNull()), dst);
	}


	struct QEOption {
		ListP<StrList> _filter;
		Map<Str, Str> _tip;
		ListOfInfo<Script> _script;

		auto isValidPath(Path const & path, Bool const & ignoreSuffixCase) const {
			auto result = kFalse;
			for_criter(oneFilter, _filter) {
				auto curIsValid = kTrue;
				for_criter(filterStr, oneFilter) {
					StrRef cmd, arg;
					cmd.set(filterStr, kThFirst, filterStr.find(':'));
					arg.set(filterStr, filterStr.find(':') + kSzOne);
					auto thisFilterIsValid = kFalse;
					if (cmd.isEmpty())
						thisFilterIsValid = kTrue;
					elif(cmd == "file")
						thisFilterIsValid = $DevUtil::isFile(path);
					elif(cmd == "dir")
						thisFilterIsValid = $DevUtil::isDir(path);
					elif(cmd == "suffix")
						thisFilterIsValid = arg.cmp("*") || (ignoreSuffixCase ? path.cmpSuffix(arg) : path.cmpSuffixWithCase(arg));
					elif(cmd == "substr")
						thisFilterIsValid = path.getLast().existSub(arg);
					curIsValid &= thisFilterIsValid;
				}
				result |= curIsValid;
			}
			return result;
		}

		auto & cpy(QEOption const & src) {
			_filter = src._filter;
			_tip = src._tip;
			_script = src._script;
			return *this;
		}
		auto & init() {
			_filter.setNull();
			_tip.setNull();
			_script.setNull();
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			init();
			if (src.exist("Filter")) {
				for_criter(e, src["Filter"].getArr()) {
					_filter.add();
					for_criter(ee, e.getArr()) {
						_filter.getLast().add(ee.getStr());
					}
				}
			}
			if (src.exist("Tip")) {
				for_criter(e, src["Tip"].getObj())
					_tip.addByKV(e._key.get(), e._val.getStr());
			}
			if (src.exist("Script")) {
				getScriptListFromJSON(src["Script"], _script);
			}
			return *this;
		}

		auto & operator=(QEOption const & src) { return cpy(src); }
	};
	using QEOptionGroup = MapOfInfo<QEOption>;
	using QEOptionGroupMap = MapOfInfo<QEOptionGroup>;

	struct QuickEnterConfig {
		Bool _ignoreFilter;
		Bool _ignoreSuffixCase;
		Bool _enableBatchMode;
		Bool _forceBatchIfDir;
		QEOptionGroupMap _option;
		QEOptionGroupMap _optionForBatch;

		auto & init() {
			_ignoreFilter = kFalse;
			_ignoreSuffixCase = kFalse;
			_enableBatchMode = kFalse;
			_forceBatchIfDir = kFalse;
			_option.setNull();
			_optionForBatch.setNull();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			init();
			if (src.exist("IgnoreFilter")) {
				_ignoreFilter = src["IgnoreFilter"].getBool();
			}
			if (src.exist("IgnoreSuffixCase")) {
				_ignoreSuffixCase = src["IgnoreSuffixCase"].getBool();
			}
			if (src.exist("EnableBatchMode")) {
				_enableBatchMode = src["EnableBatchMode"].getBool();
			}
			if (src.exist("ForceBatchIfDir")) {
				_forceBatchIfDir = src["ForceBatchIfDir"].getBool();
			}
			if (src.exist("Option")) {
				_option.read(src["Option"].getObj());
			}
			if (src.exist("BatchOption")) {
				_optionForBatch.read(src["BatchOption"].getObj());
			}
			return *this;
		}
	};

	struct LogConfig {
		Bool _scriptListProg;
		Bool _scriptLoopProg;
		Bool _scriptTimer;
		Bool _funcTimer;
		Bool _funcName;
		Bool _varName;

		auto & cpy(LogConfig const & src) {
			memCpy(this, &src, kSzOne);
			return *this;
		}
		auto & init() {
			memSet(this, kSzOne);
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			init();
			if (src.exist("ScriptListProg")) {
				_scriptListProg = src["ScriptListProg"].getBool();
			}
			if (src.exist("ScriptLoopProg")) {
				_scriptLoopProg = src["ScriptLoopProg"].getBool();
			}
			if (src.exist("ScriptTimer")) {
				_scriptTimer = src["ScriptTimer"].getBool();
			}
			if (src.exist("FuncTimer")) {
				_funcTimer = src["FuncTimer"].getBool();
			}
			if (src.exist("FuncName")) {
				_funcName = src["FuncName"].getBool();
			}
			if (src.exist("VarName")) {
				_varName = src["VarName"].getBool();
			}
			return *this;
		}

		LogConfig() {
			init();
			return;
		}
	};
	struct Config {
		LogConfig _logConfig;
		FuncTmplList _func;
		VarSrcMapForDefine _globalVar;
		ListOfInfo<Script> _introScript;
		ListOfInfo<Script> _outroScript;
		QuickEnterConfig _quickEnterConfig;

		auto & init() {
			_logConfig.init();
			_func.setNull();
			_globalVar.setNull();
			_introScript.setNull();
			_outroScript.setNull();
			_quickEnterConfig.init();
			return *this;
		}
		auto & read($JSON::Obj const & src) {
			init();
			if (src.exist("LogConfig")) {
				_logConfig.read(src["LogConfig"].getObj());
			}
			if (src.exist("Func")) {
				_func.read(src["Func"].getArr());
			}
			if (src.exist("GlobalVar")) {
				_globalVar = src["GlobalVar"].getObj();
			}
			if (src.exist("IntroScript")) {
				_introScript.read(src["IntroScript"].getArr());
			}
			if (src.exist("OutroScript")) {
				_introScript.read(src["OutroScript"].getArr());
			}
			if (src.exist("QuickEnter")) {
				_quickEnterConfig.read(src["QuickEnter"].getObj());
			}
			return *this;
		}
	};

	tmpl_dt(TSrc, TDst) auto & CnvListPToB(ListP<TSrc> const & src, ListB<TDst> & dst) {
		dst.alloc(src._sz);
		for_criter(val, src)
			dst.add(val);
		return dst;
	}
}
