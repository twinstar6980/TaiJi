# pragma once

# include "../../Type.h"
# include "../../MyType/JSON.h"

# include "../../MyType/ListOfOther.h"
# include "../../MyType/StrList.h"
# include "../../MyType/Path.h"
# include "../../ImageUtil/ImageType.h"

# include "./Struct.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$StructInfo {
	constexpr F64 kRateOfAnimOri(100.0);
	constexpr F64 kRateOfPosOri(20.0);
	constexpr F64 kRateOfScaleOri(20.0 * 65536.0);
	constexpr F64 kRateOfColorRateOri(255.0);
	constexpr F64 kRateOfMatrixOri(65536.0);
	constexpr F64 kRateOfRotateOri(1000.0);

	struct ImageInfo {
		Str _id, _src;
		$ImageUtil::ImageSize _sz;
		F64 _posX, _posY;
		F64 _scaleX, _scaleY;

		auto & read($JSON::Obj const & src) {
			src["ID"].getX(_id);
			src["Src"].getX(_src);
			_sz.read(src["Size"].getArr());
			src["Pos"][kThFirst].getX(_posX);
			src["Pos"][kThSecond].getX(_posY);
			src["Scale"][kThFirst].getX(_scaleX);
			src["Scale"][kThSecond].getX(_scaleY);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("ID", _id);
			dst.add("Src", _src);
			_sz.write(dst.addArr("Size").last()._val.getArr());
			dst.addArr("Pos").last()._val.getArr().add(_posX).add(_posY);
			dst.addArr("Scale").last()._val.getArr().add(_scaleX).add(_scaleY);
			return dst;
		}
		auto & parse($Struct::ImageInfoWithStr const & src) {
			auto idxOfDelimiter = src._str._str.find('|');
			_id.cpy(src._str._str, idxOfDelimiter + kThSecond, src._str._str._len - idxOfDelimiter - kThSecond);
			_src.cpy(src._str._str, kThFirst, idxOfDelimiter);
			_sz._w = src._info._w;
			_sz._h = src._info._h;
			_posX = static_cast<F64>(src._info._posX) / kRateOfPosOri;
			_posY = static_cast<F64>(src._info._posY) / kRateOfPosOri;
			_scaleX = static_cast<F64>(src._info._scaleX) / kRateOfScaleOri;
			_scaleY = static_cast<F64>(src._info._scaleY) / kRateOfScaleOri;
			return *this;
		}
		auto & parseTo($Struct::ImageInfoWithStr & dst) const {
			dst._str._str.cat(_src).cat('|').cat(_id);
			dst._info._w = _sz._w;
			dst._info._h = _sz._h;
			dst._info._posX = _posX * kRateOfPosOri;
			dst._info._posY = _posY * kRateOfPosOri;
			dst._info._scaleX = _scaleX * kRateOfScaleOri;
			dst._info._scaleY = _scaleY * kRateOfScaleOri;
			return *this;
		}
	};
	namespace $LayerChgType {
		struct Pos {
			F64 _x, _y;

			auto & read($JSON::Arr const & src) {
				src[kThFirst].getX(_x);
				src[kThSecond].getX(_y);
				return *this;
			}
			auto & write($JSON::Arr & dst) const {
				dst.add(_x).add(_y);
				return dst;
			}
			auto & parse($Struct::$LayerChgType::$Pos::Member const & src) {
				_x = static_cast<F64>(src._x) / kRateOfPosOri;
				_y = static_cast<F64>(src._y) / kRateOfPosOri;
				return *this;
			}
			auto & parseTo($Struct::$LayerChgType::$Pos::Member & dst) const {
				dst._x = _x * kRateOfPosOri;
				dst._y = _y * kRateOfPosOri;
				return *this;
			}
		};
		struct ColorRate {
			F64 _r, _g, _b, _a;

			auto & read($JSON::Arr const & src) {
				src[kThFirst].getX(_r);
				src[kThSecond].getX(_g);
				src[kThThird].getX(_b);
				src[kThFourth].getX(_a);
				return *this;
			}
			auto & write($JSON::Arr & dst) const {
				dst.add(_r).add(_g).add(_b).add(_a);
				return dst;
			}
			auto & parse($Struct::$LayerChgType::$ColorRate::Member const & src) {
				_r = static_cast<F64>(src._r) / kRateOfColorRateOri;
				_g = static_cast<F64>(src._g) / kRateOfColorRateOri;
				_b = static_cast<F64>(src._b) / kRateOfColorRateOri;
				_a = static_cast<F64>(src._a) / kRateOfColorRateOri;
				return *this;
			}
			auto & parseTo($Struct::$LayerChgType::$ColorRate::Member & dst) const {
				dst._r = _r * kRateOfColorRateOri;
				dst._g = _g * kRateOfColorRateOri;
				dst._b = _b * kRateOfColorRateOri;
				dst._a = _a * kRateOfColorRateOri;
				return *this;
			}
		};
		struct Matrix {
			F64 _a, _b, _c, _d;

			auto & read($JSON::Arr const & src) {
				src[kThFirst].getX(_a);
				src[kThSecond].getX(_b);
				src[kThThird].getX(_c);
				src[kThFourth].getX(_d);
				return *this;
			}
			auto & write($JSON::Arr & dst) const {
				dst.add(_a).add(_b).add(_c).add(_d);
				return dst;
			}
			auto & parse($Struct::$LayerChgType::$Matrix::Member const & src) {
				_a = src._a / kRateOfMatrixOri;
				_b = src._b / kRateOfMatrixOri;
				_c = src._c / kRateOfMatrixOri;
				_d = src._d / kRateOfMatrixOri;
				return *this;
			}
			auto & parseTo($Struct::$LayerChgType::$Matrix::Member & dst) const {
				dst._a = _a * kRateOfMatrixOri;
				dst._b = _b * kRateOfMatrixOri;
				dst._c = _c * kRateOfMatrixOri;
				dst._d = _d * kRateOfMatrixOri;
				return *this;
			}
		};
		struct Rotate {
			F64 _angle;

			auto & read($JSON::Val const & src) {
				src.getX(_angle);
				return *this;
			}
			auto & write($JSON::Val & dst) const {
				dst.set(_angle);
				return dst;
			}
			auto & parse($Struct::$LayerChgType::$Rotate::Member const & src) {
				_angle = src._angle / kRateOfRotateOri;
				return *this;
			}
			auto & parseTo($Struct::$LayerChgType::$Rotate::Member & dst) const {
				dst._angle = _angle * kRateOfRotateOri;
				return *this;
			}
		};
	}
	struct LayerChg {
		SzI _idx;
		Bool _existU8;
		Bool _existAngle;
		Bool _existColorRate;
		Bool _existMatrix;
		Bool _existPos;
		Bool _existU3;
		$LayerChgType::Rotate _angle;
		$LayerChgType::ColorRate _colorRate;
		$LayerChgType::Matrix _matrix;
		$LayerChgType::Pos _pos;

		auto & read($JSON::Obj const & src) {
			src["Idx"].getX(_idx);
			if (_existPos = src.exist("Pos"))
				_pos.read(src["Pos"].getArr());
			if (_existColorRate = src.exist("ColorRate"))
				_colorRate.read(src["ColorRate"].getArr());
			if (_existAngle = src.exist("Rotate"))
				_angle.read(src["Rotate"]);
			if (_existMatrix = src.exist("Matrix"))
				_matrix.read(src["Matrix"].getArr());
			_existU8 = kFalse;
			_existU3 = kFalse;
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Idx", _idx);
			if (_existPos)
				_pos.write(dst.addArr("Pos").last()._val.getArr());
			if (_existColorRate)
				_colorRate.write(dst.addArr("ColorRate").last()._val.getArr());
			if (_existAngle)
				_angle.write(dst.add("Rotate").last()._val);
			if (_existMatrix)
				_matrix.write(dst.addArr("Matrix").last()._val.getArr());
			return dst;
		}
		auto & parse($Struct::LayerChg const & src) {
			_idx = src._idx;
			if (_existPos = src._existPos)
				_pos.parse(src._pos);
			if (_existColorRate = src._existColorRate)
				_colorRate.parse(src._colorRate);
			if (_existAngle = src._existAngle)
				_angle.parse(src._angle);
			if (_existMatrix = src._existMatrix)
				_matrix.parse(src._matrix);
			_existU8 = src._existU8;
			_existU3 = src._existU3;
			return *this;
		}
		auto & parseTo($Struct::LayerChg & dst) const {
			dst._idx = _idx;
			dst._existU8 = _existU8;
			dst._existPos = _existPos;
			dst._existColorRate = _existColorRate;
			dst._existAngle = _existAngle;
			dst._existMatrix = _existMatrix;
			dst._existU3 = _existU3;
			if (_existPos)
				_pos.parseTo(dst._pos);
			if (_existColorRate)
				_colorRate.parseTo(dst._colorRate);
			if (_existAngle)
				_angle.parseTo(dst._angle);
			if (_existMatrix)
				_matrix.parseTo(dst._matrix);
			return *this;
		}

		LayerChg() {
			_idx = -1;
			_existU8 = kFalse;
			_existAngle = kFalse;
			_existColorRate = kFalse;
			_existMatrix = kFalse;
			_existPos = kFalse;
			_existU3 = kFalse;
			_angle._angle = 0.0;
			_colorRate._r = 0.0;
			_colorRate._g = 0.0;
			_colorRate._b = 0.0;
			_colorRate._a = 0.0;
			_matrix._a = 0.0;
			_matrix._b = 0.0;
			_matrix._c = 0.0;
			_matrix._d = 0.0;
			_pos._x = 0.0;
			_pos._y = 0.0;
			return;
		}
	};

	struct LayerErase {
		SzI _idx;
		SzI _unKnown;

		auto & read($JSON::Obj const & src) {
			src["Idx"].getX(_idx);
			src["UnKnown"].getX(_unKnown);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Idx", _idx);
			dst.add("UnKnown", _unKnown);
			return dst;
		}
		auto & parse($Struct::LayerErase const & src) {
			_idx = src._idx;
			_unKnown = src._unKnown;
			return *this;
		}
		auto & parseTo($Struct::LayerErase & dst) const {
			dst._idx = _idx;
			dst._unKnown = _unKnown;
			return *this;
		}

		LayerErase() {
			_idx = -1;
			_unKnown = 0;
			return;
		}
	};
	struct LayerAppend {
		SzI _idx;
		SzI _refIdx;
		Bool _isAnim;
		Bool _u7;
		Bool _u6;
		Bool _existLabel;
		Bool _u4;
		Str _label;

		auto & read($JSON::Obj const & src) {
			src["Idx"].getX(_idx);
			src["RefIdx"].getX(_refIdx);
			src["IsAnim"].getX(_isAnim);
			if (_existLabel = src.exist("Label"))
				src["Label"].getX(_label);
			_u7 = _u6 = _u4 = kFalse;
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Idx", _idx);
			dst.add("RefIdx", _refIdx);
			dst.add("IsAnim", _isAnim);
			if (_existLabel)
				dst.add("Label", _label);
			return dst;
		}
		auto & parse($Struct::LayerAppend const & src) {
			_idx = src._idx;
			_refIdx = src._refIdx;
			_isAnim = src._isAnim;
			if (_existLabel = src._existLabel)
				_label.cpy(src._label._str);
			else _label.setNull();
			_u7 = src._u7;
			_u6 = src._u6;
			_u4 = src._u4;
			return *this;
		}
		auto & parseTo($Struct::LayerAppend & dst) const {
			dst._idx = _idx;
			dst._refIdx = _refIdx;
			dst._isAnim = _isAnim;
			dst._u7 = _u7;
			dst._u6 = _u6;
			dst._existLabel = _existLabel;
			dst._u4 = _u4;
			if (_existLabel)
				dst._label._str.cpy(_label);
			else dst._label._str.setNull();
			return *this;
		}

		LayerAppend() {
			_idx = -1;
			_refIdx = -1;
			_isAnim = kFalse;
			_u7 = kFalse;
			_u6 = kFalse;
			_existLabel = kFalse;
			_u4 = kFalse;
			_label.setNull();
			return;
		}
	};
	struct FSCmd {
		Str _cmd, _arg;

		auto & read($JSON::Arr const & src) {
			src[kThFirst].getX(_cmd);
			src[kThSecond].getX(_arg, kNullStr);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			dst.add(_cmd).add(_arg, _arg.isNull());
			return dst;
		}
		auto & parse($Struct::FSCmd const & src) {
			_cmd.cpy(src._cmd._str);
			if (src._arg._str.isEmpty())
				_arg.setNull();
			else _arg.cpy(src._arg._str);
			return *this;
		}
		auto & parseTo($Struct::FSCmd & dst) const {
			dst._cmd._str.cpy(_cmd);
			if (_arg.isNull())
				dst._arg._str.alloc(kSzZero);
			else dst._arg._str.cpy(_arg);
			return *this;
		}
	};
	struct Frame {
		Bool _existCmd;
		Bool _existStop;
		Bool _existLabel;
		Bool _existChg;
		Bool _existAppend;
		Bool _existErase;
		ListOfInfo<FSCmd> _cmd;
		Str _label;
		ListOfInfo<LayerChg> _chg;
		ListOfInfo<LayerAppend> _append;
		ListOfInfo<LayerErase> _erase;

		auto & tfree() {
			_existCmd = kFalse;
			_existStop = kFalse;
			_existLabel = kFalse;
			_existChg = kFalse;
			_existAppend = kFalse;
			_existErase = kFalse;
			_cmd.tfree();
			_label.tfree();
			_chg.tfree();
			_append.tfree();
			_erase.tfree();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			if (_existLabel = src.exist("Label"))
				_label.cpy(src["Label"].getStr());
			else _label.setNull();
			_existStop = src.exist("Stop") ? src["Stop"].getBool() : kFalse;
			if (_existCmd = src.exist("Cmd"))
				_cmd.readByArrStyle(src["Cmd"].getArr());
			else _cmd.setNull();
			if (_existErase = src.exist("erase"))
				_erase.read(src["erase"].getArr());
			else _erase.setNull();
			if (_existAppend = src.exist("Append"))
				_append.read(src["Append"].getArr());
			else _append.setNull();
			if (_existChg = src.exist("Chg"))
				_chg.read(src["Chg"].getArr());
			else _chg.setNull();
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			if (_existLabel)
				dst.add("Label", _label);
			if (_existStop)
				dst.add("Stop", kTrue);
			if (_existCmd)
				_cmd.writeByArrStyle(dst.addArr("Cmd").last()._val.getArr());
			if (_existErase)
				_erase.write(dst.addArr("erase").last()._val.getArr());
			if (_existAppend)
				_append.write(dst.addArr("Append").last()._val.getArr());
			if (_existChg)
				_chg.write(dst.addArr("Chg").last()._val.getArr());
			return dst;
		}
		auto & parse($Struct::Frame const & src) {
			if (_existLabel = src._existLabel)
				_label.cpy(src._label._str);
			else _label.setNull();
			_existStop = src._existStop;
			if (_existCmd = src._existCmd)
				_cmd.parse(src._cmd);
			else _cmd.setNull();
			if (_existErase = src._existErase)
				_erase.parse(src._erase);
			else _erase.setNull();
			if (_existAppend = src._existAppend)
				_append.parse(src._append);
			else _append.setNull();
			if (_existChg = src._existChg)
				_chg.parse(src._chg);
			else _chg.setNull();
			return *this;
		}
		auto & parseTo($Struct::Frame & dst) const {
			dst._existLabel = _existLabel;
			dst._existStop = _existStop;
			dst._existCmd = _existCmd;
			dst._existErase = _existErase;
			dst._existAppend = _existAppend;
			dst._existChg = _existChg;
			if (_existLabel)
				dst._label._str.cpy(_label);
			else dst._label._str.setNull();
			if (_existCmd)
				_cmd.parseTo(dst._cmd);
			else dst._cmd.setNull();
			if (_existErase)
				_erase.parseTo(dst._erase);
			else dst._erase.setNull();
			if (_existAppend)
				_append.parseTo(dst._append);
			else dst._append.setNull();
			if (_existChg)
				_chg.parseTo(dst._chg);
			else dst._chg.setNull();
			return *this;
		}

		Frame() {
			_existCmd = kFalse;
			_existStop = kFalse;
			_existLabel = kFalse;
			_existChg = kFalse;
			_existAppend = kFalse;
			_existErase = kFalse;
			_cmd.setNull();
			_label.setNull();
			_chg.setNull();
			_append.setNull();
			_erase.setNull();
			return;
		}
	};

	struct Type1AtAnimBegin { // 不知其内包含的数据为何种类型，暂定为BW2
		ListOfInfo<LayerErase> _v1, _v2;

		auto & tfree() {
			_v1.tfree();
			_v2.tfree();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			_v1.read(src["V1"].getArr());
			_v2.read(src["V2"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_v1.write(dst.addArr("V1").last()._val.getArr());
			_v2.write(dst.addArr("V2").last()._val.getArr());
			return dst;
		}
		auto & parse($Struct::Type1AtAnimBegin const & src) {
			_v1.parse(src._v1);
			_v2.parse(src._v2);
			return *this;
		}
		auto & parseTo($Struct::Type1AtAnimBegin & dst) const {
			_v1.parseTo(dst._v1);
			_v2.parseTo(dst._v2);
			return *this;
		}
	};
	struct AnimFrameInfo {
		SzI _rate;
		SzI _sz, _begin, _end;

		auto & setNull() {
			_rate = kSzZero;
			_sz = kSzZero;
			_begin = kThFirst;
			_end = kThFirst;
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			src["Rate"].getX(_rate);
			src["Sum"].getX(_sz);
			src["Range"][kThFirst].getX(_begin);
			src["Range"][kThSecond].getX(_end);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Rate", _rate);
			dst.add("Sum", _sz);
			dst.addArr("Range").last()._val.getArr().add(_begin).add(_end);
			return dst;
		}
		auto & parse($Struct::$AnimFrameInfo::Member const & src) {
			_rate = src._rate;
			_sz = src._sz;
			_begin = src._begin;
			_end = src._end;
			return *this;
		}
		auto & parseTo($Struct::$AnimFrameInfo::Member & dst) const {
			dst._rate = _rate;
			dst._sz = _sz;
			dst._begin = _begin;
			dst._end = _end;
			return *this;
		}
	};
	struct Anim {
		AnimFrameInfo _info;
		ListOfInfo<Frame> _frame;

		auto & tfree() {
			_info.setNull();
			_frame.tfree();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			_info.read(src["Info"].getObj());
			_frame.read(src["Frame"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_info.write(dst.addObj("Info").last()._val.getObj());
			_frame.write(dst.addArr("Frame").last()._val.getArr());
			return dst;
		}
		auto & parse($Struct::Anim const & src) {
			_info.parse(src._info);
			_frame.parse(src._frame);
			return *this;
		}
		auto & parseTo($Struct::Anim & dst) const {
			_info.parseTo(dst._info);
			_frame.parseTo(dst._frame);
			return *this;
		}

		protected:
		auto & getBase() { return *this; }
		auto & getBase() const { return *this; }

	};
	struct AnimWithLabel : Anim {
		Str _label;

		auto & tfree() {
			getBase().tfree();
			_label.setNull();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			src["Label"].getX(_label);
			getBase().read(src);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Label", _label);
			getBase().write(dst);
			return dst;
		}
		auto & parse($Struct::AnimWithLabel const & src) {
			_label.cpy(src._label._str);
			getBase().parse(src.getBase());
			return *this;
		}
		auto & parseTo($Struct::AnimWithLabel & dst) const {
			dst._label._str.cpy(_label);
			getBase().parseTo(dst.getBase());
			return *this;
		}
	};
	struct AnimWithUnk : Anim {
		Type1AtAnimBegin _block1;

		auto & tfree() {
			getBase().tfree();
			_block1.tfree();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			_block1.read(src["Block1"].getObj());
			getBase().read(src);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			_block1.write(dst.addObj("Block1").last()._val.getObj());
			getBase().write(dst);
			return dst;
		}
		auto & parse($Struct::AnimWithUnk const & src) {
			_block1.parse(src._block1);
			getBase().parse(src.getBase());
			return *this;
		}
		auto & parseTo($Struct::AnimWithUnk & dst) const {
			_block1.parseTo(dst._block1);
			getBase().parseTo(dst.getBase());
			return *this;
		}
	};
	struct Struct {
		Byte _unk[13];
		F64 _originX, _originY;
		ListOfInfo<ImageInfo> _image;
		ListOfInfo<AnimWithLabel> _subAnim;
		AnimWithUnk _mainAnim;

		auto & getThis() { return *this; }

		auto & tfree() {
			_image.setNull();
			_subAnim.setNull();
			_mainAnim.tfree();
			return *this;
		}

		auto & read($JSON::Obj const & src) {
			auto unkSz = kSzZero;
			readHexStr(src["UnK"].getStr(), _unk, unkSz);
			if (unkSz != 13) {
				// todo
			}
			src["Origin"][kThFirst].getX(_originX);
			src["Origin"][kThSecond].getX(_originY);
			_image.read(src["Image"].getArr());
			_subAnim.read(src["SubAnim"].getArr());
			_mainAnim.read(src["MainAnim"].getObj());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			writeHexStr(_unk, 13, dst.add("UnK", $JSON::ValType::kStr).last()._val.getStr());
			dst.addArr("Origin").last()._val.getArr().add(_originX).add(_originY);
			_image.write(dst.addArr("Image").last()._val.getArr());
			_subAnim.write(dst.addArr("SubAnim").last()._val.getArr());
			_mainAnim.write(dst.addObj("MainAnim").last()._val.getObj());
			return dst;
		}
		auto & parse($Struct::Struct const & src) {
			memCpy(&(_unk[0]), &(src._unk[0]), 13);
			_originX = src._originX / kRateOfAnimOri;
			_originY = src._originY / kRateOfAnimOri;
			_image.parse(src._image);
			_subAnim.parse(src._subAnim);
			_mainAnim.parse(src._mainAnim);
			return *this;
		}
		auto & parseTo($Struct::Struct & dst) const {
			memCpy(&(dst._unk[0]), &(_unk[0]), 13);
			dst._originX = _originX * kRateOfAnimOri;
			dst._originY = _originY * kRateOfAnimOri;
			_image.parseTo(dst._image);
			_subAnim.parseTo(dst._subAnim);
			_mainAnim.parseTo(dst._mainAnim);
			return *this;
		}
	};
}
