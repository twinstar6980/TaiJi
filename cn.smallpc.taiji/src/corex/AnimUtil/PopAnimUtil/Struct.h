# pragma once

# include "../../Type.h"
# include "../../UtilKit/DataBlockUtil.h"
# include "../../MyType/Str.h"
# include "../../MyType/ListOfOther.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Struct {
	inline auto CalcVarIntSize(SzI const & val) { // 16bit only
		return static_cast<SzI>(ternary_oper(val >= 0xFFu, szOf<UI8>() + szOf<UI16>(), szOf<UI8>()));
	}
	inline auto ReadVarInt(Byte const * const & data, SzI & val) {
		auto pos = kThFirst;
		val = *reinterpret_cast<UI8 const *>(data + pos);
		pos += szOf<UI8>();
		if (val == 0xFFu) {
			val = *reinterpret_cast<UI16 const *>(data + pos);
			pos += szOf<UI16>();
		}
		return pos;
	}
	inline SzI WriteVarInt(Byte * const & data, SzI const & val) {
		if (val >= 0xFFu) {
			*reinterpret_cast<UI8 *>(data + kThFirst) = 0xFFu;
			*reinterpret_cast<UI16 *>(data + kThSecond) = val;
			return szOf<UI8>() + szOf<UI16>();
		} else {
			*reinterpret_cast<UI8 *>(data + kThFirst) = val;
			return szOf<UI8>();
		}
	}

	inline auto CalcVarInt2Size(SzI const & val, UI8 const & unit) { // 32bit and 8bit only
		return static_cast<SzI>(ternary_oper(val >= ((0x100u * unit) - 1), szOf<UI16>() + szOf<UI32>(), szOf<UI16>()));
	}
	inline SzI ReadVarInt2(Byte const * const & data, SzI & val, UI8 & val2, UI8 const & unit) {
		val = data[kThFirst]; val2 = data[kThSecond];
		auto diff = val2 % unit;
		val2 -= diff;
		if (val == 0xFFu && diff == (unit - 1)) {
			val = *reinterpret_cast<UI32 const *>(data + kThThird);
			return szOf<UI16>() + szOf<UI32>();
		} else {
			val += diff * 0x100u;
			return szOf<UI16>();
		}
	}
	inline SzI WriteVarInt2(Byte * const & data, SzI const & val, UI8 const & val2, UI8 const & unit) {
		if (val >= ((0x100 * unit) - 1)) {
			*reinterpret_cast<Byte *>(data + kThFirst) = 0xFF;
			*reinterpret_cast<Byte *>(data + kThSecond) = val2 + (unit - 1);
			*reinterpret_cast<UI32 *>(data + kThThird) = val;
			return szOf<UI16>() + szOf<UI32>();
		} else {
			*reinterpret_cast<UI16 *>(data + kThFirst) = val;
			*reinterpret_cast<Byte *>(data + kThSecond) += val2;/*
			*reinterpret_cast<UI8 *>(data + kThFirst) = val % 0x100;
			*reinterpret_cast<UI8 *>(data + kThSecond) = val2 + val / 0x100;*/
			return szOf<UI16>();
		}
	}

	enum class TypeOfSumInt : UI8 { kUInt16, kVarInt };

	tmpl_st(T) auto GetBlockSize(ListOfBlock<T> const & blockList, TypeOfSumInt const & sumIntType) {
		auto sz = kSzZero;
		goif(sumIntType == TypeOfSumInt::kUInt16) sz += szOf<UI16>();
		elif(sumIntType == TypeOfSumInt::kVarInt) sz += CalcVarIntSize(blockList._sz);
		sz += blockList.getSz();
		return sz;
	}
	tmpl_st(T) auto ReadBlockList(Byte const * const & data, ListOfBlock <T> & blockList, TypeOfSumInt const & sumIntType) {
		auto pos = kThFirst;
		SzI ItemSum;
		if (sumIntType == TypeOfSumInt::kUInt16) {
			ItemSum = *reinterpret_cast <UI16 const *> (data + pos);
			pos += szOf<UI16>();
		} elif(sumIntType == TypeOfSumInt::kVarInt) {
			pos += ReadVarInt(data + pos, ItemSum);
		}
		pos += blockList.read(data + pos, ItemSum).getSz();
		return pos;
	}
	tmpl_st(T) auto WriteBlockList(Byte * const & data, ListOfBlock<T> const & blockList, TypeOfSumInt const & sumIntType) {
		auto pos = kThFirst;
		if (sumIntType == TypeOfSumInt::kUInt16) {
			*reinterpret_cast <UI16 *> (data + pos) = blockList._sz;
			pos += szOf<UI16>();
		} elif(sumIntType == TypeOfSumInt::kVarInt) {
			pos += WriteVarInt(data + pos, blockList._sz);
		}
		pos += blockList.write(data + pos).getSz();
		return pos;
	}

	constexpr SzI kHeaderSize(17);

	namespace $ImageInfo {
		constexpr SzI kBlockSize(24);
		constexpr SzI kMemberSum(6);
		namespace $MemberOSD {
			constexpr OSD kWidth(0x0, 2);
			constexpr OSD kHeight(0x2, 2);
			constexpr OSD kPosX(0x14, 2);
			constexpr OSD kPosY(0x16, 2);
			constexpr OSD kScaleX(0x4, 4);
			constexpr OSD kScaleY(0x10, 4);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI16 _w;
			UI16 _h;
			SI16 _posX;
			SI16 _posY;
			SI32 _scaleX;
			SI32 _scaleY;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_w,
						& _h,
						& _posX,
						& _posY,
						& _scaleX,
						& _scaleY,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_w,
						& _h,
						& _posX,
						& _posY,
						& _scaleX,
						& _scaleY,
				};
			}
		};
	}
	struct ImageInfoWithStr {
		StrBlock16 _str;
		$ImageInfo::Member _info;

		SzI getSz() const {
			return _str.getSz() + _info.getSz();
		}
		auto & read(Byte const * const & data) {
			_str.read(data + kThFirst);
			$DataBlockUtil::read(data + _str.getSz(), _info);
			return *this;
		}
		auto & write(Byte * const & data) const {
			_str.write(data + kThFirst);
			$DataBlockUtil::write(data + _str.getSz(), _info);
			return *this;
		}
	};

	struct LayerErase {
		SzI _idx;
		UI8 _unKnown; // unknown

		SzI getSz() const {
			return CalcVarInt2Size(_idx, 8);
		}
		auto & read(Byte const * const & data) {
			ReadVarInt2(data + kThFirst, _idx, _unKnown, 8);
			return *this;
		}
		auto & write(Byte * const & data) const {
			WriteVarInt2(data + kThFirst, _idx, _unKnown, 8);
			return *this;
		}
	};
	namespace $ExistBit_RefType {
		constexpr Byte kIsAnim(128);
		constexpr Byte kU7(64);
		constexpr Byte kU6(32);
		constexpr Byte kLabel(16);
		constexpr Byte kU4(8);
		constexpr Byte kNone(0);
	}
	struct LayerAppend {
		SzI _idx;
		SzI _refIdx; // TODO : u8 or varint ?
		Bool _isAnim;
		Bool _u7;
		Bool _u6;
		Bool _existLabel;
		Bool _u4;
		StrBlock16 _label;

		SzI getSz() const {
			auto sz = kSzZero;
			sz += CalcVarInt2Size(_idx, 8);
			sz += CalcVarIntSize(_refIdx);
			if (_existLabel)
				sz += _label.getSz();
			return sz;
		}
		auto & read(Byte const * const & data) {
			auto pos = kThFirst;
			{
				Byte ident;
				pos += ReadVarInt2(data + pos, _idx, ident, 8);
				_isAnim = ident & $ExistBit_RefType::kIsAnim;
				_u7 = ident & $ExistBit_RefType::kU7;
				_u6 = ident & $ExistBit_RefType::kU6;
				_existLabel = ident & $ExistBit_RefType::kLabel;
				_u4 = ident & $ExistBit_RefType::kU4;
				if (_u7) {
					wLogLn("u7");
					system("pause");
				}
				if (_u6) {
					wLogLn("u6");
					system("pause");
				}
				if (_u4) {
					wLogLn("u4");
					system("pause");
				}
			}
			pos += ReadVarInt(data + pos, _refIdx);
			if (_existLabel)
				pos += _label.read(data + pos).getSz();
			return *this;
		}
		auto & write(Byte * const & data) const {
			auto pos = kThFirst;
			{
				auto ident = $ExistBit_RefType::kNone;
				if (_isAnim)
					ident |= $ExistBit_RefType::kIsAnim;
				if (_u7)
					ident |= $ExistBit_RefType::kU7;
				if (_u6)
					ident |= $ExistBit_RefType::kU6;
				if (_existLabel)
					ident |= $ExistBit_RefType::kLabel;
				if (_u4)
					ident |= $ExistBit_RefType::kU4;
				pos += WriteVarInt2(data + pos, _idx, ident, 8);
			}
			pos += WriteVarInt(data + CalcVarInt2Size(_idx, 8), _refIdx);
			{
				if (_existLabel)
					pos += _label.write(data + pos).getSz();
			}
			return *this;
		}
	};
	namespace $LayerChgType {
		namespace $Pos {
			constexpr SzI kBlockSize(8);
			constexpr SzI kMemberSum(2);
			namespace $MemberOSD {
				constexpr OSD kX(0x0, 4);
				constexpr OSD kY(0x4, 4);
			}
			extern OSD const * const kMemberOSDPtr[kMemberSum];
			struct Member {
				SI32 _x;
				SI32 _y;

				constexpr auto getSz() const { return kBlockSize; }
				constexpr auto getSum() const { return kMemberSum; }

				OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

				auto getPtr() {
					return new Void * const [kMemberSum] {
						&_x,
							& _y,
					};
				}
				auto getPtr() const {
					return new Void const * const [kMemberSum] {
						&_x,
							& _y,
					};
				}
			};
		}
		namespace $Matrix {
			constexpr SzI kBlockSize(16);
			constexpr SzI kMemberSum(4);
			namespace $MemberOSD {
				constexpr OSD kA(0x0, 4);
				constexpr OSD kB(0x8, 4);
				constexpr OSD kC(0x4, 4);
				constexpr OSD kD(0xC, 4);
			}
			extern OSD const * const kMemberOSDPtr[kMemberSum];
			struct Member {
				SI32 _a;
				SI32 _b;
				SI32 _c;
				SI32 _d;

				constexpr auto getSz() const { return kBlockSize; }
				constexpr auto getSum() const { return kMemberSum; }

				OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

				auto getPtr() {
					return new Void * const [kMemberSum] {
						&_a,
							& _b,
							& _c,
							& _d,
					};
				}
				auto getPtr() const {
					return new Void const * const [kMemberSum] {
						&_a,
							& _b,
							& _c,
							& _d,
					};
				}
			};
		}
		namespace $ColorRate {
			constexpr SzI kBlockSize(4);
			constexpr SzI kMemberSum(4);
			namespace $MemberOSD {
				constexpr OSD kRed(0x0, 1);
				constexpr OSD kGreen(0x1, 1);
				constexpr OSD kBlue(0x2, 1);
				constexpr OSD kAlpha(0x3, 1);
			}
			extern OSD const * const kMemberOSDPtr[kMemberSum];
			struct Member {
				UI8 _r;
				UI8 _g;
				UI8 _b;
				UI8 _a;

				constexpr auto getSz() const { return kBlockSize; }
				constexpr auto getSum() const { return kMemberSum; }

				OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

				auto getPtr() {
					return new Void * const [kMemberSum] {
						&_r,
							& _g,
							& _b,
							& _a,
					};
				}
				auto getPtr() const {
					return new Void const * const [kMemberSum] {
						&_r,
							& _g,
							& _b,
							& _a,
					};
				}
			};
		}
		namespace $Rotate {
			constexpr SzI kBlockSize(2);
			constexpr SzI kMemberSum(1);
			namespace $MemberOSD {
				constexpr OSD kAngle(0x0, 2);
			}
			extern OSD const * const kMemberOSDPtr[kMemberSum];
			struct Member {
				SI16 _angle;

				constexpr auto getSz() const { return kBlockSize; }
				constexpr auto getSum() const { return kMemberSum; }

				OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

				auto getPtr() {
					return new Void * const [kMemberSum] {
						&_angle,
					};
				}
				auto getPtr() const {
					return new Void const * const [kMemberSum] {
						&_angle,
					};
				}
			};
		}
	}
	namespace $ExistBit_LayerChg {
		constexpr Byte kU8(128);
		constexpr Byte kAngle(64);
		constexpr Byte kColorRate(32);
		constexpr Byte kMatrix(16);
		constexpr Byte kPos(8);
		constexpr Byte kU3(4);
		constexpr Byte kNone(0);
	}
	struct LayerChg {
		SzI _idx;
		Bool _existU8;
		Bool _existAngle;
		Bool _existColorRate;
		Bool _existMatrix;
		Bool _existPos;
		Bool _existU3;
		$LayerChgType::$Rotate::Member _angle;
		$LayerChgType::$ColorRate::Member _colorRate;
		$LayerChgType::$Matrix::Member _matrix;
		$LayerChgType::$Pos::Member _pos;

		auto getSz() const {
			auto sz = kSzZero;
			sz += CalcVarInt2Size(_idx, 4);
			if (_existAngle)
				sz += _angle.getSz();
			if (_existMatrix)
				sz += _matrix.getSz();
			if (_existPos)
				sz += _pos.getSz();
			if (_existColorRate)
				sz += _colorRate.getSz();
			return sz;
		}
		auto & read(Byte const * const & data) {
			auto pos = kThFirst;
			{
				Byte ident;
				pos += ReadVarInt2(data + pos, _idx, ident, 4);
				_existU8 = ident & $ExistBit_LayerChg::kU8;
				_existAngle = ident & $ExistBit_LayerChg::kAngle;
				_existColorRate = ident & $ExistBit_LayerChg::kColorRate;
				_existMatrix = ident & $ExistBit_LayerChg::kMatrix;
				_existPos = ident & $ExistBit_LayerChg::kPos;
				_existU3 = ident & $ExistBit_LayerChg::kU3;
				if (_existU8) {
					wLogLn("u8");
					system("pause");
				}
				if (_existU3) {
					wLogLn("u3");
					system("pause");
				}
			}
			{
				if (_existAngle)
					pos += $DataBlockUtil::read(data + pos, _angle).getSz();
				if (_existMatrix)
					pos += $DataBlockUtil::read(data + pos, _matrix).getSz();
				if (_existPos)
					pos += $DataBlockUtil::read(data + pos, _pos).getSz();
				if (_existColorRate)
					pos += $DataBlockUtil::read(data + pos, _colorRate).getSz();
			}
			return *this;
		}
		auto & write(Byte * const & data) const {
			auto pos = kThFirst;
			{
				auto ident = $ExistBit_LayerChg::kNone;
				if (_existU8)
					ident |= $ExistBit_LayerChg::kU8;
				if (_existAngle)
					ident |= $ExistBit_LayerChg::kAngle;
				if (_existColorRate)
					ident |= $ExistBit_LayerChg::kColorRate;
				if (_existMatrix)
					ident |= $ExistBit_LayerChg::kMatrix;
				if (_existPos)
					ident |= $ExistBit_LayerChg::kPos;
				if (_existU3)
					ident |= $ExistBit_LayerChg::kU3;
				pos += WriteVarInt2(data + pos, _idx, ident, 4);
			}
			{
				if (_existAngle)
					pos += $DataBlockUtil::write(data + pos, _angle).getSz();
				if (_existMatrix)
					pos += $DataBlockUtil::write(data + pos, _matrix).getSz();
				if (_existPos)
					pos += $DataBlockUtil::write(data + pos, _pos).getSz();
				if (_existColorRate)
					pos += $DataBlockUtil::write(data + pos, _colorRate).getSz();
			}
			return *this;
		}
	};
	struct FSCmd {
		StrBlock16 _cmd, _arg;

		SzI getSz() const {
			return _cmd.getSz() + _arg.getSz();
		}
		auto & read(Byte const * const & data) {
			_cmd.read(data + kThFirst);
			_arg.read(data + _cmd.getSz());
			return *this;
		}
		auto & write(Byte * const & data) const {
			_cmd.write(data + kThFirst);
			_arg.write(data + _cmd.getSz());
			return *this;
		}
	};
	namespace $ExistBit_Frame {
		constexpr Byte kU8(128);
		constexpr Byte kU7(64);
		constexpr Byte kCmd(32);
		constexpr Byte kStop(16);
		constexpr Byte kLabel(8);
		constexpr Byte kChg(4);
		constexpr Byte kAppend(2);
		constexpr Byte kErase(1);
		constexpr Byte kNone(0);
	}
	struct Frame {
		Bool _existCmd;
		Bool _existStop;
		Bool _existLabel;
		Bool _existChg;
		Bool _existAppend;
		Bool _existErase;
		ListOfBlock<FSCmd> _cmd;
		StrBlock16 _label;
		ListOfBlock<LayerChg> _chg;
		ListOfBlock<LayerAppend> _append;
		ListOfBlock<LayerErase> _erase;

		auto getSz() const {
			auto sz = kSzZero;
			sz += szOf<Byte>();
			if (_existErase)
				sz += GetBlockSize(_erase, TypeOfSumInt::kVarInt);
			if (_existAppend)
				sz += GetBlockSize(_append, TypeOfSumInt::kVarInt);
			if (_existChg)
				sz += GetBlockSize(_chg, TypeOfSumInt::kVarInt);
			if (_existLabel)
				sz += _label.getSz();
			if (_existCmd)
				sz += GetBlockSize(_cmd, TypeOfSumInt::kVarInt);
			return sz;
		}
		auto & read(Byte const * const & data) {
			auto pos = kThFirst;
			{
				auto ident = data[pos++];
				_existCmd = ident & $ExistBit_Frame::kCmd;
				_existStop = ident & $ExistBit_Frame::kStop;
				_existLabel = ident & $ExistBit_Frame::kLabel;
				_existChg = ident & $ExistBit_Frame::kChg;
				_existAppend = ident & $ExistBit_Frame::kAppend;
				_existErase = ident & $ExistBit_Frame::kErase;
			}
			{
				if (_existErase)
					pos += ReadBlockList(data + pos, _erase, TypeOfSumInt::kVarInt);
				if (_existAppend)
					pos += ReadBlockList(data + pos, _append, TypeOfSumInt::kVarInt);
				if (_existChg)
					pos += ReadBlockList(data + pos, _chg, TypeOfSumInt::kVarInt);
				if (_existLabel)
					pos += _label.read(data + pos).getSz();
				if (_existCmd)
					pos += ReadBlockList(data + pos, _cmd, TypeOfSumInt::kVarInt);
			}
			return *this;
		}
		auto & write(Byte * const & data) const {
			auto pos = kThFirst;
			{
				auto & ident = data[pos++];
				ident &= $ExistBit_Frame::kNone;
				if (_existCmd)
					ident |= $ExistBit_Frame::kCmd;
				if (_existStop)
					ident |= $ExistBit_Frame::kStop;
				if (_existLabel)
					ident |= $ExistBit_Frame::kLabel;
				if (_existChg)
					ident |= $ExistBit_Frame::kChg;
				if (_existAppend)
					ident |= $ExistBit_Frame::kAppend;
				if (_existErase)
					ident |= $ExistBit_Frame::kErase;
			}
			{
				if (_existErase)
					pos += WriteBlockList(data + pos, _erase, TypeOfSumInt::kVarInt);
				if (_existAppend)
					pos += WriteBlockList(data + pos, _append, TypeOfSumInt::kVarInt);
				if (_existChg)
					pos += WriteBlockList(data + pos, _chg, TypeOfSumInt::kVarInt);
				if (_existLabel)
					pos += _label.write(data + pos).getSz();
				if (_existCmd)
					pos += WriteBlockList(data + pos, _cmd, TypeOfSumInt::kVarInt);
			}
			return *this;
		}
	};

	struct Type1AtAnimBegin {
		ListOfBlock<LayerErase> _v1;
		ListOfBlock<LayerErase> _v2;

		auto & tfree() {
			_v1.tfree();
			_v2.tfree();
			return *this;
		}

		SzI getSz() const {
			return GetBlockSize(_v1, TypeOfSumInt::kVarInt) + GetBlockSize(_v2, TypeOfSumInt::kVarInt);
		}
		auto & read(Byte const * const & data) {
			ReadBlockList(data + kThFirst, _v1, TypeOfSumInt::kVarInt);
			ReadBlockList(data + GetBlockSize(_v1, TypeOfSumInt::kVarInt), _v2, TypeOfSumInt::kVarInt);
			return *this;
		}
		auto & write(Byte * const & data) const {
			WriteBlockList(data + kThFirst, _v1, TypeOfSumInt::kVarInt);
			WriteBlockList(data + GetBlockSize(_v1, TypeOfSumInt::kVarInt), _v2, TypeOfSumInt::kVarInt);
			return *this;
		}
	};
	namespace $AnimFrameInfo {
		constexpr SzI kBlockSize(12);
		constexpr SzI kMemberSum(4);
		namespace $MemberOSD {
			constexpr OSD kRate(0x4, 2);
			constexpr OSD kSum(0x6, 2);
			constexpr OSD kBegin(0x8, 2);
			constexpr OSD kEnd(0xA, 2);
		}
		extern OSD const * const kMemberOSDPtr[kMemberSum];
		struct Member {
			UI16 _rate;
			UI16 _sz;
			UI16 _begin;
			UI16 _end;

			constexpr auto getSz() const { return kBlockSize; }
			constexpr auto getSum() const { return kMemberSum; }

			OSD const * const * const getOSDPtr() const { return kMemberOSDPtr; }

			auto getPtr() {
				return new Void * const [kMemberSum] {
					&_rate,
						& _sz,
						& _begin,
						& _end,
				};
			}
			auto getPtr() const {
				return new Void const * const [kMemberSum] {
					&_rate,
						& _sz,
						& _begin,
						& _end,
				};
			}
		};
	}
	struct Anim {
		$AnimFrameInfo::Member _info;
		ListOfBlock<Frame> _frame;

		auto & getBase() { return *this; }
		auto & getBase() const { return *this; }

		auto & tfree() {
			//_info.setNull();
			_frame.tfree();
			return *this;
		}

		SzI getSz() const {
			return _info.getSz() + _frame.getSz();
		}
		auto & read(Byte const * const & data) {
			$DataBlockUtil::read(data + kThFirst, _info);
			_frame.read(data + _info.getSz(), _info._sz);
			return *this;
		}
		auto & write(Byte * const & data) const {
			$DataBlockUtil::write(data + kThFirst, _info);
			_frame.write(data + _info.getSz());
			return *this;
		}
	};
	struct AnimWithLabel : Anim {
		StrBlock16 _label;

		auto & tfree() {
			getBase().tfree();
			_label.tfree();
			return *this;
		}

		SzI getSz() const {
			return _label.getSz() + getBase().getSz();
		}
		auto & read(Byte const * const & data) {
			_label.read(data + kThFirst);
			getBase().read(data + _label.getSz());
			return *this;
		}
		auto & write(Byte * const & data) const {
			_label.write(data + kThFirst);
			getBase().write(data + _label.getSz());
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

		SzI getSz() const {
			return szOf<UI8>() + _block1.getSz() + getBase().getSz();
		}
		auto & read(Byte const * const & data) {
			auto pos = kThFirst;
			if (data[pos++] != 0x1)
				wErr("Anim-ERROR : Not 01 Block");
			if (data[pos] != 0 || data[pos + 1] != 0)
				wWarn("not 0 & 0 at %X", pos);
			pos += _block1.read(data + pos).getSz();
			pos += getBase().read(data + pos).getSz();
			return *this;
		}
		auto & write(Byte * const & data) const {
			auto pos = kThFirst;
			*reinterpret_cast<UI8 *>(data + pos) = 0x1;
			pos += szOf<UI8>();
			pos += _block1.write(data + pos).getSz();
			pos += getBase().write(data + pos).getSz();
			return *this;
		}
	};
	struct Struct {
		Byte _unk[13];
		F64 _originX, _originY;
		ListOfBlock<ImageInfoWithStr> _image;
		ListOfBlock<AnimWithLabel> _subAnim;
		AnimWithUnk _mainAnim;

		auto & getThis() { return *this; };

		auto & tfree() {
			_image.setNull();
			_subAnim.setNull();
			_mainAnim.tfree();
			return *this;
		}

		auto & read(Byte const * const & data) {
			auto pos = kThFirst;
			memCpy(&(_unk[0]), data + pos, 13);
			pos += 13;
			_originX = *reinterpret_cast<SI16 const *>(data + pos);
			pos += szOf<SI16>();
			_originY = *reinterpret_cast<SI16 const *>(data + pos);
			pos += szOf<SI16>();
			pos += ReadBlockList(data + pos, _image, $Struct::TypeOfSumInt::kUInt16);
			pos += ReadBlockList(data + pos, _subAnim, $Struct::TypeOfSumInt::kUInt16);
			pos += _mainAnim.read(data + pos).getSz();
			return *this;
		}
		auto & write(Byte * const & data, SzI & sz) const {
			sz = kThFirst;
			memCpy(data + sz, &(_unk[0]), 13);
			sz += 13;
			*reinterpret_cast<SI16 *>(data + sz) = _originX;
			sz += szOf<SI16>();
			*reinterpret_cast<SI16 *>(data + sz) = _originY;
			sz += szOf<SI16>();
			sz += WriteBlockList(data + sz, _image, $Struct::TypeOfSumInt::kUInt16);
			sz += WriteBlockList(data + sz, _subAnim, $Struct::TypeOfSumInt::kUInt16);
			sz += _mainAnim.write(data + sz).getSz();
			return *this;
		}
	};
}
