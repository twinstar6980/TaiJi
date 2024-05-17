#include "./JSON.hpp"

#include "../Util/File/File.hpp"

#include <list>

namespace $TJ::$JSON {

#if defined PLATFORM_ANDROID

	Val & Val::set(Obj const & t, Bool const & forceNull) {
		if (forceNull)
			return setNull();
		clear();
		$ty = ValType::kObj;
		$v = new Obj(t);
		return thiz;
	}
	Val & Val::set(Arr const & t, Bool const & forceNull) {
		if (forceNull)
			return setNull();
		clear();
		$ty = ValType::kArr;
		$v = new Arr(t);
		return thiz;
	}

#endif

	Val & Val::set(ValType const & type) {
		clear();
		$ty = type;
		switch (type) {
			default:
				throw VAMGException("JSON", "unknown value-type when set");
				break;
			case ValType::kUndef:
				$v = kNullPtr;
				break;
			case ValType::kNull:
				$v = kNullPtr;
				break;
			case ValType::kBool:
				$v = new Bool();
				break;
			case ValType::kIntU:
				$v = new IU64();
				break;
			case ValType::kIntS:
				$v = new IS64();
				break;
			case ValType::kFloat:
				$v = new F64();
				break;
			case ValType::kStr:
				$v = new Str();
				break;
			case ValType::kArr:
				$v = new Arr();
				break;
			case ValType::kObj:
				$v = new Obj();
				break;
		}
		return thiz;
	}

	Val & Val::clear() {
		switch ($ty) {
			default:
				throw VAMGException("JSON", "unknown value-type when clear");
				break;
			case ValType::kUndef:
				$v = kNullPtr;
				break;
			case ValType::kNull:
				$v = kNullPtr;
				break;
			case ValType::kBool:
				pvFreeS<Bool>($v);
				break;
			case ValType::kIntU:
				pvFreeS<IU64>($v);
				break;
			case ValType::kIntS:
				pvFreeS<IS64>($v);
				break;
			case ValType::kFloat:
				pvFreeS<F64>($v);
				break;
			case ValType::kStr:
				pvFreeS<Str>($v);
				break;
			case ValType::kArr:
				pvFreeS<Arr>($v);
				break;
			case ValType::kObj:
				pvFreeS<Obj>($v);
				break;
		}
		return thiz;
	}


	Val & Val::cpy(Val const & t) {
		clear();
		$ty = t.$ty;
		switch (t.$ty) {
			default:
				throw VAMGException("JSON", "unknown value-type when copy");
				break;
			case ValType::kUndef:
				$v = kNullPtr;
				break;
			case ValType::kNull:
				$v = kNullPtr;
				break;
			case ValType::kBool:
				$v = new Bool(t.getBool());
				break;
			case ValType::kIntU:
				$v = new IU64(t.getIntU());
				break;
			case ValType::kIntS:
				$v = new IS64(t.getIntS());
				break;
			case ValType::kFloat:
				$v = new F64(t.getFloat());
				break;
			case ValType::kStr:
				$v = new Str(t.getStr());
				break;
			case ValType::kArr:
				$v = new Arr(t.getArr());
				break;
			case ValType::kObj:
				$v = new Obj(t.getObj());
				break;
		}
		return thiz;
	}

	Bool Val::cmp(Val const & t) const {
		if ($ty != t.$ty)
			return kBoF;
		Bool result;
		switch ($ty) {
			default:
				throw VAMGException("JSON", "unknown value-type when compare");
				result = kBoT;
				break;
			case ValType::kUndef:
				result = kBoT;
				break;
			case ValType::kNull:
				result = kBoT;
				break;
			case ValType::kBool:
				result = getBool() == t.getBool();
				break;
			case ValType::kIntU:
				result = getIntU() == t.getIntU();
				break;
			case ValType::kIntS:
				result = getIntS() == t.getIntS();
				break;
			case ValType::kFloat:
				result = getFloat() == t.getFloat();
				break;
			case ValType::kStr:
				result = getStr() == t.getStr();
				break;
			case ValType::kArr:
				result = getArr() == t.getArr();
				break;
			case ValType::kObj:
				result = getObj() == t.getObj();
				break;
		}
		return result;
	}

	namespace $Style {

		static Bool gNoLineFeedWhenElement(kBoT);

		Void set(Bool const & noLineFeedWhenElement) {
			gNoLineFeedWhenElement = noLineFeedWhenElement;
			return;
		}

	}

	Void Val::read(ConstCStr const & src, Sz & pos) {
		//log.txtln("val at %X", pos);
		Ch ch;
		auto willBreak = kBoF;
		while (kBoT) {
			//log.txtln("is %c %X at %X", src[pos], src[pos], pos);
			switch (ch = src[pos++]) {
				default:
					throw VAMGException("JSON-Reader", "[0x%X] error value-begin-character : 0x%02X", pos - kNo2, ch);
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					break;
				case '/':
					pos += $Str::cntCmtLen(src + pos);
					break;
				case 'n':
					willBreak = kBoT;
					if (src[pos++] == 'u' && src[pos++] == 'l' && src[pos++] == 'l')
						setNull();
					else
						throw VAMGException("JSON-Reader", "[0x%X] error value null", pos);
					break;
				case 'f':
					willBreak = kBoT;
					if (src[pos++] == 'a' && src[pos++] == 'l' && src[pos++] == 's' && src[pos++] == 'e')
						set(kBoF);
					else
						throw VAMGException("JSON-Reader", "[0x%X] error value false", pos);
					break;
				case 't':
					willBreak = kBoT;
					if (src[pos++] == 'r' && src[pos++] == 'u' && src[pos++] == 'e')
						set(kBoT);
					else
						throw VAMGException("JSON-Reader", "[0x%X] error value true", pos);
					break;
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
					willBreak = kBoT;
					setIntU();
					$Str::NumType type;
					--pos;
					pos += $Str::strToNum(src + pos, $v, type);
					if (type == $Str::NumType::kSInt)
						$ty = ValType::kIntS;
					else if (type == $Str::NumType::kFloat)
						$ty = ValType::kFloat;
					break;
				}
				case '\"':
					willBreak = kBoT;
					pos += $Str::readUTF8Str(src + pos, setStr().getStr());
					break;
				case '[':
					willBreak = kBoT;
					setArr().getArr().read(src, pos);
					break;
				case '{':
					willBreak = kBoT;
					setObj().getObj().read(src, pos);
					break;
			}
			if (willBreak)
				break;
		}
		return;
	}
	Void Val::write(CStr const & dst, Sz & pos, Sz indent) const {
		switch ($ty) {
			case ValType::kUndef:
				log.war("write undef val");
				dst[pos++] = 'u';
				dst[pos++] = 'n';
				dst[pos++] = 'd';
				dst[pos++] = 'e';
				dst[pos++] = 'f';
				dst[pos++] = 'i';
				dst[pos++] = 'n';
				dst[pos++] = 'e';
				dst[pos++] = 'd';
				break;
			case ValType::kNull:
				dst[pos++] = 'n';
				dst[pos++] = 'u';
				dst[pos++] = 'l';
				dst[pos++] = 'l';
				break;
			case ValType::kBool:
				if (getBool()) {
					dst[pos++] = 't';
					dst[pos++] = 'r';
					dst[pos++] = 'u';
					dst[pos++] = 'e';
				} else {
					dst[pos++] = 'f';
					dst[pos++] = 'a';
					dst[pos++] = 'l';
					dst[pos++] = 's';
					dst[pos++] = 'e';
				}
				break;
			case ValType::kIntU:
				pos += $Str::numToStr(getIntU(), dst + pos);
				break;
			case ValType::kIntS:
				pos += $Str::numToStr(getIntS(), dst + pos);
				break;
			case ValType::kFloat:
				pos += $Str::numToStr(getFloat(), dst + pos);
				break;
			case ValType::kStr:
				dst[pos++] = '\"';
				pos += $Str::writeUTF8Str(getStr(), dst + pos);
				dst[pos++] = '\"';
				break;
			case ValType::kArr:
				getArr().write(dst, pos, indent);
				break;
			case ValType::kObj:
				getObj().write(dst, pos, indent);
				break;
		}
		return;
	}

	Void Arr::read(ConstCStr const & src, Sz & pos) {
		//log.txtln("arr at %X", pos);
		std::list<Val> tmp;
		auto cnt = kSzN;
		auto shouldEnd = kBoF;
		while (kBoT) {
			//log.txtln("is %c %X at %X", src[pos], src[pos], pos);
			if (src[pos] == ']') {
				++pos;
				break;
			} else if (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n' || src[pos] == '\r') {
				++pos;
				continue;
			} else if (src[pos] == '/') {
				++pos;
				pos += $Str::cntCmtLen(src + pos);
			} else {
				tmp.push_back(Val());
				tmp.back().read(src, pos);
				if (src[pos] == ',') {
					++pos;
					shouldEnd = kBoF;
				} else {
					//log.txtln("arrend");
					shouldEnd = kBoT;
				}
				++cnt;
			}
		}
		alloc(cnt);
		for (auto const & v : tmp)
			TS::append(v);
		if ($sz > kSzN && !shouldEnd)
			throw VAMGException("JSON-Reader", "[0x%X] array's last value shouldn't append ',' character", pos);
		return;
	}
	Void Arr::write(CStr const & dst, Sz & pos, Sz indent) const {
		dst[pos++] = '[';
		if ($sz > kSzN) {
			++indent;
			dst[pos++] = $Style::gNoLineFeedWhenElement ? ' ' : '\n';
			range(kNo1, $sz - kNo2).each(
				[&dst, &pos, &indent](auto & v) {
					if (!$Style::gNoLineFeedWhenElement)
						pos += $Str::putChN('\t', indent, dst + pos);
					v.write(dst, pos, indent);
					dst[pos++] = ',';
					dst[pos++] = $Style::gNoLineFeedWhenElement ? ' ' : '\n';
				}
			);
			{
				if (!$Style::gNoLineFeedWhenElement)
					pos += $Str::putChN('\t', indent, dst + pos);
				last().write(dst, pos, indent);
				dst[pos++] = $Style::gNoLineFeedWhenElement ? ' ' : '\n';
			}
			--indent;
			if (!$Style::gNoLineFeedWhenElement)
				pos += $Str::putChN('\t', indent, dst + pos);
		}
		dst[pos++] = ']';
		return;
	}

	Void Obj::read(ConstCStr const & src, Sz & pos) {
		//log.txtln("obj at %X", pos);
		std::list<Member> tmp;
		auto cnt = kSzN;
		auto shouldEnd = kBoF;
		while (kBoT) {
			//log.txtln("%X x%X", src[pos], pos);
			if (src[pos] == '}') {
				++pos;
				break;
			} else if (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n' || src[pos] == '\r') {
				++pos;
				continue;
			} else if (src[pos] == '/') {
				++pos;
				pos += $Str::cntCmtLen(src + pos);
			} else {
				tmp.push_back(Member());
				tmp.back().$k.read(src, pos);
				if (src[pos++] != ':')
					throw VAMGException("JSON-Reader", "[0x%X] error key", pos - kNo2);
				tmp.back().$v.read(src, pos);
				if (src[pos] == ',') {
					++pos;
					shouldEnd = kBoF;
				} else {
					//log.txtln("objend");
					shouldEnd = kBoT;
				}
				++cnt;
			}
		}
		alloc(cnt);
		for (auto const & v : tmp)
			TS::append(v);
		if ($sz > kSzN && !shouldEnd)
			throw VAMGException("JSON-Reader", "[0x%X] object's last member shouldn't append ',' character", pos);
		return;
	}
	Void Obj::write(CStr const & dst, Sz & pos, Sz indent) const {
		dst[pos++] = '{';
		if ($sz > kSzN) {
			dst[pos++] = '\n';
			++indent;
			range(kNo1, $sz - kNo2).each(
				[&dst, &pos, &indent](auto & v) {
					pos += $Str::putChN('\t', indent, dst + pos);
					v.$k.write(dst, pos);
					dst[pos++] = ':';
					dst[pos++] = ' ';
					v.$v.write(dst, pos, indent);
					dst[pos++] = ',';
					dst[pos++] = '\n';
				}
			);
			{
				pos += $Str::putChN('\t', indent, dst + pos);
				last().$k.write(dst, pos);
				dst[pos++] = ':';
				dst[pos++] = ' ';
				last().$v.write(dst, pos, indent);
				dst[pos++] = '\n';
			}
			--indent;
			pos += $Str::putChN('\t', indent, dst + pos);
		}
		dst[pos++] = '}';
		return;
	}

	namespace $Tool {

		Val & eraseUndef(Val const & src, Val & dst) {
			if (src.isArr()) {
				auto & t = dst.setArr().getArr().alloc(src.getArr().$sz);
				src.getArr().range().each(
					[&t](auto & v) {
						if (!v.isUndef()) {
							if (v.isArr() || v.isObj()) {
								eraseUndef(v, t.appendX().last());
							} else {
								t.appendX().last() = v;
							}
						}
					}
				);
			} else if (src.isObj()) {
				auto & t = dst.setObj().getObj().alloc(src.getObj().$sz);
				src.getObj().range().each(
					[&t](auto & v) {
						if (!v.$v.isUndef()) {
							if (v.$v.isArr() || v.$v.isObj()) {
								eraseUndef(v.$v, t.appendX().last().setKey(v.$k).$v);
							} else {
								t.appendX().last() = v;
							}
						}
					}
				);
			}
			return dst;
		}

		static Ch * gJSONIOMem;
		static Sz gJSONIOMemSz(kSzN);

		namespace $IOMem {

			Void free() {
				pFreeN(gJSONIOMem);
				gJSONIOMemSz = kSzN;
				return;
			}
			Void realloc(Sz const & sz) {
				pFreeN(gJSONIOMem);
				pAlloc(gJSONIOMem, sz);
				gJSONIOMemSz = sz;
				return;
			}

		}

		Val & read(Str const & src, Val & dst) {
			auto pos = kNo1;
			dst.read(src.$data, pos);
			return dst;
		}
		Obj & read(Str const & src, Obj & dst) {
			auto pos = kNo2;
			dst.read(src.$data, pos);
			return dst;
		}
		Arr & read(Str const & src, Arr & dst) {
			auto pos = kNo2;
			dst.read(src.$data, pos);
			return dst;
		}

		Str & write(Str & dst, Val const & src) {
			auto len = kSz0;
			src.write(gJSONIOMem, len);
			dst.cpy(Str().ref(gJSONIOMem, len));
			return dst;
		}
		Str & write(Str & dst, Obj const & src) {
			auto len = kSz0;
			src.write(gJSONIOMem, len);
			dst.cpy(Str().ref(gJSONIOMem, len));
			return dst;
		}
		Str & write(Str & dst, Arr const & src) {
			auto len = kSz0;
			src.write(gJSONIOMem, len);
			dst.cpy(Str().ref(gJSONIOMem, len));
			return dst;
		}

		Val & read(Path const & src, Val & dst) {
			$Util::$File::readFile(src, ByteSection().ref(reinterpret_cast<Byte *&>(gJSONIOMem), kSzN, gJSONIOMemSz));
			auto pos = kNo1;
			dst.read(gJSONIOMem, pos);
			return dst;
		}
		Obj & read(Path const & src, Obj & dst) {
			$Util::$File::readFile(src, ByteSection().ref(reinterpret_cast<Byte *&>(gJSONIOMem), kSzN, gJSONIOMemSz));
			auto pos = kNo2;
			dst.read(gJSONIOMem, pos);
			return dst;
		}
		Arr & read(Path const & src, Arr & dst) {
			$Util::$File::readFile(src, ByteSection().ref(reinterpret_cast<Byte *&>(gJSONIOMem), kSzN, gJSONIOMemSz));
			auto pos = kNo2;
			dst.read(gJSONIOMem, pos);
			return dst;
		}

		Void write(Path const & dst, Val const & src) {
			auto len = kSz0;
			src.write(gJSONIOMem, len);
			$Util::$File::writeFile(dst, ByteSection().ref(reinterpret_cast<Byte const * const &>(gJSONIOMem), len));
			return;
		}
		Void write(Path const & dst, Obj const & src) {
			auto len = kSz0;
			src.write(gJSONIOMem, len);
			$Util::$File::writeFile(dst, ByteSection().ref(reinterpret_cast<Byte const * const &>(gJSONIOMem), len));
			return;
		}
		Void write(Path const & dst, Arr const & src) {
			auto len = kSz0;
			src.write(gJSONIOMem, len);
			$Util::$File::writeFile(dst, ByteSection().ref(reinterpret_cast<Byte const * const &>(gJSONIOMem), len));
			return;
		}

	}

}
