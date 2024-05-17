#pragma once

# include "../../Type.h"
# include "../../MyType/JSON.h"
# include "../../UtilKit/JSONUtil.h"

# include "../../MyType/ListOfOther.h"
# include "../../MyType/StrList.h"
# include "../../MyType/Path.h"
# include "../../ImageUtil/ImageType.h"

# include "./Struct.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$RipePAMStructInfo {
	struct RangeInfo {
		SzI _begin, _end;

		auto & read($JSON::Arr const & src) {
			src[kThFirst].getX(_begin);
			src[kThSecond].getX(_end);
			return *this;
		}
		auto & write($JSON::Arr & dst) const {
			dst.setNull().add(_begin).add(_end);
			return dst;
		}
	};
	struct LabelRangeInfo {
		Str _label;
		SzI _begin, _end;

		auto & read($JSON::Member const & src) {
			src._key.getX(_label);
			src[kThFirst].getX(_begin);
			src[kThSecond].getX(_end);
			return *this;
		}
		auto & write($JSON::Member & dst) const {
			dst._key.set(_label);
			dst._val.setArr().getArr().add(_begin).add(_end);
			return dst;
		}
	};
	struct ImageInfo {
		Str _src;
		F64 _w, _h;
		F64 _originX, _originY;

		auto & read($JSON::Obj const & src) {
			src["Src"].getX(_src);
			src["Size"][kThFirst].getX(_w);
			src["Size"][kThSecond].getX(_h);
			src["Origin"][kThFirst].getX(_originX);
			src["Origin"][kThSecond].getX(_originY);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Src", _src);
			dst.addArr("Size").last()._val.getArr().add(_w).add(_h);
			dst.addArr("Origin").last()._val.getArr().add(_originX).add(_originY);
			return dst;
		}
	};
	struct FrameInfo {
		F64 _opacity;
		F64 _matrixA, _matrixB, _matrixC, _matrixD, _matrixX, _matrixY;

		auto & read($JSON::Obj const & src) {
			src["Opacity"].getX(_opacity);
			src["Matrix"][kThFirst].getX(_matrixA);
			src["Matrix"][kThSecond].getX(_matrixB);
			src["Matrix"][kThThird].getX(_matrixC);
			src["Matrix"][kThFourth].getX(_matrixD);
			src["Matrix"][kThFifth].getX(_matrixX);
			src["Matrix"][kThSixth].getX(_matrixY);
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.add("Opacity", _opacity);
			dst.addArr("Matrix").last()._val.getArr().add(_matrixA).add(_matrixB).add(_matrixC).add(_matrixD).add(_matrixX).add(_matrixY);
			return dst;
		}
	};
	struct AnimInfo {
		ListB<SzI> _id;
		ListB<SzI> _parent;
		RangeInfo _range;
		ListOfInfo<FrameInfo> _frame;

		auto & read($JSON::Obj const & src) {
			$JSON::getArrForBaseType(src["ID"].getArr(), _id);
			$JSON::getArrForBaseType(src["Parent"].getArr(), _parent);
			_range.read(src["Range"].getArr());
			_frame.read(src["Frame"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			$JSON::setArrForBaseType(_id, dst.addArr("ID").last()._val.getArr());
			$JSON::setArrForBaseType(_parent, dst.addArr("Parent").last()._val.getArr());
			_range.write(dst.addArr("Range").last()._val.getArr());
			_frame.write(dst.addArr("Frame").last()._val.getArr());
			return dst;
		}
	};
	struct StructInfo {
		F64 _originX, _originY;
		SzI _frameRate;
		SzI _frameSum;
		StrList _subName;
		ListOfInfo<LabelRangeInfo> _labelRange;
		ListOfInfo<ImageInfo> _imageInfo;
		ListOfInfo<AnimInfo> _animInfo;

		auto & getThis() { return *this; }

		auto & read($JSON::Obj const & src) {
			src["Origin"][kThFirst].getX(_originX);
			src["Origin"][kThSecond].getX(_originY);
			src["FrameRate"].getX(_frameRate);
			src["FrameSum"].getX(_frameSum);
			$JSON::getArrStr(src["Sub"].getArr(), _subName);
			_labelRange.read(src["Label"].getObj());
			_imageInfo.read(src["Image"].getArr());
			_animInfo.read(src["Anim"].getArr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			dst.addArr("Origin").last()._val.getArr().add(_originX).add(_originY);
			dst.add("FrameRate", _frameRate);
			dst.add("FrameSum", _frameSum);
			$JSON::setArrStr(_subName, dst.addArr("Sub").last()._val.getArr());
			_labelRange.write(dst.addObj("Label").last()._val.getObj());
			_imageInfo.write(dst.addArr("Image").last()._val.getArr());
			_animInfo.write(dst.addArr("Anim").last()._val.getArr());
			return dst;
		}
	};
}
