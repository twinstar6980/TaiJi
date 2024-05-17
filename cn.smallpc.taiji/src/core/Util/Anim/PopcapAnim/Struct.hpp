#pragma once

#include "../../../Type.hpp"
#include "../../../Type/Str.hpp"
#include "../../../Type/BlockFunc.hpp"
#include "../../../Type/InfoType.hpp"
#include "../../../UserLiteral.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Struct {

	inline auto red4arInt(ByteStream & data, Sz & val) {
		val = data.rd<IU8>();
		if (val == 0xFFu)
			val = data.rd<IU16>();
		return;
	}
	inline auto writeVarInt(ByteStream & data, Sz const & val) {
		if (val >= 0xFFu)
			data << IU8(0xFF) << IU16(val);
		else
			data << IU8(val);
		return;
	}

	inline auto red4arInt2(ByteStream & data, Sz & val, IU8 & val2, IU8 const & unit) {
		val = data.rd<IU8>();
		val2 = data.rd<IU8>();
		auto diff = val2 % unit;
		val2 -= diff;
		if (val == 0xFFu && diff == unit - 1) {
			val = data.rd<IU32>();
		} else {
			val += diff * 0x100u;
		}
		return;
	}
	inline auto writeVarInt2(ByteStream & data, Sz const & val, IU8 const & val2, IU8 const & unit) {
		if (val >= 0x100 * unit - 1) {
			data << IU8(0xFF) << IU8(val2 + (unit - 1)) << IU32(val);
		} else {
			data << IU16(val);
			data.posBack<IU8>().rd<IU8>() += val2;
		}
		return;
	}

	enum class SumIntType : IU8 { kU16, kVar };

	template <SumIntType sumTy, typename T>
	auto readBlockList(ByteStream & data, List<T> & block) {
		Sz sum;
		if constexpr (sumTy == SumIntType::kU16) {
			sum = data.rd<IU16>();
		} else if constexpr (sumTy == SumIntType::kVar) {
			red4arInt(data, sum);
		}
		block.allocThenUse(sum);
		$DynamicBlock::read(block, data);
		return;
	}
	template <SumIntType sumTy, typename T>
	auto writeBlockList(ByteStream & data, List<T> const & block) {
		if constexpr (sumTy == SumIntType::kU16) {
			data << IU16(block.$sz);
		} else if constexpr (sumTy == SumIntType::kVar) {
			writeVarInt(data, block.$sz);
		}
		$DynamicBlock::write(block, data);
		return;
	}

	constexpr Sz kHeaderSize(17);

	struct ImageBase {

		static inline constexpr Sz kSz = 24;
		static inline constexpr Sz kSum = 6;

		static inline constexpr OSD kInfo[]{
			OSD(0x0, 2), // szW
			OSD(0x2, 2), // szH
			OSD(0x14, 2), // posX
			OSD(0x16, 2), // posY
			OSD(0x4, 4), // scaleX
			OSD(0x10, 4), // scaleY
		};

		IU16 $szW;
		IU16 $szH;
		IS16 $posX;
		IS16 $posY;
		IS32 $scaleX;
		IS32 $scaleY;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $szW,
				& $szH,
				& $posX,
				& $posY,
				& $scaleX,
				& $scaleY,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct Image {

		StrBlock<IU16> $str;
		ImageBase $info;

		auto & read(ByteStream & t) {
			$str.read(t);
			$StaticBlock::read($info, t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			$str.write(t);
			$StaticBlock::write($info, t);
			return thiz;
		}

	};

	struct LayerErase {

		Sz $idx;
		IU8 $unk;

		auto & read(ByteStream & t) {
			red4arInt2(t, $idx, $unk, 8);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			writeVarInt2(t, $idx, $unk, 8);
			return thiz;
		}

	};

	namespace $ExistBit_RefType {

		inline constexpr Byte kIsAnim("10000000"_bin8);
		inline constexpr Byte kU7("01000000"_bin8);
		inline constexpr Byte kU6("00100000"_bin8);
		inline constexpr Byte kLabel("00010000"_bin8);
		inline constexpr Byte kU4("00001000"_bin8);
		inline constexpr Byte kNone("00000000"_bin8);

	}

	struct LayerAppend {

		Sz $idx;
		Sz $refIdx; // TODO : u8 or varint ?
		Bool $isAnim;
		Bool $u7;
		Bool $u6;
		ExistShell<StrBlock<IU16>> $label;
		Bool $u4;

		auto & read(ByteStream & t) {
			{
				Byte ident;
				red4arInt2(t, $idx, ident, 8);
				$isAnim = ident & $ExistBit_RefType::kIsAnim;
				$u7 = ident & $ExistBit_RefType::kU7;
				$u6 = ident & $ExistBit_RefType::kU6;
				$label.$state = ident & $ExistBit_RefType::kLabel;
				$u4 = ident & $ExistBit_RefType::kU4;

				if ($u7) throw VAMGException("PopcapAnimation-Reader", "u7");
				if ($u6) throw VAMGException("PopcapAnimation-Reader", "u6");
				if ($u4) throw VAMGException("PopcapAnimation-Reader", "u4");
			}
			red4arInt(t, $refIdx);
			if ($label.$state)
				$label.$data.read(t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			{
				auto ident = $ExistBit_RefType::kNone;
				if ($isAnim)
					ident |= $ExistBit_RefType::kIsAnim;
				if ($u7)
					ident |= $ExistBit_RefType::kU7;
				if ($u6)
					ident |= $ExistBit_RefType::kU6;
				if ($label.$state)
					ident |= $ExistBit_RefType::kLabel;
				if ($u4)
					ident |= $ExistBit_RefType::kU4;
				writeVarInt2(t, $idx, ident, 8);
			}
			writeVarInt(t, $refIdx);
			{
				if ($label.$state)
					$label.$data.write(t);
			}
			return thiz;
		}

	};

	namespace $LayerChgType {

		struct Pos {

			static inline constexpr Sz kSz = 8;
			static inline constexpr Sz kSum = 2;

			static inline constexpr OSD kInfo[]{
				OSD(0x0, 4), // x
				OSD(0x4, 4), // y
			};

			IS32 $x;
			IS32 $y;

			auto addr() const {
				return new Void const * const [kSum] {\
					& $x,
					& $y,
				};
			}
			auto addr() {
				return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
			}

		};

		struct Matrix {

			static inline constexpr Sz kSz = 16;
			static inline constexpr Sz kSum = 4;

			static inline constexpr OSD kInfo[]{
				OSD(0x0, 4), // a
				OSD(0x8, 4), // b
				OSD(0x4, 4), // c
				OSD(0xC, 4), // d
			};

			IS32 $a;
			IS32 $b;
			IS32 $c;
			IS32 $d;

			auto addr() const {
				return new Void const * const [kSum] {\
					& $a,
					& $b,
					& $c,
					& $d,
				};
			}
			auto addr() {
				return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
			}

		};

		struct ColorRate {

			static inline constexpr Sz kSz = 4;
			static inline constexpr Sz kSum = 4;

			static inline constexpr OSD kInfo[]{
				OSD(0x0, 1), // r
				OSD(0x1, 1), // g
				OSD(0x2, 1), // b
				OSD(0x3, 1), // a
			};

			IU8 $r;
			IU8 $g;
			IU8 $b;
			IU8 $a;

			auto addr() const {
				return new Void const * const [kSum] {\
					& $r,
					& $g,
					& $b,
					& $a,
				};
			}
			auto addr() {
				return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
			}

		};

		struct Rotate {

			static inline constexpr Sz kSz = 2;
			static inline constexpr Sz kSum = 1;

			static inline constexpr OSD kInfo[]{
				OSD(0x0, 2), // angle
			};

			IS16 $angle;

			auto addr() const {
				return new Void const * const [kSum] {\
					& $angle,
				};
			}
			auto addr() {
				return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
			}

		};

	}

	namespace $ExistBit_LayerChg {

		inline constexpr Byte kU8("10000000"_bin8);
		inline constexpr Byte kRotate("01000000"_bin8);
		inline constexpr Byte kColor("00100000"_bin8);
		inline constexpr Byte kMatrix("00010000"_bin8);
		inline constexpr Byte kPos("00001000"_bin8);
		inline constexpr Byte kU3("00000100"_bin8);
		inline constexpr Byte kNone("00000000"_bin8);

	}

	struct LayerChg {

		Sz $idx;
		Bool $existU8;
		ExistShell<$LayerChgType::Rotate> $rotate;
		ExistShell<$LayerChgType::ColorRate> $color;
		ExistShell<$LayerChgType::Matrix> $matrix;
		ExistShell<$LayerChgType::Pos> $pos;
		Bool $existU3;

		auto & read(ByteStream & t) {
			{
				Byte ident;
				red4arInt2(t, $idx, ident, 4);
				$existU8 = ident & $ExistBit_LayerChg::kU8;
				$rotate.$state = ident & $ExistBit_LayerChg::kRotate;
				$color.$state = ident & $ExistBit_LayerChg::kColor;
				$matrix.$state = ident & $ExistBit_LayerChg::kMatrix;
				$pos.$state = ident & $ExistBit_LayerChg::kPos;
				$existU3 = ident & $ExistBit_LayerChg::kU3;
				if ($existU8) {
					log.txtln("u8");
				}
				if ($existU3) {
					log.txtln("u3");
				}
			}
			{
				if ($rotate.$state)
					$StaticBlock::read($rotate.$data, t);
				if ($matrix.$state)
					$StaticBlock::read($matrix.$data, t);
				if ($pos.$state)
					$StaticBlock::read($pos.$data, t);
				if ($color.$state)
					$StaticBlock::read($color.$data, t);
			}
			return thiz;
		}
		auto & write(ByteStream & t) const {
			{
				auto ident = $ExistBit_LayerChg::kNone;
				if ($existU8)
					ident |= $ExistBit_LayerChg::kU8;
				if ($rotate.$state)
					ident |= $ExistBit_LayerChg::kRotate;
				if ($color.$state)
					ident |= $ExistBit_LayerChg::kColor;
				if ($matrix.$state)
					ident |= $ExistBit_LayerChg::kMatrix;
				if ($pos.$state)
					ident |= $ExistBit_LayerChg::kPos;
				if ($existU3)
					ident |= $ExistBit_LayerChg::kU3;
				writeVarInt2(t, $idx, ident, 4);
			}
			{
				if ($rotate.$state)
					$StaticBlock::write($rotate.$data, t);
				if ($matrix.$state)
					$StaticBlock::write($matrix.$data, t);
				if ($pos.$state)
					$StaticBlock::write($pos.$data, t);
				if ($color.$state)
					$StaticBlock::write($color.$data, t);
			}
			return thiz;
		}

	};

	struct FSCmd {

		StrBlock<IU16> $cmd, $arg;

		auto & read(ByteStream & t) {
			$cmd.read(t);
			$arg.read(t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			$cmd.write(t);
			$arg.write(t);
			return thiz;
		}

	};

	namespace $ExistBit_Frame {

		inline constexpr Byte kU8("10000000"_bin8);
		inline constexpr Byte kU7("01000000"_bin8);
		inline constexpr Byte kCmd("00100000"_bin8);
		inline constexpr Byte kStop("00010000"_bin8);
		inline constexpr Byte kLabel("00001000"_bin8);
		inline constexpr Byte kChg("00000100"_bin8);
		inline constexpr Byte kAppend("00000010"_bin8);
		inline constexpr Byte kErase("00000001"_bin8);
		inline constexpr Byte kNone("00000000"_bin8);

	}

	struct Frame {

		ExistShell<List<FSCmd>> $cmd;
		ExistShell<Bool> $stop;
		ExistShell<StrBlock<IU16>> $label;
		ExistShell<List<LayerChg>> $chg;
		ExistShell<List<LayerAppend>> $append;
		ExistShell<List<LayerErase>> $erase;

		auto & read(ByteStream & t) {
			{
				auto ident = t.rd<IU8>();
				$cmd.$state = ident & $ExistBit_Frame::kCmd;
				$stop.$state = ident & $ExistBit_Frame::kStop;
				$label.$state = ident & $ExistBit_Frame::kLabel;
				$chg.$state = ident & $ExistBit_Frame::kChg;
				$append.$state = ident & $ExistBit_Frame::kAppend;
				$erase.$state = ident & $ExistBit_Frame::kErase;
			}
			{
				if ($stop.$state)
					$stop.$data = kBoT;
				if ($erase.$state)
					readBlockList<SumIntType::kVar>(t, $erase.$data);
				if ($append.$state)
					readBlockList<SumIntType::kVar>(t, $append.$data);
				if ($chg.$state)
					readBlockList<SumIntType::kVar>(t, $chg.$data);
				if ($label.$state)
					$label.$data.read(t);
				if ($cmd.$state)
					readBlockList<SumIntType::kVar>(t, $cmd.$data);
			}
			return thiz;
		}
		auto & write(ByteStream & t) const {
			{
				auto ident = $ExistBit_Frame::kNone;
				if ($cmd.$state)
					ident |= $ExistBit_Frame::kCmd;
				if ($stop.$state)
					ident |= $ExistBit_Frame::kStop;
				if ($label.$state)
					ident |= $ExistBit_Frame::kLabel;
				if ($chg.$state)
					ident |= $ExistBit_Frame::kChg;
				if ($append.$state)
					ident |= $ExistBit_Frame::kAppend;
				if ($erase.$state)
					ident |= $ExistBit_Frame::kErase;
				t << ident;
			}
			{
				if ($erase.$state)
					writeBlockList<SumIntType::kVar>(t, $erase.$data);
				if ($append.$state)
					writeBlockList<SumIntType::kVar>(t, $append.$data);
				if ($chg.$state)
					writeBlockList<SumIntType::kVar>(t, $chg.$data);
				if ($label.$state)
					$label.$data.write(t);
				if ($cmd.$state)
					writeBlockList<SumIntType::kVar>(t, $cmd.$data);
			}
			return thiz;
		}

	};

	struct Type1AtAnimBegin {

		List<LayerErase> $v1;
		List<LayerErase> $v2;

		auto & read(ByteStream & t) {
			readBlockList<SumIntType::kVar>(t, $v1);
			readBlockList<SumIntType::kVar>(t, $v2);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			writeBlockList<SumIntType::kVar>(t, $v1);
			writeBlockList<SumIntType::kVar>(t, $v2);
			return thiz;
		}

	};

	struct AnimFrameInfo {

		static inline constexpr Sz kSz = 12;
		static inline constexpr Sz kSum = 4;

		static inline constexpr OSD kInfo[]{
			OSD(0x4, 2), // rate
			OSD(0x6, 2), // sum
			OSD(0x8, 2), // beg
			OSD(0xA, 2), // end
		};

		IU16 $rate;
		IU16 $sum;
		IU16 $beg;
		IU16 $end;

		auto addr() const {
			return new Void const * const [kSum] {\
				& $rate,
				& $sum,
				& $beg,
				& $end,
			};
		}
		auto addr() {
			return const_cast<Void * const *>(static_cast<TRR<decltype(thiz)> const &>(thiz).addr());
		}

	};

	struct Anim {

		AnimFrameInfo $info;
		List<Frame> $frame;

		auto & read(ByteStream & t) {
			$StaticBlock::read($info, t);
			$frame.allocThenUse($info.$sum);
			$DynamicBlock::read($frame, t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			$StaticBlock::write($info, t);
			$DynamicBlock::write($frame, t);
			return thiz;
		}

	};

	struct AnimWithLabel : Anim {

		StrBlock<IU16> $label;

		auto & read(ByteStream & t) {
			$label.read(t);
			Anim::read(t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			$label.write(t);
			Anim::write(t);
			return thiz;
		}

	};

	struct AnimWithUnk : Anim {

		Type1AtAnimBegin $block1;

		auto & read(ByteStream & t) {
			if (t.rd<Byte>() != 0x1)
				throw VAMGException("PopcapAnimation", "not 01-block");
			if (t[t.$pos] != 0 || t[t.$pos + 1] != 0)
				log.war("not 0 & 0 at %X", t.$pos);
			$block1.read(t);
			Anim::read(t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			t << IU8(0x1);
			$block1.write(t);
			Anim::write(t);
			return thiz;
		}

	};

	inline constexpr Sz kUNKDataSz(13);

	struct Struct {

		ConstSzArray<Byte, kUNKDataSz> $unk;
		IS16 $originX, $originY;
		List<Image> $img;
		List<AnimWithLabel> $anim;
		AnimWithUnk $main;

		auto & read(ByteStream & t) {
			t >> $unk.$data >> $originX >> $originY;
			readBlockList<SumIntType::kU16>(t, $img);
			readBlockList<SumIntType::kU16>(t, $anim);
			$main.read(t);
			return thiz;
		}
		auto & write(ByteStream & t) const {
			t << $unk.$data << $originX << $originY;
			writeBlockList<SumIntType::kU16>(t, $img);
			writeBlockList<SumIntType::kU16>(t, $anim);
			$main.write(t);
			return thiz;
		}

	};

}
