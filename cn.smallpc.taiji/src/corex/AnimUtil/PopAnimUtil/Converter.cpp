# include "./Converter.h"
# include "../../MyType/Map.h"
# include <cmath>

# include <iostream>
# include "../../../lib/tinyxml/tinyxml.h"
# include "../../../lib/tinyxml/tinystr.h"

namespace $TJ::$AnimUtil::$PopAnimUtil::$Converter {
	struct ChgMatrix {
		F64 _a, _b, _c, _d, _x, _y;

		auto & init() {
			memSet(this, kSzOne);
			_a = _d = 1.0;
			return *this;
		}
		auto & cpy(ChgMatrix const & t) {
			memCpy(this, &t, kSzOne);
			return *this;
		}
		auto & mix($StructInfo::LayerChg const & t) {
			if (t._existPos) {
				_x = t._pos._x;
				_y = t._pos._y;
			}
			if (t._existAngle) {
				_a = cos(t._angle._angle);
				_b = sin(t._angle._angle);
				_c = -sin(t._angle._angle);
				_d = cos(t._angle._angle);
			}
			if (t._existMatrix) {
				_a = t._matrix._a;
				_b = t._matrix._b;
				_c = t._matrix._c;
				_d = t._matrix._d;
			}
			return *this;
		}

		auto & operator=(ChgMatrix const & t) { return cpy(t); }
		auto & operator*=(ChgMatrix const & t) {
			ChgMatrix tmp;
			tmp._a = _a * t._a + _c * t._b;
			tmp._b = _b * t._a + _d * t._b;
			tmp._c = _a * t._c + _c * t._d;
			tmp._d = _b * t._c + _d * t._d;
			tmp._x = _a * t._x + _c * t._y + _x;
			tmp._y = _b * t._x + _d * t._y + _y;
			*this = tmp;
			return *this;
		}
	};
	auto & mixChgMatrix(ChgMatrix & srcChg, ChgMatrix const & newChg) {
		auto tmp = newChg;
		tmp *= srcChg;
		srcChg = tmp;
		return srcChg;
	};
	struct ChgColor {
		F64 _r, _g, _b, _a;

		auto & init() {
			_r = _g = _b = _a = 1.0;
			return *this;
		}

		auto & cpy(ChgColor const & t) {
			memCpy(this, &t, kSzOne);
			return *this;
		}
		auto & load($StructInfo::LayerChg const & t) {
			if (t._existColorRate) {
				_r = t._colorRate._r;
				_g = t._colorRate._g;
				_b = t._colorRate._b;
				_a = t._colorRate._a;
			}
			return *this;
		}

		auto & operator=(ChgColor const & t) { return cpy(t); }
	};
	struct LayerResult {
		Bool _isAnim; SzI _refIdx;
		SzI _frameBeginIdx, _frameSum;
	};
	struct LayerState {
		Bool _isAnim; SzI _refIdx;
		SzI _duration;
		ChgMatrix _matrix;
		ChgColor _color;
		Bool _isNewFrameNow, _nextErase;
		TiXmlElement * _ptr = nullptr;
	};
	static Str const kXFLContent("PROXY-CS5");
	static constexpr SzI kStdImgSz(1200);
	static F64 gResScaleRate(1.0);
	Void WriteXMLFile(TiXmlNode * node, Path const & path) {
		// will clear document after output
		TiXmlDocument doc;
		TiXmlPrinter printer;
		doc.LinkEndChild(node);
		printer.SetIndent("\t");
		doc.Accept(&printer);
		StrRef str(printer.CStr());
		$DevUtil::WriteData(path, reinterpret_cast<Byte const *>(str._str), str._len);
		doc.Clear();
		return;
	}
	Void ReadXMLFile(TiXmlDocument & doc, Path const & path) {
		doc.LoadFile(path.getFullPath()._str, TIXML_ENCODING_UTF8);
		return;
	}
	auto SetShell(Ch const * const & value, TiXmlElement * element) {
		auto xShell = new TiXmlElement(value);
		xShell->LinkEndChild(element);
		return xShell;
	}
	struct XFLOtherInfo {
		Byte _unk[13];
		F64 _originX, _originY;
		Map<Str, $ImageUtil::ImageSize> _imageSz;
		Map<Str, Str> _imageMapper;
		Map<Str, Str> _subAnimMapper;

		auto & read($JSON::Obj const & src) {
			auto unkSz = kSzZero;
			readHexStr(src["UnK"].getStr(), _unk, unkSz);
			if (unkSz != 13) {
				// todo
			}
			src["Origin"][kThFirst].getX(_originX);
			src["Origin"][kThSecond].getX(_originY);
			for_criter(member, src["ImageSize"].getObj())
				_imageSz.addByKV(member._key.get(), $ImageUtil::ImageSize(member[kThFirst].getUInt(), member[kThSecond].getUInt()));
			for_criter(member, src["ImageMapper"].getObj())
				_imageMapper.addByKV(member._key.get(), member._val.getStr());
			for_criter(member, src["SubAnimMapper"].getObj())
				_subAnimMapper.addByKV(member._key.get(), member._val.getStr());
			return *this;
		}
		auto & write($JSON::Obj & dst) const {
			writeHexStr(_unk, 13, dst.add("UnK", $JSON::ValType::kStr).getLast()._val.getStr());
			dst.addArr("Origin").getLast()._val.getArr().add(_originX).add(_originY);
			auto & dstImageSize = dst.addObj("ImageSize").getLast()._val.getObj();
			for_criter(member, _imageSz)
				dstImageSize.add(member._key, $JSON::Arr().add(member._val._w).add(member._val._h));
			auto & dstImageMapper = dst.addObj("ImageMapper").getLast()._val.getObj();
			for_criter(member, _imageMapper)
				dstImageMapper.add(member._key, member._val);
			auto & dstSubAnimMapper = dst.addObj("SubAnimMapper").getLast()._val.getObj();
			for_criter(member, _subAnimMapper)
				dstSubAnimMapper.add(member._key, member._val);
			return dst;
		}
	};
	namespace $CnvToXFL {
		static auto GenXTransformationPointWithShell(F64 const & originX, F64 const & originY) {
			auto xPoint = new TiXmlElement("Point");
			xPoint->SetAttribute("x", Str().printFmtByAppend("%.6f", originX)._str);
			xPoint->SetAttribute("y", Str().printFmtByAppend("%.6f", originY)._str);
			return SetShell("transformationPoint", xPoint);
		}
		static auto GenXMatrixWithShell(ChgMatrix const & matrix) {
			auto xMatrix = new TiXmlElement("Matrix");
			xMatrix->SetAttribute("a", Str().printFmtByAppend("%.6f", matrix._a)._str);
			xMatrix->SetAttribute("b", Str().printFmtByAppend("%.6f", matrix._b)._str);
			xMatrix->SetAttribute("c", Str().printFmtByAppend("%.6f", matrix._c)._str);
			xMatrix->SetAttribute("d", Str().printFmtByAppend("%.6f", matrix._d)._str);
			xMatrix->SetAttribute("tx", Str().printFmtByAppend("%.6f", matrix._x)._str);
			xMatrix->SetAttribute("ty", Str().printFmtByAppend("%.6f", matrix._y)._str);
			return SetShell("matrix", xMatrix);
		}
		static auto GenXColorWithShell(ChgColor const & color) {
			auto xColor = new TiXmlElement("Color");
			xColor->SetAttribute("redMultiplier", Str().printFmtByAppend("%.6f", color._r)._str);
			xColor->SetAttribute("greenMultiplier", Str().printFmtByAppend("%.6f", color._g)._str);
			xColor->SetAttribute("blueMultiplier", Str().printFmtByAppend("%.6f", color._b)._str);
			xColor->SetAttribute("alphaMultiplier", Str().printFmtByAppend("%.6f", color._a)._str);
			return SetShell("color", xColor);
		}
		static auto GenXMatrixWithShellForBitmapInstance(F64 const & scaleX, F64 const & scaleY, F64 const & posX, F64 const & posY) {
			auto xMatrix = new TiXmlElement("Matrix");
			xMatrix->SetAttribute("a", Str().printFmtByAppend("%.6f", scaleX * gResScaleRate)._str);
			xMatrix->SetAttribute("d", Str().printFmtByAppend("%.6f", scaleY * gResScaleRate)._str);
			xMatrix->SetAttribute("tx", Str().printFmtByAppend("%.6f", posX)._str);
			xMatrix->SetAttribute("ty", Str().printFmtByAppend("%.6f", posY)._str);
			return SetShell("matrix", xMatrix);
		}

		static auto GenXFrameOnlySymbolInstance(SzI const & idx, SzI const & duration, Str const & library, ChgMatrix const & matrix, ChgColor const & color) {
			auto xFrame = new TiXmlElement("DOMFrame");
			auto xSymbolInstance = new TiXmlElement("DOMSymbolInstance");
			xFrame->SetAttribute("index", idx);
			xFrame->SetAttribute("duration", duration);
			xSymbolInstance->SetAttribute("libraryItemName", library._str);
			xSymbolInstance->LinkEndChild(GenXMatrixWithShell(matrix));
			xSymbolInstance->LinkEndChild(GenXColorWithShell(color));
			xFrame->LinkEndChild(SetShell("elements", xSymbolInstance));
			return xFrame;
		}

		static auto GenXEmptyFrame(SzI const & idx, SzI const & duration) {
			auto xFrame = new TiXmlElement("DOMFrame");
			xFrame->SetAttribute("index", idx);
			xFrame->SetAttribute("duration", duration);
			return xFrame;
		}
		static auto GenXLabelFrame(Str const & label, SzI const & idx, SzI const & duration) {
			auto xFrame = GenXEmptyFrame(idx, duration);
			xFrame->SetAttribute("name", label._str);
			return xFrame;
		}
		static auto GenXCmdFrame(SzI const & idx, Str const & cmd) {
			auto xFrame = new TiXmlElement("DOMFrame");
			auto xScript = new TiXmlElement("script");
			auto xScriptText = new TiXmlText("");
			xFrame->SetAttribute("index", idx);
			xScriptText->SetCDATA(kTrue);
			xScriptText->SetValue(cmd._str);
			xScript->LinkEndChild(xScriptText);
			xFrame->LinkEndChild(SetShell("Actionscript", xScript));
			return xFrame;
		}

		static auto GenImageXTimeLineWithShell(Str const & name, $StructInfo::ImageInfo const & image) {
			auto xTimeLine = new TiXmlElement("DOMTimeline");
			auto xFrame = new TiXmlElement("DOMFrame");
			auto xBitmapInstance = new TiXmlElement("DOMBitmapInstance");
			xTimeLine->SetAttribute("name", name._str);
			xFrame->SetAttribute("index", static_cast<int>(kThFirst));
			xBitmapInstance->SetAttribute("libraryItemName", image._src._str);
			xBitmapInstance->LinkEndChild(GenXMatrixWithShellForBitmapInstance(image._scaleX, image._scaleY, image._posX, image._posY));
			xBitmapInstance->LinkEndChild(GenXTransformationPointWithShell(-image._posX, -image._posY));
			xFrame->LinkEndChild(SetShell("elements", xBitmapInstance));
			xTimeLine->LinkEndChild(
				SetShell("layers",
						 SetShell("DOMLayer",
								  SetShell("frames", xFrame))));
			return SetShell("timeline", xTimeLine);
		}
		static auto GenAnimXTimeLineWithShellori(Str const & name, ListP<$StructInfo::Frame> const & frameList, StrList const & imageIDList, StrList const & subAnimLabelList) {
			auto xTimeLine = new TiXmlElement("DOMTimeline");
			auto xLayerShell = new TiXmlElement("layers");
			xTimeLine->SetAttribute("name", name._str);
			SzI layerSum = kSzZero;
			for_criter(frame, frameList) {
				if (frame._existAppend)
					layerSum += frame._append._sz;
					//for_criter(layer, frame._append)
						//if (layerSum < static_cast<SzSI>(layer._idx))
							//layerSum = layer._idx;
			}
			ListB<Bool> layerIsEnable;
			Map<SzI, LayerState> lastLayerStateList;
			Map<SzI, TiXmlElement *> xLayerMap, xLayerFrameShellMap;
			layerIsEnable.allocThenNew(layerSum);
			xLayerMap.allocThenNew(layerSum);
			for_cnt(frameIdx, kThFirst, frameList._sz) {
				auto & frame = frameList[frameIdx];
				if (frame._existErase) {
					for_criter(layer, frame._erase)
						lastLayerStateList.getValByKey(layer._idx)._nextErase = kTrue;
				}
				if (frame._existAppend) {
					for_criter(layer, frame._append) {
						layerIsEnable[layer._idx] = kTrue;
						auto & state = lastLayerStateList.addByKey(layer._idx).getLast()._val;
						state._refIdx = layer._refIdx;
						state._isAnim = layer._isAnim;
						state._duration = frameIdx;
						state._matrix.init();
						state._color.init();
						state._nextErase = kFalse;
						auto xLayer = new TiXmlElement("DOMLayer");
						auto xFrameShell = new TiXmlElement("frames");
						if (frameIdx > kThFirst)
							xFrameShell->LinkEndChild(GenXEmptyFrame(kThFirst, frameIdx));
						xLayer->SetAttribute("name", Str().printFmtByAppend("L_%u", layer._idx)._str);
						xLayerMap[layer._idx]._key = layer._idx;
						xLayerMap[layer._idx]._val = xLayer;
						xLayer->LinkEndChild(xFrameShell);
						xLayerFrameShellMap.addByKV(layer._idx, xFrameShell);
					}
				}
				for_cnt(i, kThFirst, layerIsEnable._sz)
					if (layerIsEnable[i])
						lastLayerStateList.getValByKey(i)._isNewFrameNow = kFalse;
				if (frame._existChg) {
					for_criter(layer, frame._chg) {
						auto & state = lastLayerStateList.getValByKey(layer._idx);
						state._matrix.mix(layer);
						state._color.load(layer);
						state._isNewFrameNow = kTrue;
					}
				}/*
				for_cnt(i, kThFirst, layerIsEnable._sz) {
					if (layerIsEnable[i]) {
						auto & state = layerStateList.getValByKey(i);
						if ((state.mIsNewFrameNow || state.mNextErase) && state.ptr != nullptr)
							state.ptr->SetAttribute("duration", state.mDuration);
						if (state.mNextErase)
							layerIsEnable[i] = kFalse;
						if (state.mIsNewFrameNow) {
							state.mDuration = kSzOne;
							state.ptr = GenXFrameOnlySymbolInstance(
								frameIdx, state.mDuration,
								state._isAnim ? subAnimLabelList[state._refIdx] : imageIDList[state._refIdx],
								state._matrix, state.mColor
								);
							xLayerFrameShellList.getValByKey(i)->LinkEndChild(state.ptr);
						} else
							++state.mDuration;
						state.mIsFirstFrame = kFalse;
					}
				}*/
				for_cnt(i, kThFirst, layerIsEnable._sz) {
					if (layerIsEnable[i]) {
						auto & state = lastLayerStateList.getValByKey(i);
						if ((state._isNewFrameNow || state._nextErase) && state._ptr != nullptr)
							state._ptr->SetAttribute("duration", state._duration);
						if (state._nextErase)
							layerIsEnable[i] = kFalse;
						if (state._isNewFrameNow) {
							state._duration = kSzOne;
							state._ptr = GenXFrameOnlySymbolInstance(
								frameIdx, state._duration,
								state._isAnim ? subAnimLabelList[state._refIdx] : imageIDList[state._refIdx],
								state._matrix, state._color
							);
							xLayerFrameShellMap.getValByKey(i)->LinkEndChild(state._ptr);
						} else
							++state._duration;
					}
				}
			}
			for_cnt(i, kThFirst, layerIsEnable._sz) {
				if (layerIsEnable[i]) {
					auto & state = lastLayerStateList.getValByKey(i);
					if (state._ptr != nullptr)
						state._ptr->SetAttribute("duration", state._duration);
				}
			}
			for_cnt_lt(idx, layerSum, kThFirst) {
				auto & layer = xLayerMap[idx - kThSecond]._val;
				xLayerShell->LinkEndChild(layer);
			}
			xTimeLine->LinkEndChild(xLayerShell);
			return SetShell("timeline", xTimeLine);
		}

		static auto GenAnimXTimeLineWithShell(Str const & name, ListP<$StructInfo::Frame> const & frameList, StrList const & imageIDList, StrList const & subAnimLabelList) {
			auto layerSum = kSzZero;
			for_criter(frame, frameList) {
				if (frame._existAppend)
					layerSum += frame._append._sz;
					//for_criter(layer, frame._append)
						//if (layerSum < static_cast<SzSI>(layer._idx))
							//layerSum = layer._idx;
			}
			ListB<Bool> layerIsEnable;
			ListP<LayerState> layerStateList;
			ListP<LayerResult> layerResultList;
			ListP<ListB<TiXmlElement *>> xLayerFrameShellList;
			layerIsEnable.allocThenNew(layerSum);
			layerStateList.allocThenNew(layerSum);
			layerResultList.allocThenNew(layerSum);
			xLayerFrameShellList.allocThenNew(layerSum);
			for_cnt(frameIdx, kThFirst, frameList._sz) {
				auto & frame = frameList[frameIdx];
				if (frame._existErase) {
					for_criter(layer, frame._erase)
						layerStateList[layer._idx]._nextErase = kTrue;
				}
				if (frame._existAppend) {
					for_criter(layer, frame._append) {
						layerIsEnable[layer._idx] = kTrue;
						auto & state = layerStateList[layer._idx];
						state._refIdx = layer._refIdx;
						state._isAnim = layer._isAnim;
						state._duration = frameIdx;
						state._matrix.init();
						state._color.init();
						state._nextErase = kFalse;
						layerResultList[layer._idx]._isAnim = layer._isAnim;
						layerResultList[layer._idx]._refIdx = layer._refIdx;
						layerResultList[layer._idx]._frameBeginIdx = frameIdx;
						xLayerFrameShellList[layer._idx].alloc(frameList._sz - frameIdx);
					}
				}
				for_cnt(i, kThFirst, layerIsEnable._sz)
					if (layerIsEnable[i])
						layerStateList[i]._isNewFrameNow = kFalse;
				if (frame._existChg) {
					for_criter(layer, frame._chg) {
						auto & state = layerStateList[layer._idx];
						state._matrix.mix(layer);
						state._color.load(layer);
						state._isNewFrameNow = kTrue;
					}
				}
				for_cnt(i, kThFirst, layerIsEnable._sz) {
					if (layerIsEnable[i]) {
						auto & state = layerStateList[i];
						if ((state._isNewFrameNow || state._nextErase) && state._ptr != nullptr)
							state._ptr->SetAttribute("duration", state._duration);
						if (state._isNewFrameNow) {
							state._duration = kSzOne;
							state._ptr = GenXFrameOnlySymbolInstance(
								frameIdx, state._duration,
								state._isAnim ? subAnimLabelList[state._refIdx] : imageIDList[state._refIdx],
								state._matrix, state._color
							);
							xLayerFrameShellList[i].add(state._ptr);
						} else
							++state._duration;
						if (state._nextErase) {
							layerIsEnable[i] = kFalse;
							xLayerFrameShellList[i].eraseSurplusCap();
							layerResultList[i]._frameSum = frameIdx - layerResultList[i]._frameBeginIdx;
						}
					}
				}
			}
			for_cnt(i, kThFirst, layerIsEnable._sz) {
				if (layerIsEnable[i]) {
					xLayerFrameShellList[i].eraseSurplusCap();
					layerResultList[i]._frameSum = frameList._sz - layerResultList[i]._frameBeginIdx;
					auto & state = layerStateList[i];
					if (state._ptr != nullptr)
						state._ptr->SetAttribute("duration", state._duration);
				}
			}
			auto xTimeLine = new TiXmlElement("DOMTimeline");
			auto xLayerShell = new TiXmlElement("layers");
			xTimeLine->SetAttribute("name", name._str);

			/*for_riter(xxx, xLayerFrameShellList) {
				for_riter(bbb, xxx) {
					delete bbb;
				}
			}*/

			for_cnt_lt(cnt, xLayerFrameShellList._sz, kThFirst) {
				SzI idx = cnt - kThSecond;
				auto xLayer = new TiXmlElement("DOMLayer");
				xLayer->SetAttribute("name", Str().printFmtByAppend("L_%u", idx)._str);
				auto xFrameShell = new TiXmlElement("frames");
				if (layerResultList[idx]._frameBeginIdx > kThFirst)
					xFrameShell->LinkEndChild(GenXEmptyFrame(kThFirst, layerResultList[idx]._frameBeginIdx));
				for_riter(layerShell, xLayerFrameShellList[idx])
					xFrameShell->LinkEndChild(layerShell);
				xLayer->LinkEndChild(xFrameShell);
				xLayerShell->LinkEndChild(xLayer);
			}
			xTimeLine->LinkEndChild(xLayerShell);
			return SetShell("timeline", xTimeLine);
		}

		static auto GenOtherXLayer(ListP<$StructInfo::Frame> const & frameList, TiXmlElement *& xLabels, TiXmlElement *& xActions, TiXmlElement *& xAudio) { // labels/actions/audio
			auto xLabelsFrameShell = new TiXmlElement("frames");
			auto xActionsFrameShell = new TiXmlElement("frames");
			auto xAudioFrameShell = new TiXmlElement("frames");
			Str curLabel; SzI curLabelBeginIdx, lastActionIdx = kThFirst, lastAudioIdx = kThFirst;
			if (!frameList[kThFirst]._existLabel) {
				curLabel.cpy("");
				curLabelBeginIdx = kThFirst;
			}
			for_cnt(frameIdx, kThFirst, frameList._sz) {
				auto & frame = frameList[frameIdx];
				if (frame._existLabel) {
					if (frameIdx > kThFirst)
						xLabelsFrameShell->LinkEndChild(GenXLabelFrame(curLabel, curLabelBeginIdx, frameIdx - curLabelBeginIdx));
					curLabel = frame._label;
					curLabelBeginIdx = frameIdx;
				}
				if (frame._existStop || frame._existCmd) {
					Str actionCmd, audioCmd;
					auto isFirstActionCmd = !frame._existStop, isFirstAudioCmd = kTrue;
					if (frame._existStop)
						actionCmd.cat("stop();");
					for_criter(fsCmd, frame._cmd) {
						if (fsCmd._cmd == "PlaySample" || fsCmd._cmd == "playsample") {
							if (!isFirstAudioCmd)
								audioCmd.add('\n');
							else isFirstAudioCmd = kFalse;
							audioCmd.printFmtByAppend("fscommand(\"%s\", \"%s\");", fsCmd._cmd._str, fsCmd._arg._str);
						} else {
							if (!isFirstActionCmd)
								actionCmd.add('\n');
							else isFirstActionCmd = kFalse;
							if (fsCmd._arg.isNull())
								actionCmd.printFmtByAppend("fscommand(\"%s\");", fsCmd._cmd._str);
							else actionCmd.printFmtByAppend("fscommand(\"%s\", \"%s\");", fsCmd._cmd._str, fsCmd._arg._str);
						}
					}
					if (!isFirstActionCmd) {
						if (lastActionIdx != frameIdx - lastActionIdx)
							xActionsFrameShell->LinkEndChild(GenXEmptyFrame(lastActionIdx, frameIdx - lastActionIdx));
						xActionsFrameShell->LinkEndChild(GenXCmdFrame(frameIdx, actionCmd));
						lastActionIdx = frameIdx + kThSecond;
					}
					if (!isFirstAudioCmd) {
						if (lastAudioIdx != frameIdx - lastAudioIdx)
							xAudioFrameShell->LinkEndChild(GenXEmptyFrame(lastAudioIdx, frameIdx - lastAudioIdx));
						xAudioFrameShell->LinkEndChild(GenXCmdFrame(frameIdx, audioCmd));
						lastAudioIdx = frameIdx + kThSecond;
					}
				}
			}
			if (frameList._sz > kSzZero)
				xLabelsFrameShell->LinkEndChild(GenXLabelFrame(curLabel, curLabelBeginIdx, frameList._sz - curLabelBeginIdx));
			xLabels = new TiXmlElement("DOMLayer");
			xLabels->SetAttribute("name", "labels");
			xLabels->LinkEndChild(xLabelsFrameShell);
			xActions = new TiXmlElement("DOMLayer");
			xActions->SetAttribute("name", "actions");
			xActions->LinkEndChild(xActionsFrameShell);
			xAudio = new TiXmlElement("DOMLayer");
			xAudio->SetAttribute("name", "audio");
			xAudio->LinkEndChild(xAudioFrameShell);
			return;
		}

		static auto GenXSymbolItem(Str const & name, TiXmlElement * xTimeLine) {
			auto xSymbolItem = new TiXmlElement("DOMSymbolItem");
			xSymbolItem->SetAttribute("name", name._str);
			xSymbolItem->LinkEndChild(xTimeLine);
			return xSymbolItem;
		}
		static auto GenXBitmapItem(Str const & path) {
			auto xBitmapItem = new TiXmlElement("DOMBitmapItem");
			xBitmapItem->SetAttribute("name", path._str);
			xBitmapItem->SetAttribute("href", Str(path).cat(".png")._str);
			return xBitmapItem;
		}
		static auto GenXInclude(Str const & href) {
			auto xInclude = new TiXmlElement("Include");
			xInclude->SetAttribute("href", href._str);
			return xInclude;
		}
		static auto MakeFlashDocInfo(TiXmlElement * xElement) {
			xElement->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			xElement->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			return xElement;
		}
		Void Cnv($StructInfo::Struct const & pam, Path const & xflDir, SzI const & imgSz) {
		# if ! defined DISABLE_PAM_FUNC_ON_ANDROID
			gResScaleRate = static_cast<F64>(kStdImgSz) / static_cast<F64>(imgSz);
			XFLOtherInfo otherInfo;
			memCpy(&(otherInfo._unk[0]), &(pam._unk[0]), 13);
			otherInfo._originX = pam._originX;
			otherInfo._originY = pam._originY;
			Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, xflDir);
			$DevUtil::makeDir(Path("LIBRARY"));
			auto xDocument = new TiXmlElement("DOMDocument");
			auto xSymbols = new TiXmlElement("symbols");
			auto xMedia = new TiXmlElement("media");
		# if ! defined VER_ULTIMATE
			xDocument->SetAttribute("__ABOUT__", "This XFL is convert from PAM file, By TaiJi.");
		# endif
			StrList imageIDList, subAnimLabelList;
			for_cnt(idx, kThFirst, pam._image._sz) {
				auto const & image = pam._image[idx];
				Str id; id.printFmtByAppend("M_%u", idx);
				otherInfo._imageSz.addByKV(image._id, image._sz);
				otherInfo._imageMapper.addByKV(id, image._id);
				xSymbols->LinkEndChild(GenXInclude(Str(id).cat(".xml")));
				xMedia->LinkEndChild(GenXBitmapItem(image._src));
				WriteXMLFile(MakeFlashDocInfo(GenXSymbolItem(id, GenImageXTimeLineWithShell(id, image))), Path("LIBRARY").add(Str(id).cat(".xml")));
				imageIDList.add(id);
			}
			for_cnt(idx, kThFirst, pam._subAnim._sz) {
				auto const & subAnim = pam._subAnim[idx];
				Str id; id.printFmtByAppend("A_%u", idx);
				otherInfo._subAnimMapper.addByKV(id, subAnim._label);
				xSymbols->LinkEndChild(GenXInclude(Str(id).cat(".xml")));
				WriteXMLFile(MakeFlashDocInfo(GenXSymbolItem(id, GenAnimXTimeLineWithShell(id, subAnim._frame, imageIDList, subAnimLabelList))), Path("LIBRARY").add(Str(id).cat(".xml")));
				subAnimLabelList.add(id);
			}
			xSymbols->LinkEndChild(GenXInclude(Str("A_Main.xml")));
			WriteXMLFile(MakeFlashDocInfo(GenXSymbolItem(StrRef("A_Main"), GenAnimXTimeLineWithShell(StrRef("A_Main"), pam._mainAnim._frame, imageIDList, subAnimLabelList))), Path("LIBRARY").add("A_Main.xml"));
			xDocument->LinkEndChild(xMedia);
			xDocument->LinkEndChild(xSymbols);
			{
				TiXmlElement * xLabelsLayer, * xActionsLayer, * xAudioLayer;
				GenOtherXLayer(pam._mainAnim._frame, xLabelsLayer, xActionsLayer, xAudioLayer);
				auto xTimeLine = new TiXmlElement("DOMTimeline");
				auto xLayerShell = new TiXmlElement("layers");
				auto xAnimLayer = new TiXmlElement("DOMLayer");
				auto xAnimFrame = new TiXmlElement("DOMFrame");
				auto xAnimSymbolInstance = new TiXmlElement("DOMSymbolInstance");

				xTimeLine->SetAttribute("name", "Anim");
				xAnimLayer->SetAttribute("name", "animation");
				xAnimFrame->SetAttribute("index", 0);
				xAnimFrame->SetAttribute("duration", pam._mainAnim._info._sz);
				xAnimSymbolInstance->SetAttribute("libraryItemName", "A_Main");

				xAnimFrame->LinkEndChild(SetShell("elements", xAnimSymbolInstance));
				xAnimLayer->LinkEndChild(SetShell("frames", xAnimFrame));
				xLayerShell->LinkEndChild(xLabelsLayer);
				xLayerShell->LinkEndChild(xActionsLayer);
				xLayerShell->LinkEndChild(xAudioLayer);
				xLayerShell->LinkEndChild(xAnimLayer);
				xTimeLine->LinkEndChild(xLayerShell);
				xDocument->LinkEndChild(SetShell("timelines", xTimeLine));
			}
			xDocument->SetAttribute("frameRate", pam._mainAnim._info._rate);
			xDocument->SetAttribute("width", 0);
			xDocument->SetAttribute("height", 0);
			xDocument->SetAttribute("xflVersion", "2.97");
			WriteXMLFile(MakeFlashDocInfo(xDocument), Path("DOMDocument.xml"));
			$DevUtil::WriteData(Path("Anim.xfl"), reinterpret_cast<Byte const *>(kXFLContent._str), kXFLContent._len);
			$JSON::write(Path("OtherInfo.json"), otherInfo.write($JSON::Obj().setNull()));
			$DevUtil::chgDirThenFree(parentDir);
			return;
		# endif
		}
	}
	namespace $CnvFromXFL {
		static auto & LoadXMatrixWithShell(TiXmlElement const & xElement, ChgMatrix & matrix) {
			auto xMatrix = *xElement.FirstChildElement("Matrix");
			xMatrix.QueryDoubleAttribute("a", &matrix._a);
			xMatrix.QueryDoubleAttribute("b", &matrix._b);
			xMatrix.QueryDoubleAttribute("c", &matrix._c);
			xMatrix.QueryDoubleAttribute("d", &matrix._d);
			xMatrix.QueryDoubleAttribute("tx", &matrix._x);
			xMatrix.QueryDoubleAttribute("ty", &matrix._y);
			return matrix;
		}
		static auto & LoadXColorWithShell(TiXmlElement const & xElement, ChgColor & color) {
			auto xColor = *xElement.FirstChildElement("Color");
			xColor.QueryDoubleAttribute("redMultiplier", &color._r);
			xColor.QueryDoubleAttribute("greenMultiplier", &color._g);
			xColor.QueryDoubleAttribute("blueMultiplier", &color._b);
			xColor.QueryDoubleAttribute("alphaMultiplier", &color._a);
			return color;
		}

		static auto & LoadImageXTimeLine(TiXmlElement const & xTimeLine, Str & name, $StructInfo::ImageInfo & image) {
			name.cpy(xTimeLine.Attribute("name"));
			auto xDOMBitmapInstance = *xTimeLine
				.FirstChildElement()->FirstChildElement()
				->FirstChildElement()->FirstChildElement()
				->FirstChildElement()->FirstChildElement();
			image._src.cpy(xDOMBitmapInstance.Attribute("libraryItemName"));
			//auto xTransformationPoint = xDOMBitmapInstance->FirstChildElement("matrix")->FirstChildElement("Matrix");
			//Str(xTransformationPoint->Attribute("x")).scanFmt("%lf", image._posX);
			//Str(xTransformationPoint->Attribute("y")).scanFmt("%lf", image._posY);
			auto xMatrix = *xDOMBitmapInstance.FirstChildElement("matrix")->FirstChildElement("Matrix");
			xMatrix.QueryDoubleAttribute("a", &image._scaleX);
			xMatrix.QueryDoubleAttribute("d", &image._scaleY);
			xMatrix.QueryDoubleAttribute("tx", &image._posX);
			xMatrix.QueryDoubleAttribute("ty", &image._posY);
			image._scaleX /= gResScaleRate;
			image._scaleY /= gResScaleRate;
			return image;
		}
		static auto LoadXFrameOnlySymbolInstance(TiXmlElement const & xFrame, SzI & idx, SzI & duration, Str & library, ChgMatrix & matrix, ChgColor & color) {
			duration = kSzOne; matrix.init(); color.init();
			xFrame.QueryUnsignedAttribute("index", &idx);
			xFrame.QueryUnsignedAttribute("duration", &duration);
			auto xDOMSymbolInstance = xFrame.FirstChildElement("elements")->FirstChildElement("DOMSymbolInstance");
			library.cpy(xDOMSymbolInstance->Attribute("libraryItemName"));
			if (xDOMSymbolInstance->FirstChildElement("matrix") != nullptr)
				LoadXMatrixWithShell(*xDOMSymbolInstance->FirstChildElement("matrix"), matrix);
			if (xDOMSymbolInstance->FirstChildElement("color") != nullptr)
				LoadXColorWithShell(*xDOMSymbolInstance->FirstChildElement("color"), color);
			return;
		}
		static auto LoadAnimXTimeLine(TiXmlElement const & xTimeLine, Str & name, ListP<$StructInfo::Frame> & frameList, SzI & frameSum) {
			name.cpy(xTimeLine.Attribute("name"));
			auto xLayerShell = xTimeLine.FirstChildElement("layers");
			ListB<TiXmlElement const *> xLayerList;
			SzI layerSum = kSzZero;
			frameSum = kSzZero;
			for (auto xLayer = xLayerShell->FirstChildElement("DOMLayer"); xLayer != nullptr; xLayer = xLayer->NextSiblingElement()) {
				xLayerList.add(xLayer);
				++layerSum;
				auto xFrame = xLayer->FirstChildElement("frames")->LastChild("DOMFrame")->ToElement();
				SzI thisFrameIdx = kThFirst, thisFrameDuration = kSzOne;
				xFrame->QueryUnsignedAttribute("index", &thisFrameIdx);
				xFrame->QueryUnsignedAttribute("duration", &thisFrameDuration);
				if (frameSum < thisFrameIdx + thisFrameDuration)
					frameSum = thisFrameIdx + thisFrameDuration;
			}
			frameList.allocThenNew(frameSum);
			for_cnt(layerIdx, kThFirst, layerSum) {
				auto & xLayer = xLayerList[layerSum - kThSecond - layerIdx];
				wLog("\r%.9s", xLayer->Attribute("name"));
				SzI frameIdxOfBeErase;
				auto xFrameShell = xLayer->FirstChildElement("frames");
				auto xFirstRealFrame = xFrameShell->FirstChildElement("DOMFrame");
				while (xFirstRealFrame != nullptr) {
					if (!(xFirstRealFrame->NoChildren() || xFirstRealFrame->FirstChildElement("elements")->NoChildren()))
						break;
					xFirstRealFrame = xFirstRealFrame->NextSiblingElement();
				}
				if (xFirstRealFrame == nullptr) {
					wWarn("this layer cann't found valid-frame.");
				} else {
					auto firstFrameIdx = 0;
					{
						SzI frameIdx, frameDuration; Str library; ChgMatrix matrix; ChgColor color;
						LoadXFrameOnlySymbolInstance(*xFirstRealFrame, frameIdx, frameDuration, library, matrix, color);
						auto & frame = frameList[frameIdx];
						frame._existAppend = kTrue;
						auto & layerAppend = frame._append.add().getLast();
						layerAppend._idx = layerIdx;
						goif(library[kThFirst] == 'A')
							layerAppend._isAnim = kTrue;
						elif(library[kThFirst] == 'M')
							layerAppend._isAnim = kFalse;
						library.scanFmt(kThThird, "%u", &layerAppend._refIdx);
						firstFrameIdx = frameIdx;
					}
					for (auto xFrame = xFirstRealFrame; xFrame != nullptr; xFrame = xFrame->NextSiblingElement()) {
						SzI frameIdx, frameDuration; Str library; ChgMatrix matrix; ChgColor color;
						LoadXFrameOnlySymbolInstance(*xFrame, frameIdx, frameDuration, library, matrix, color);
						frameIdxOfBeErase = frameIdx + frameDuration;
						auto & frame = frameList[frameIdx];
						frame._existChg = kTrue;
						auto & layerChg = frame._chg.add().getLast();
						layerChg._idx = layerIdx;
						layerChg._existMatrix = kTrue;
						layerChg._existPos = kTrue;
						layerChg._existColorRate = kTrue;
						layerChg._matrix._a = matrix._a;
						layerChg._matrix._b = matrix._b;
						layerChg._matrix._c = matrix._c;
						layerChg._matrix._d = matrix._d;
						layerChg._pos._x = matrix._x;
						layerChg._pos._y = matrix._y;
						layerChg._colorRate._r = color._r;
						layerChg._colorRate._g = color._g;
						layerChg._colorRate._b = color._b;
						layerChg._colorRate._a = color._a;
						/*if (library == "A_20" || library == "A_28" || library == "A_26") {
							layerChg._colorRate._a = abs(50 - (int)((frameIdx - firstFrameIdx) % 100)) * 0.02;
						}*/
					}
					if (frameIdxOfBeErase < frameSum) {
						auto & frame = frameList[frameIdxOfBeErase];
						frame._existErase = kTrue;
						auto & layerErase = frame._erase.add().getLast();
						layerErase._idx = layerIdx;
						layerErase._unKnown = 0u;
					}
				}
			}
			wLogLn("");
			return;
		}

		static auto & AnalysisFSCmdStr(Str const & src, ListP<$StructInfo::FSCmd> & dst) {
			StrList cmdStrList; cmdStrList.read(src, src.cnt(';'), ';');
			for_criter(cmdStr, cmdStrList) {
				if (cmdStr.existSub(StrRef("fscommand"))) {
					auto & cmd = dst.add().getLast();
					auto pos = cmdStr.find('('); ++pos;
					while (cmdStr[pos++] != '\"');
					for_cnt(pos2, pos, cmdStr._len)
						if (cmdStr[pos2] == '\"') {
							cmd._cmd.cpy(cmdStr, pos, pos2 - pos);
							break;
						}
					if (!cmdStr.find(',', pos))
						cmd._arg.setNull();
					else {
						++pos;
						while (cmdStr[pos++] != '\"');
						for_cnt(pos2, pos, cmdStr._len)
							if (cmdStr[pos2] == '\"') {
								cmd._arg.cpy(cmdStr, pos, pos2 - pos);
								break;
							}
					}
				}
			}
			return dst;
		}

		static auto LoadOtherLayer(TiXmlElement const & xLabels, TiXmlElement const & xActions, TiXmlElement const & xAudio, ListP<$StructInfo::Frame> & frameList) {
			for (auto xFrame = xLabels.FirstChildElement("frames")->FirstChildElement("DOMFrame"); xFrame != nullptr; xFrame = xFrame->NextSiblingElement()) {
				SzI index;
				xFrame->QueryUnsignedAttribute("index", &index);
				frameList[index]._existLabel = kTrue;
				frameList[index]._label.cpy(xFrame->Attribute("name"));
			}
			for (auto xFrame = xActions.FirstChildElement("frames")->FirstChildElement("DOMFrame"); xFrame != nullptr; xFrame = xFrame->NextSiblingElement()) {
				if (!xFrame->NoChildren() && StrRef(xFrame->FirstChildElement()->Value()) == "Actionscript") {
					SzI index; Str allCmd;
					xFrame->QueryUnsignedAttribute("index", &index);
					allCmd.cpy(xFrame->FirstChildElement("Actionscript")->FirstChildElement("script")->GetText());
					if (allCmd.existSub(StrRef("stop();")))
						frameList[index]._existStop = kTrue;
					if (allCmd.existSub(StrRef("fscommand"))) {
						frameList[index]._existCmd = kTrue;
						AnalysisFSCmdStr(allCmd, frameList[index]._cmd);
					}
				}
			}
			for (auto xFrame = xAudio.FirstChildElement("frames")->FirstChildElement("DOMFrame"); xFrame != nullptr; xFrame = xFrame->NextSiblingElement()) {
				if (!xFrame->NoChildren() && StrRef(xFrame->FirstChildElement()->Value()) == "Actionscript") {
					SzI index; Str allCmd;
					xFrame->QueryUnsignedAttribute("index", &index);
					allCmd.cpy(xFrame->FirstChildElement("Actionscript")->FirstChildElement("script")->GetText());
					frameList[index]._existCmd = kTrue;
					AnalysisFSCmdStr(allCmd, frameList[index]._cmd);
				}
			}
			return;
		}
		$StructInfo::Struct & Cnv(Path const & xflDir, $StructInfo::Struct & pam, SzI const & imgSz) {
		# if ! defined DISABLE_PAM_FUNC_ON_ANDROID
			gResScaleRate = static_cast<F64>(kStdImgSz) / static_cast<F64>(imgSz);
			SzI imageSum, subAnimSum;
			XFLOtherInfo otherInfo;
			SzI frameRate;
			StrList imageNameList, symbolHrefList;
			Path parentDir; $DevUtil::getCurWorkDirThenForceChgDir(parentDir, xflDir);
			otherInfo.read($JSON::read(Path("OtherInfo.json"), $JSON::Obj().setNull()));
			memCpy(&(pam._unk[0]), &(otherInfo._unk[0]), 13);
			pam._originX = otherInfo._originX;
			pam._originY = otherInfo._originY;
			TiXmlDocument mainDoc;
			ReadXMLFile(mainDoc, Path("DOMDocument.xml"));
		# if ! defined VER_ULTIMATE
			if (mainDoc.RootElement()->Attribute("__ABOUT__") == nullptr || StrRef(mainDoc.RootElement()->Attribute("__ABOUT__")) != "This XFL is convert from PAM file, By TaiJi.") {
				wErr("Document attribute __ABOUT__ does not exist, or its value is wrong. (attr-name is '__ABOUT__' and value is 'This XFL is convert from PAM file, By TaiJi.')");
			}
		# endif
			mainDoc.RootElement()->QueryUnsignedAttribute("frameRate", &frameRate);
			auto const xMedia = mainDoc.RootElement()->FirstChildElement("media");
			for (auto xChild = xMedia->FirstChildElement("DOMBitmapItem"); xChild != nullptr; xChild = xChild->NextSiblingElement()) {
				imageNameList.add(xChild->Attribute("name"));
			}
			auto const xSymbols = mainDoc.RootElement()->FirstChildElement("symbols");
			for (auto xChild = xSymbols->FirstChildElement("Include"); xChild != nullptr; xChild = xChild->NextSiblingElement()) {
				symbolHrefList.add(xChild->Attribute("href"));
			}
			pam._image.allocThenNew(symbolHrefList._sz - kSzOne);
			pam._subAnim.allocThenNew(symbolHrefList._sz - kSzOne);
			imageSum = kSzZero; subAnimSum = kSzZero;
			for_criter(symbolHref, symbolHrefList) {
				TiXmlDocument xDoc;
				ReadXMLFile(xDoc, Path("LIBRARY").add(symbolHref));
				wLogLn("in %s", symbolHref._str);
				goif(symbolHref[kThFirst] == 'M') {
					SzI imageIdx;
					symbolHref.scanFmt(kThThird, "%u", &imageIdx);
					auto & image = pam._image[imageIdx]; Str name;
					LoadImageXTimeLine(*xDoc.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, image);
					Str imageID; imageID = otherInfo._imageMapper.getValByKey(name);
					image._id = imageID;
					//image._src = ;
					image._sz = otherInfo._imageSz.getValByKey(imageID);
					++imageSum;
				}
				elif(symbolHref[kThFirst] == 'A') {
					if (symbolHref == "A_Main.xml") {
						Str name; SzI frameSum;
						LoadAnimXTimeLine(*xDoc.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, pam._mainAnim._frame, frameSum);
						pam._mainAnim._info._rate = frameRate;
						pam._mainAnim._info._sz = frameSum;
						pam._mainAnim._info._begin = kThFirst;
						pam._mainAnim._info._end = frameSum - kThSecond;
					} else {
						SzI subAnimIdx;
						symbolHref.scanFmt(kThThird, "%u", &subAnimIdx);
						auto & subAnim = pam._subAnim[subAnimIdx]; Str name;
						SzI frameSum;
						LoadAnimXTimeLine(*xDoc.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, subAnim._frame, frameSum);
						subAnim._info._rate = frameRate;
						subAnim._info._sz = frameSum;
						subAnim._info._begin = kThFirst;
						subAnim._info._end = frameSum - kThSecond;
						Str subAnimLabel; subAnimLabel = otherInfo._subAnimMapper.getValByKey(name);
						subAnim._label = subAnimLabel;
						++subAnimSum;
					}
				}
				xDoc.Clear();
			}
			pam._image.eraseAfter(imageSum);
			pam._subAnim.eraseAfter(subAnimSum);
			{
				auto xLabelsLayer = mainDoc.RootElement()->FirstChildElement("timelines")->FirstChildElement("DOMTimeline")->FirstChildElement("layers")->FirstChildElement("DOMLayer");
				auto xActionsLayer = xLabelsLayer->NextSiblingElement();
				auto xAudioLayer = xActionsLayer->NextSiblingElement();
				LoadOtherLayer(*xLabelsLayer, *xActionsLayer, *xAudioLayer, pam._mainAnim._frame);
			}
			mainDoc.Clear();
			$DevUtil::chgDirThenFree(parentDir);
		# endif
			return pam;
		}
	}
	namespace $XFLScaleCnv {
		Void cnv(Path const & xflDir, SzI const & srcSz, SzI const & dstSz) {
			Path libDir;
			libDir.cpy(xflDir).add("LIBRARY");
			F64 srcScale, dstScale;
			srcScale = static_cast<F64>(kStdImgSz) / static_cast<F64>(srcSz);
			dstScale = static_cast<F64>(kStdImgSz) / static_cast<F64>(dstSz);
			ListP<Path> fl;
			for_criter(libFile, $DevUtil::getFilePathList(libDir, fl, kTrue)) {
				if (libFile.cmpSuffix(StrRef("xml")) && libFile.getFilePath()[kThFirst] == 'M' && libFile.getFilePath()[kThSecond] == '_') {
					TiXmlDocument xDoc;
					ReadXMLFile(xDoc, libFile);
					auto mat = xDoc.RootElement()
						->FirstChildElement("timeline")->FirstChildElement("DOMTimeline")
						->FirstChildElement("layers")->FirstChildElement("DOMLayer")
						->FirstChildElement("frames")->FirstChildElement("DOMFrame")
						->FirstChildElement("elements")->FirstChildElement("DOMBitmapInstance")
						->FirstChildElement("matrix")->FirstChildElement("Matrix");
					F64 oriA, oriD;
					mat->QueryDoubleAttribute("a", &oriA);
					mat->QueryDoubleAttribute("d", &oriD);
					mat->RemoveAttribute("a");
					mat->RemoveAttribute("d");
					mat->SetDoubleAttribute("a", oriA / srcScale * dstScale);
					mat->SetDoubleAttribute("d", oriD / srcScale * dstScale);
					WriteXMLFile(xDoc.RootElement()->Clone(), libFile);
				}
			}
			return;
		}
	}

	struct _ImageItem {
		SzI _imgIdx;
		ChgMatrix _chgMatrix;
		ChgColor _chgColor;

		auto & operator=(_ImageItem const & t) {
			_imgIdx = t._imgIdx;
			_chgMatrix = t._chgMatrix;
			_chgColor = t._chgColor;
			return *this;
		}
	};
	struct _Layer {
		ListB<SzI> _id;
		ListB<SzI> _parent;
		_ImageItem _img;

		auto & cpy(_Layer const & src) {
			_id = src._id;
			_parent = src._parent;
			_img = src._img;
			return *this;
		}
	};
	struct _RefSlot {
		SzI _idx;
		Bool _isAnim;
		SzI _refIdx;
		SzI _beginFramNum;
		ChgMatrix _chgMatrix;
		ChgColor _chgColor;
	};
	namespace $CnvToRipe {
		static auto GetImageLayerIdx(ListP<_Layer> const & layerList, ListB<SzI> const & id) {
			SzI idx;
			for_cnt_ev(idx, kThFirst, layerList._sz)
				if (layerList[idx]._id == id)
					break;
			return idx;
		}
		static auto GetImageLayerIdx(ListP<_Layer> const & layerList, ListB<SzI> const & id, SzI & idx) {
			for_cnt_ev(idx, kThFirst, layerList._sz)
				if (layerList[idx]._id == id)
					break;
			return idx < layerList._sz;
		}
		static Void GenLayerListList(
			ListP<ListP<_Layer>> & dst,
			$StructInfo::Anim const & srcAnim,
			ListP<$StructInfo::AnimWithLabel> const & animList,
			ListOfInfo<$StructInfo::ImageInfo> const & imageList,
			ListP<ListP<ListP<_Layer>>> const & cacheLayerListListList
		) {
			ListP<_RefSlot> refSlotList;
			SzI curFrameIdx;
			dst.allocThenNew(srcAnim._frame._sz);
			curFrameIdx = kThFirst;
			for_criter(frame, srcAnim._frame) {
				if (frame._existAppend) {
					for_criter(refAppend, frame._append) {
						auto & newRefSlot = refSlotList.add().getLast();
						newRefSlot._beginFramNum = curFrameIdx;
						newRefSlot._idx = refAppend._idx;
						newRefSlot._isAnim = refAppend._isAnim;
						newRefSlot._refIdx = refAppend._refIdx;
						newRefSlot._chgMatrix.init();
						newRefSlot._chgColor.init();
					}
				}
				if (frame._existChg) {
					for_criter(e, frame._chg)
						for_cnt(idx, kThFirst, refSlotList._sz)
						if (refSlotList[idx]._idx == e._idx) {
							refSlotList[idx]._chgMatrix.mix(e);
							refSlotList[idx]._chgColor.load(e);
							break;
						}
				}
				if (frame._existErase) {
					for_criter(refErase, frame._erase)
						for_cnt(idx, kThFirst, refSlotList._sz)
						if (refSlotList[idx]._idx == refErase._idx) {
							refSlotList.erase(idx);
							break;
						}
				}
				for_criter(refSlot, refSlotList) {
					if (refSlot._isAnim) {
						ListP<ListP<_Layer>> * refAnimImgListP(kNullPtr);
						auto isNewInfo = refSlot._refIdx >= cacheLayerListListList._sz;
						if (!isNewInfo)
							refAnimImgListP = const_cast<ListP<ListP<_Layer>> *>(&cacheLayerListListList[refSlot._refIdx]);
						else {
							newPtr(refAnimImgListP);
							GenLayerListList(*refAnimImgListP, animList[refSlot._refIdx], animList, imageList, cacheLayerListListList);
						}
						SzI tgtFrameIdx;
						if (curFrameIdx - refSlot._beginFramNum < (*refAnimImgListP)._sz)
							tgtFrameIdx = curFrameIdx - refSlot._beginFramNum;
						else
							tgtFrameIdx = (*refAnimImgListP).lastIdx();
						for_criter(curRefImgItem, (*refAnimImgListP)[tgtFrameIdx]) {
							auto & layer = dst[curFrameIdx].add(curRefImgItem).getLast();
							layer._id.ins(refSlot._idx);
							layer._parent.add(refSlot._refIdx);
							mixChgMatrix(layer._img._chgMatrix, refSlot._chgMatrix);
							layer._img._chgColor = refSlot._chgColor;
						}
						if (isNewInfo)
							delPtrSingle(refAnimImgListP);
					} else {
						/*ChgMatrix nmat;
						nmat.SetInit();
						nmat._x = imageList[refSlot._refIdx]._posX;
						nmat._y = imageList[refSlot._refIdx]._posY;*/
						auto & newLayer = dst[curFrameIdx].add().getLast();
						newLayer._id.ins(refSlot._refIdx).ins(refSlot._idx);
						newLayer._img._imgIdx = refSlot._refIdx;
						/*newLayer._img._chgMatrix.SetInit();
						newLayer._img._chgMatrix._x = imageList[refSlot._refIdx]._posX;
						newLayer._img._chgMatrix._y = imageList[refSlot._refIdx]._posY;
						newLayer._img._chgMatrix *= refSlot._chgMatrix;*/
						/*newLayer._img._chgMatrix = refSlot._chgMatrix;
						newLayer._img._chgMatrix *= nmat;*/
						newLayer._img._chgMatrix = refSlot._chgMatrix;
						newLayer._img._chgColor = refSlot._chgColor;
					}
				}
				++curFrameIdx;
			}
		}
		static auto & GenSubLabelList(ListOfInfo<$StructInfo::AnimWithLabel> const & src, StrList & dst) {
			for_criter(sub, src)
				dst.add(sub._label);
			return dst;
		}
		static auto & GenLabelRangeList(ListOfInfo<$StructInfo::Frame> const & src, ListOfInfo<$RipePAMStructInfo::LabelRangeInfo> & dst) {
			for_cnt(idx, kThFirst, src._sz) {
				if (src[idx]._existLabel) {
					if (dst._sz > kSzZero)
						dst.getLast()._end = idx - kThSecond;
					dst.add();
					dst.getLast()._label.cpy(src[idx]._label);
					dst.getLast()._begin = idx;
				} elif(idx == kThFirst) {
					dst.add();
					dst.getLast()._label.cpy("");
					dst.getLast()._begin = idx;
				}
			}
			if (dst._sz > kSzZero)
				dst.getLast()._end = src._sz - kThSecond;
			return dst;
		}
		static auto & GenImageListJSON(ListOfInfo<$StructInfo::ImageInfo> const & imageList, ListOfInfo<$RipePAMStructInfo::ImageInfo> & dst) {
			for_criter(image, imageList) {
				auto & obj = dst.add().getLast();
				obj._src = image._src;
				obj._w = image._sz._w * image._scaleX;
				obj._h = image._sz._h * image._scaleY;
				obj._originX = image._posX;
				obj._originY = image._posY;
			}
			return dst;
		}
		static auto CmpLayerID(ListB<SzI> const & id1, ListB<SzI> const & id2) {
			auto result = kFalse;
			for (auto idx = kThFirst; idx < (id1._sz - 1) && idx < (id2._sz - 1); ++idx)
				if (id1[idx] > id2[idx]) {
					result = kTrue;
					break;
				} elif(id1[idx] < id2[idx])
					break;
				return result;
		}
		static auto & GenAnimListJSON(ListP<ListP<_Layer>> const & src, ListOfInfo<$StructInfo::ImageInfo> const & imageList, ListOfInfo<$RipePAMStructInfo::AnimInfo> & dst) {
			ListP<ListB<SzI>> idList;
			for_cnt(layerListIdx, kThFirst, src._sz) {
				for_cnt(layerIdx, kThFirst, src[layerListIdx]._sz) {
					auto const & curID = src[layerListIdx][layerIdx]._id;
					if (!idList.exist(curID)) {
						idList.add(curID);
						ListB<SzI> const * parentPtr(kNullPtr);
						auto & obj = dst.add().getLast();
						obj._id = curID;
						obj._range._begin = layerListIdx;
						obj._range._end = kThNull;
						SzI layerListIdx2;
						for_cnt_ev(layerListIdx2, layerListIdx, src._sz) {
							SzI imageLayerIdx;
							if (GetImageLayerIdx(src[layerListIdx2], curID, imageLayerIdx)) {
								ChgMatrix chgMatrix;
								ChgColor chgColor;
								{
									chgMatrix = src[layerListIdx2][imageLayerIdx]._img._chgMatrix;
									chgColor = src[layerListIdx2][imageLayerIdx]._img._chgColor;
									ChgMatrix lastChg;
									lastChg.init();
									lastChg._x = imageList[curID.getLast()]._posX;
									lastChg._y = imageList[curID.getLast()]._posY;
									mixChgMatrix(chgMatrix, lastChg);
								}
								auto & frame = obj._frame.add().getLast();
								frame._opacity = chgColor._a;
								frame._matrixA = chgMatrix._a;
								frame._matrixB = chgMatrix._b;
								frame._matrixC = chgMatrix._c;
								frame._matrixD = chgMatrix._d;
								frame._matrixX = chgMatrix._x;
								frame._matrixY = chgMatrix._y;
								if (parentPtr == kNullPtr)
									parentPtr = &src[layerListIdx2][imageLayerIdx]._parent;
							} else {
								obj._range._end = layerListIdx2 - kThSecond;
								break;
							}
						}
						if (obj._range._end == kThNull)
							obj._range._end = layerListIdx2;
						obj._parent.cpy(*parentPtr);
					}
				}
			}
			/*
			for (SzI idx = kFirstItemIdx; idx < dst._sz; ++idx)
				for (SzI Idx_2 = (idx + kThSecond); Idx_2 < dst._sz; ++Idx_2) {
					if (CmpLayerID(dst[idx]._id, dst[Idx_2]._id))
						dst.swpItem(idx, Idx_2);
				}*/
			return dst;
		}
		static auto & GenJSON(ListP<ListP<_Layer>> const & layerListList, ListOfInfo<$StructInfo::ImageInfo> const & imageList, StrList const & subLabelList, ListOfInfo<$StructInfo::Frame> const & frameList, SzI const & framePerSecond, SzI const & frameSum, $RipePAMStructInfo::StructInfo & dst) {
			dst._frameRate = framePerSecond;
			dst._frameSum = frameSum;
			dst._subName = subLabelList;
			GenLabelRangeList(frameList, dst._labelRange);
			GenImageListJSON(imageList, dst._imageInfo);
			GenAnimListJSON(layerListList, imageList, dst._animInfo);
			return dst;
		}
		$RipePAMStructInfo::StructInfo & Cnv($StructInfo::Struct const & src, $RipePAMStructInfo::StructInfo & dst) {
			//ListOfInfo<$StructInfo::ImageInfo> imageList;
			{/*
				for_riter(image, imageList)
					if (image._src.existSubStr(StrRef("add$")))
						image._src.eraseSubStr(StrRef("add$"));*/
			}
			ListP<ListP<ListP<_Layer>>> cacheLayerListListList;
			cacheLayerListListList.setNull();
			{
				ListP<ListP<_Layer>> layerListList;
				GenLayerListList(layerListList, src._mainAnim, src._subAnim, src._image, cacheLayerListListList);
				GenJSON(layerListList, src._image, GenSubLabelList(src._subAnim, StrList().setNull()), src._mainAnim._frame, src._mainAnim._info._rate, src._mainAnim._info._sz, dst);
				layerListList.tfree();
				dst._originX = src._originX;
				dst._originY = src._originY;
			}
			return dst;
		}
	}
}
