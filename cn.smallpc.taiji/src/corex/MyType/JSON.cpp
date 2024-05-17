# include "./JSON.h"

# include <list>

namespace $TJ::$JSON {

# if defined PLATFORM_ANDROID
	Val & Val::set(Obj const & val, Bool const & nullOrNot) {
		if (nullOrNot) return setNull();
		clear();
		_type = ValType::kObj;
		_val = new Obj(val);
		return *this;
	}
	Val & Val::set(Arr const & val, Bool const & nullOrNot) {
		if (nullOrNot) return setNull();
		clear();
		_type = ValType::kArr;
		_val = new Arr(val);
		return *this;
	}
# endif

	Val & Val::cpy(Val const & t) {
		clear();
		_type = t._type;
		switch (_type) {
			default: wErr("UnKnown ValType"); break;
			case ValType::kUndef: _val = kNullPtr; break;
			case ValType::kNull: _val = kNullPtr; break;
			case ValType::kObj: _val = new Obj(t.getObj()); break;
			case ValType::kArr: _val = new Arr(t.getArr()); break;
			case ValType::kBool: _val = new Bool(t.getBool()); break;
			case ValType::kUInt: _val = new UI64(t.getUInt()); break;
			case ValType::kSInt: _val = new SI64(t.getSInt()); break;
			case ValType::kFloat: _val = new F64(t.getFloat()); break;
			case ValType::kStr: _val = new Str(t.getStr()); break;
		}
		return *this;
	}

	Val & Val::set(ValType const & type) {
		_type = type;
		switch (type) {
			default: wErr("UnKnown ValType"); break;
			case ValType::kUndef: _val = kNullPtr; break;
			case ValType::kNull: _val = kNullPtr; break;
			case ValType::kObj: _val = new Obj(); break;
			case ValType::kArr: _val = new Arr(); break;
			case ValType::kBool: _val = new Bool(); break;
			case ValType::kUInt: _val = new UI64(); break;
			case ValType::kSInt: _val = new SI64(); break;
			case ValType::kFloat: _val = new F64(); break;
			case ValType::kStr: _val = new Str(); break;
		}
		return *this;
	}

	Bool Val::cmp(Val const & t) const {
		if (_type != t._type) return kFalse;
		Bool result;
		switch (_type) {
			default: wErr("UnKnown ValType"); result = kTrue; break;
			case ValType::kUndef: result = kTrue; break;
			case ValType::kNull: result = kTrue; break;
			case ValType::kObj: result = getObj() == t.getObj(); break;
			case ValType::kArr: result = getArr() == t.getArr(); break;
			case ValType::kBool: result = getBool() == t.getBool(); break;
			case ValType::kUInt: result = getUInt() == t.getUInt(); break;
			case ValType::kSInt: result = getSInt() == t.getSInt(); break;
			case ValType::kFloat: result = getFloat() == t.getFloat(); break;
			case ValType::kStr: result = getStr() == t.getStr(); break;
		}
		return result;
	}

	namespace $StyleCtrl {
		static Bool gOutNewLineIfElement(kTrue);

		Void setStyle(Bool const & outNewLineIfElement) {
			gOutNewLineIfElement = outNewLineIfElement;
			return;
		}
	}

	Val & Val::read(Ch * const & src, SzI & pos) {
		//wLogLn("val at %X", pos);
		Ch ch;
		auto willBreak = kFalse;
		while_nonstop{
			//wLogLn("is %c %X at %X", src[pos], src[pos], pos);
			switch (ch = src[pos++]) {
				default: wErr("unk v-ch %X at x%X", ch, pos - 1); break;
				case ' ': case '\t': case '\n': case '\r': break;
				case '{': willBreak = kTrue; setObj().getObj().read(src, pos); break;
				case '[': willBreak = kTrue; setArr().getArr().read(src, pos); break;
				case 'n':
					willBreak = kTrue;
					if (src[pos++] == 'u' && src[pos++] == 'l' && src[pos++] == 'l')
						setNull();
					else
						wErr("err json-null at %x", pos);
					break;
				case 'f':
					willBreak = kTrue;
					if (src[pos++] == 'a' && src[pos++] == 'l' && src[pos++] == 's' && src[pos++] == 'e')
						set(kFalse);
					else
						wErr("err json-false at %x", pos);
					break;
				case 't':
					willBreak = kTrue;
					if (src[pos++] == 'r' && src[pos++] == 'u' && src[pos++] == 'e')
						set(kTrue);
					else
						wErr("err json-true at %x", pos);
					break;
				case '/': $StrUtil::skipCmt(src, pos); break;
				case '\"': willBreak = kTrue; $StrUtil::readUTF8Str(src, pos, setStr().getStr()); break;
				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': {
					willBreak = kTrue;
					setUInt();
					auto type = $StrUtil::strToNum(src, --pos, _val);
					if (type == $StrUtil::NumType::kSInt)
						_type = ValType::kSInt;
					elif(type == $StrUtil::NumType::kFloat)
						_type = ValType::kFloat;
					break;
				}
			}
			if (willBreak)
				break;
		}
		return *this;
	}
	Void Val::write(Ch * const & dst, SzI & pos, SzI tabLv) const {
		switch (_type) {
			case ValType::kUndef:
				wWarn("write undef val");
				dst[pos++] = 'u'; dst[pos++] = 'n'; dst[pos++] = 'd'; dst[pos++] = 'e'; dst[pos++] = 'f'; dst[pos++] = 'i'; dst[pos++] = 'n'; dst[pos++] = 'e'; dst[pos++] = 'd';
				break;
			case ValType::kNull:
				dst[pos++] = 'n'; dst[pos++] = 'u'; dst[pos++] = 'l'; dst[pos++] = 'l';
				break;
			case ValType::kObj: getObj().write(dst, pos, tabLv); break;
			case ValType::kArr: getArr().write(dst, pos, tabLv); break;
			case ValType::kBool:
				if (getBool()) {
					dst[pos++] = 't'; dst[pos++] = 'r'; dst[pos++] = 'u'; dst[pos++] = 'e';
				} else {
					dst[pos++] = 'f'; dst[pos++] = 'a'; dst[pos++] = 'l'; dst[pos++] = 's'; dst[pos++] = 'e';
				}
				break;
			case ValType::kUInt: $StrUtil::numToStr(getUInt(), dst, pos); break;
			case ValType::kSInt: $StrUtil::numToStr(getSInt(), dst, pos); break;
			case ValType::kFloat: $StrUtil::numToStr(getFloat(), dst, pos); break;
			case ValType::kStr:
				dst[pos++] = '\"';
				$StrUtil::writeUTF8Str(getStr(), dst, pos);
				dst[pos++] = '\"';
				break;
		}
		return;
	}

	Key & Key::read(Ch * const & src, SzI & pos) {
		//wLogLn("key at %X", pos);
		Ch ch;
		auto willBreak = kFalse;
		while_nonstop{
			switch (ch = src[pos++]) {
				default: wErr("unk k-ch %X at x%X", ch, pos - 1); break;
				case ' ': case '\t': case '\n': case '\r': break;
				case '/': $StrUtil::skipCmt(src, pos); break;
				case '\"': willBreak = kTrue; $StrUtil::readUTF8Str(src, pos, setStr().getStr()); break;
			}
			if (willBreak)
				break;
		}
		return *this;
	}
	Void Key::write(Ch * const & dst, SzI & pos) const {
		dst[pos++] = '\"';
		$StrUtil::writeUTF8Str(get(), dst, pos);
		dst[pos++] = '\"';
		return;
	}

	Arr & Arr::read(Ch * const & src, SzI & pos) {
		//wLogLn("arr at %X", pos);
		std::list<Val *> vlist;
		auto cnt = kSzNone;
		auto shouldEnd = kFalse;
		while_nonstop{
			//wLogLn("is %c %X at %X", src[pos], src[pos], pos);
			if (src[pos] == ']') {
				++pos; break;
			} elif(src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n' || src[pos] == '\r') {
				++pos; continue;
			} elif(src[pos] == '/') {
				++pos; $StrUtil::skipCmt(src, pos);
			} else {
				vlist.push_back(new Val());
				(*vlist.back()).read(src, pos);
				if (src[pos] == ',') {
					++pos;
					shouldEnd = kFalse;
				} else {
					//wLogLn("arrend");
					shouldEnd = kTrue;
				}
				++cnt;
			}
		}
		alloc(cnt);
		for_criter(v, vlist)
			getListB().append(v);
		if (_sz > kSzNone && !shouldEnd)
			wErr("err json-arr at %x", pos);
		return *this;
	}
	Void Arr::write(Ch * const & dst, SzI & pos, SzI tabLv) const {
		dst[pos++] = '[';
		if (_sz > kSzNone) {
			++tabLv;
			dst[pos++] = $StyleCtrl::gOutNewLineIfElement ? ' ' : '\n';
			for_cnt(idx, kThFirst, _sz) {
				if (!$StyleCtrl::gOutNewLineIfElement)
					pos += $StrUtil::putChN('\t', tabLv, dst + pos);
				get(idx).write(dst, pos, tabLv);
				if (idx < lastIdx())
					dst[pos++] = ',';
				dst[pos++] = $StyleCtrl::gOutNewLineIfElement ? ' ' : '\n';
			}
			--tabLv;
			if (!$StyleCtrl::gOutNewLineIfElement)
				pos += $StrUtil::putChN('\t', tabLv, dst + pos);
		}
		dst[pos++] = ']';
		return;
	}

	Obj & Obj::read(Ch * const & src, SzI & pos) {
		//wLogLn("obj at %X", pos);
		std::list<Member *> vlist;
		auto cnt = kSzZero;
		auto shouldEnd = kFalse;
		while_nonstop{
			//wLogLn("%X x%X", src[pos], pos);
			if (src[pos] == '}') {
				++pos; break;
			} elif(src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n' || src[pos] == '\r') {
				++pos; continue;
			} elif(src[pos] == '/') {
				++pos; $StrUtil::skipCmt(src, pos);
			} else {
				vlist.push_back(new Member());
				(*vlist.back())._key.read(src, pos);
				if (src[pos++] != ':')
					wErr("err json-key at %x", pos - 1);
				(*vlist.back())._val.read(src, pos);
				if (src[pos] == ',') {
					++pos;
					shouldEnd = kFalse;
				} else {
					//wLogLn("objend");
					shouldEnd = kTrue;
				}
				++cnt;
			}
		}
		alloc(cnt);
		for_criter(v, vlist)
			getListB().append(v);
		if (_sz > kSzNone && !shouldEnd)
			wErr("err json-arr at %x", pos);
		return *this;
	}
	Void Obj::write(Ch * const & dst, SzI & pos, SzI tabLv) const {
		dst[pos++] = '{';
		if (_sz > kSzNone) {
			dst[pos++] = '\n';
			++tabLv;
			for_cnt(idx, kThFirst, _sz) {
				pos += $StrUtil::putChN('\t', tabLv, dst + pos);
				get(idx)._key.write(dst, pos);
				dst[pos++] = ':';
				dst[pos++] = ' ';
				get(idx)._val.write(dst, pos, tabLv);
				if (idx < lastIdx())
					dst[pos++] = ',';
				dst[pos++] = '\n';
			}
			--tabLv;
			pos += $StrUtil::putChN('\t', tabLv, dst + pos);
		}
		dst[pos++] = '}';
		return;
	}

	$JSON::Val & eraseUndef($JSON::Val & t) {
		if (t.isTypeArr()) {
			for_cnt_lt(i, t.getArr()._sz, kThFirst) {
				auto & e = t.getArr()[i - kThSecond];
				if (e.isTypeUndef())
					t.getArr().eraseOne(i - kThSecond);
				elif(e.isTypeArr() || e.isTypeObj())
					eraseUndef(e);
			}
		} elif(t.isTypeObj()) {
			for_cnt_lt(i, t.getObj()._sz, kThFirst) {
				auto & e = t.getObj()[i - kThSecond]._val;
				if (e.isTypeUndef())
					t.getObj().eraseOne(i - kThSecond);
				elif(e.isTypeArr() || e.isTypeObj())
					eraseUndef(e);
			}
		}
		return t;
	}

}
