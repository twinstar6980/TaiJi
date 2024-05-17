#include "./XFL.hpp"

#include "../Struct.hpp"
#include "../Info.hpp"
#include "./CSSInfo.hpp"
#include "./Common.hpp"
#include "../../../File/File.hpp"

#include <cmath>
#include <iostream>
#include "../../../../../dependency/tinyxml/tinyxml.h"
#include "../../../../../dependency/tinyxml/tinystr.h"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter::$XFL {

	struct LayerResult {

		Bool $isAnim; Sz $refIdx;
		Sz $frameBegIdx, $frameSum;

	};

	struct LayerState {

		Bool $isAnim; Sz $refIdx;
		Sz $duration;
		ChgMatrix $matrix;
		ChgColor $color;
		Bool $isNewFrameNow, $nextErase;
		TiXmlElement * $ptr = kNullPtr;

	};

	static Str const kXFLContent("PROXY-CS5"_r);

	static constexpr Sz kStdImgSz(1200);
	static F64 gResScaleRate(1.0);

	namespace $XML {

		inline auto setShell(ConstCStr const & value, TiXmlElement * element) {
			auto xShell = new TiXmlElement(value);
			xShell->LinkEndChild(element);
			return xShell;
		}

		inline auto read(TiXmlDocument & doc, Path const & path) {
			doc.LoadFile(path.fullName().$data, TIXML_ENCODING_UTF8);
			return;
		}
		// will clear document after output
		static auto write(TiXmlNode * node, Path const & path) {
			TiXmlDocument doc;
			TiXmlPrinter printer;
			doc.LinkEndChild(node);
			printer.SetIndent("\t");
			doc.Accept(&printer);
			$File::writeFile(path, ByteSection().ref(reinterpret_cast<Byte const *>(printer.CStr()), getCStrLen(printer.CStr())));
			doc.Clear();
			return;
		}

	}

	using $Struct::kUNKDataSz;

	struct XFLExtraInfo {

		ConstSzArray<Byte, kUNKDataSz> $unk;
		XYPair<F64> $origin;
		InfoMap<ImgSz> $imgSz;
		Map<Str, Str> $imgMapper;
		Map<Str, Str> $animMapper;

		auto & read($JSON::Obj const & t) {
			ByteSection unk;
			$Str::readHexStr(t["unk"_r].getStr(), unk);
			if (unk.$sz != kUNKDataSz)
				throw VAMGException("PopcapAnimation-Converter-XFL-ExtraInfo", "unk.$sz != kUNKDataSz");
			Range(&*$unk.$data, kUNKDataSz).cpy(unk.begin());
			$origin.read(t["origin"_r]);
			$imgSz.read(t["imgSz"_r]);
			$JSON::getMapFromArr(t["imgMapper"_r], $imgMapper);
			$JSON::getMapFromArr(t["animMapper"_r], $animMapper);
			return thiz;
		}
		auto & write($JSON::Obj & t) const {
			t.alloc(5);
			$Str::writeHexStr(ByteSection().ref(&*$unk.$data, kUNKDataSz), t("unk"_r).setStr().getStr());
			$origin.write(t("origin"_r));
			$imgSz.write(t("imgSz"_r));
			$JSON::setMapToArr($imgMapper, t("imgMapper"_r));
			$JSON::setMapToArr($animMapper, t("animMapper"_r));
			return t;
		}

		auto & read($JSON::Val const & t) {
			return read(t.getObj());
		}
		auto & write($JSON::Val & t) const {
			return write(t.setObj().getObj());
		}

	};

#if defined ENABLE_XFL_INFO_VALIDATE

	static Str const kXFLInfoValidateTextKey("__ABOUT__"_r);
	static Str const kXFLInfoValidateTextVal("this XFL is convert from Popcap-AniMation file , by TaiJi ."_r);

#endif

	namespace $To {

		inline auto makeXTransformationPointWithShell(XYPair<F64> const & origin) {
			auto t = new TiXmlElement("Point");
			t->SetAttribute("x", $Str::printFmt("%.6f", origin.$x).$data);
			t->SetAttribute("y", $Str::printFmt("%.6f", origin.$y).$data);
			return $XML::setShell("transformationPoint", t);
		}
		inline auto makeXMatrixWithShell(ChgMatrix const & matrix) {
			auto t = new TiXmlElement("Matrix");
			t->SetAttribute("a", $Str::printFmt("%.6f", matrix.$a).$data);
			t->SetAttribute("b", $Str::printFmt("%.6f", matrix.$b).$data);
			t->SetAttribute("c", $Str::printFmt("%.6f", matrix.$c).$data);
			t->SetAttribute("d", $Str::printFmt("%.6f", matrix.$d).$data);
			t->SetAttribute("tx", $Str::printFmt("%.6f", matrix.$x).$data);
			t->SetAttribute("ty", $Str::printFmt("%.6f", matrix.$y).$data);
			return $XML::setShell("matrix", t);
		}
		inline auto makeXColorWithShell(ChgColor const & color) {
			auto t = new TiXmlElement("Color");
			t->SetAttribute("redMultiplier", $Str::printFmt("%.6f", color.$r).$data);
			t->SetAttribute("greenMultiplier", $Str::printFmt("%.6f", color.$g).$data);
			t->SetAttribute("blueMultiplier", $Str::printFmt("%.6f", color.$b).$data);
			t->SetAttribute("alphaMultiplier", $Str::printFmt("%.6f", color.$a).$data);
			return $XML::setShell("color", t);
		}
		inline auto makeXMatrixWithShellForBitmapInstance(XYPair<F64> const & scale, XYPair<F64> const & pos) {
			auto t = new TiXmlElement("Matrix");
			t->SetAttribute("a", $Str::printFmt("%.6f", scale.$x * gResScaleRate).$data);
			t->SetAttribute("d", $Str::printFmt("%.6f", scale.$y * gResScaleRate).$data);
			t->SetAttribute("tx", $Str::printFmt("%.6f", pos.$x).$data);
			t->SetAttribute("ty", $Str::printFmt("%.6f", pos.$y).$data);
			return $XML::setShell("matrix", t);
		}

		inline auto makeXFrameOnlySymbolInstance(Sz const & idx, Sz const & duration, Str const & library, ChgMatrix const & matrix, ChgColor const & color) {
			auto xFrame = new TiXmlElement("DOMFrame");
			auto xSymbolInstance = new TiXmlElement("DOMSymbolInstance");
			xFrame->SetAttribute("index", idx);
			xFrame->SetAttribute("duration", duration);
			xSymbolInstance->SetAttribute("libraryItemName", library.$data);
			xSymbolInstance->LinkEndChild(makeXMatrixWithShell(matrix));
			xSymbolInstance->LinkEndChild(makeXColorWithShell(color));
			xFrame->LinkEndChild($XML::setShell("elements", xSymbolInstance));
			return xFrame;
		}

		inline auto makeXEmptyFrame(Sz const & idx, Sz const & duration) {
			auto xFrame = new TiXmlElement("DOMFrame");
			xFrame->SetAttribute("index", idx);
			xFrame->SetAttribute("duration", duration);
			return xFrame;
		}
		inline auto makeXLabelFrame(Str const & label, Sz const & idx, Sz const & duration) {
			auto xFrame = makeXEmptyFrame(idx, duration);
			xFrame->SetAttribute("name", label.$data);
			return xFrame;
		}
		inline auto makeXCmdFrame(Sz const & idx, Str const & cmd) {
			auto xFrame = new TiXmlElement("DOMFrame");
			auto xScript = new TiXmlElement("script");
			auto xScriptText = new TiXmlText("");
			xFrame->SetAttribute("index", idx);
			xScriptText->SetCDATA(kBoT);
			xScriptText->SetValue(cmd.$data);
			xScript->LinkEndChild(xScriptText);
			xFrame->LinkEndChild($XML::setShell("Actionscript", xScript));
			return xFrame;
		}

		inline auto makeImgXTimeLineWithShell(Str const & name, $Info::Image const & img) {
			auto xTimeLine = new TiXmlElement("DOMTimeline");
			auto xFrame = new TiXmlElement("DOMFrame");
			auto xBitmapInstance = new TiXmlElement("DOMBitmapInstance");
			xTimeLine->SetAttribute("name", name.$data);
			xFrame->SetAttribute("index", static_cast<int>(kNo1));
			xBitmapInstance->SetAttribute("libraryItemName", img.$src.$data);
			xBitmapInstance->LinkEndChild(makeXMatrixWithShellForBitmapInstance(img.$scale, img.$pos));
			xBitmapInstance->LinkEndChild(makeXTransformationPointWithShell(XYPair<F64>(-img.$pos.$x, -img.$pos.$y)));
			xFrame->LinkEndChild($XML::setShell("elements", xBitmapInstance));
			xTimeLine->LinkEndChild(
				$XML::setShell("layers",
							   $XML::setShell("DOMLayer",
											  $XML::setShell("frames", xFrame))));
			return $XML::setShell("timeline", xTimeLine);
		}

		static auto makeAnimXTimeLineWithShell(Str const & name, List<$Info::Frame> const & frameList, List<Str> const & imgID, List<Str> const & animLabel) {
			auto layerSum = frameList.range().eachSum([](auto & v) { return v.$append.$state ? v.$append.$data.$sz : kSzN; }, kSzN);
			List<Bool> layerIsEnable;
			List<LayerState> layerStateList;
			List<LayerResult> layerResultList;
			List<List<TiXmlElement *>> xLayerFrameShellList;
			layerIsEnable.allocThenUse(layerSum);
			layerStateList.allocThenUse(layerSum);
			layerResultList.allocThenUse(layerSum);
			xLayerFrameShellList.allocThenUse(layerSum);
			for (auto frameIdx = kNo1; frameIdx < frameList.$sz; ++frameIdx) {
				auto & frame = frameList[frameIdx];
				if (frame.$erase.$state) {
					for (auto const & e : frame.$erase.$data) {
						layerStateList[e.$idx].$nextErase = kBoT;
					}
				}
				if (frame.$append.$state) {
					for (auto const & e : frame.$append.$data) {
						layerIsEnable[e.$idx] = kBoT;
						auto & t = layerStateList[e.$idx];
						t.$refIdx = e.$refIdx;
						t.$isAnim = e.$isAnim;
						t.$duration = frameIdx;
						t.$matrix.init();
						t.$color.init();
						t.$nextErase = kBoF;
						layerResultList[e.$idx].$isAnim = e.$isAnim;
						layerResultList[e.$idx].$refIdx = e.$refIdx;
						layerResultList[e.$idx].$frameBegIdx = frameIdx;
						xLayerFrameShellList[e.$idx].alloc(frameList.$sz - frameIdx);
					}
				}
				for (auto i = kNo1; i < layerIsEnable.$sz; ++i) {
					if (layerIsEnable[i])
						layerStateList[i].$isNewFrameNow = kBoF;
				}
				if (frame.$chg.$state) {
					for (auto const & e : frame.$chg.$data) {
						auto & t = layerStateList[e.$idx];
						t.$matrix.mix(e);
						t.$color.load(e);
						t.$isNewFrameNow = kBoT;
					}
				}
				for (auto i = kNo1; i < layerIsEnable.$sz; ++i) {
					if (layerIsEnable[i]) {
						auto & t = layerStateList[i];
						if ((t.$isNewFrameNow || t.$nextErase) && t.$ptr != kNullPtr)
							t.$ptr->SetAttribute("duration", t.$duration);
						if (t.$isNewFrameNow) {
							t.$duration = kSz1;
							t.$ptr = makeXFrameOnlySymbolInstance(
								frameIdx, t.$duration,
								(t.$isAnim ? animLabel : imgID)[t.$refIdx],
								t.$matrix, t.$color
							);
							xLayerFrameShellList[i].append(t.$ptr);
						} else {
							++t.$duration;
						}
						if (t.$nextErase) {
							layerIsEnable[i] = kBoF;
							layerResultList[i].$frameSum = frameIdx - layerResultList[i].$frameBegIdx;
						}
					}
				}
			}
			for (auto i = kNo1; i < layerIsEnable.$sz; ++i) {
				if (layerIsEnable[i]) {
					layerResultList[i].$frameSum = frameList.$sz - layerResultList[i].$frameBegIdx;
					auto & state = layerStateList[i];
					if (state.$ptr != kNullPtr)
						state.$ptr->SetAttribute("duration", state.$duration);
				}
			}
			auto xTimeLine = new TiXmlElement("DOMTimeline");
			auto xLayerShell = new TiXmlElement("layers");
			xTimeLine->SetAttribute("name", name.$data);

			/*for_riter(xxx, xLayerFrameShellList) {
				for_riter(bbb, xxx) {
					delete bbb;
				}
			}*/

			for (auto j = xLayerFrameShellList.$sz; j > kNo1; --j) {
				Sz i = j - kNo2;
				auto xLayer = new TiXmlElement("DOMLayer");
				xLayer->SetAttribute("name", $Str::printFmt("l%u", i).$data);
				auto xFrameShell = new TiXmlElement("frames");
				if (layerResultList[i].$frameBegIdx > kNo1)
					xFrameShell->LinkEndChild(makeXEmptyFrame(kNo1, layerResultList[i].$frameBegIdx));
				for (auto & e : xLayerFrameShellList[i])
					xFrameShell->LinkEndChild(e);
				xLayer->LinkEndChild(xFrameShell);
				xLayerShell->LinkEndChild(xLayer);
			}
			xTimeLine->LinkEndChild(xLayerShell);
			return $XML::setShell("timeline", xTimeLine);
		}

		// labels/actions/audio
		static auto makeOtherXLayer(List<$Info::Frame> const & frameList, TiXmlElement *& xLabels, TiXmlElement *& xActions, TiXmlElement *& xAudio) {
			auto xLabelsFrameShell = new TiXmlElement("frames");
			auto xActionsFrameShell = new TiXmlElement("frames");
			auto xAudioFrameShell = new TiXmlElement("frames");
			Str curLabel; Sz curLabelBegIdx;
			if (!frameList.first().$label.$state) {
				curLabel = ""_s;
				curLabelBegIdx = kNo1;
			}
			auto lastActionIdx = kNo1, lastAudioIdx = kNo1;
			for (auto frameIdx = kNo1; frameIdx < frameList.$sz; ++frameIdx) {
				auto & frame = frameList[frameIdx];
				if (frame.$label.$state) {
					if (frameIdx > kNo1)
						xLabelsFrameShell->LinkEndChild(makeXLabelFrame(curLabel, curLabelBegIdx, frameIdx - curLabelBegIdx));
					curLabel = frame.$label.$data;
					curLabelBegIdx = frameIdx;
				}
				if (frame.$stop.$state || frame.$cmd.$state) {
					Str actionCmd(kSzN), audioCmd(kSzN);
					auto isFirstActionCmd = !frame.$stop.$state, isFirstAudioCmd = kBoT;
					if (frame.$stop.$state)
						actionCmd.append("stop();"_r);
					if (frame.$cmd.$state) {
						for (auto const & fsCmd : frame.$cmd.$data) {
							if ($Str::cmpIgnoreCase(fsCmd.$cmd, "PlaySample"_r)) {
								if (!isFirstAudioCmd)
									audioCmd.append('\n');
								else
									isFirstAudioCmd = kBoF;
								audioCmd += $Str::printFmt("fscommand(\"%s\", \"%s\");", fsCmd.$cmd.$data, fsCmd.$arg.$data);
							} else {
								if (!isFirstActionCmd)
									actionCmd.append('\n');
								else
									isFirstActionCmd = kBoF;
								if (fsCmd.$arg.isNull())
									actionCmd += $Str::printFmt("fscommand(\"%s\");", fsCmd.$cmd.$data);
								else
									actionCmd += $Str::printFmt("fscommand(\"%s\", \"%s\");", fsCmd.$cmd.$data, fsCmd.$arg.$data);
							}
						}
					}
					if (!isFirstActionCmd) {
						if (lastActionIdx != frameIdx - lastActionIdx)
							xActionsFrameShell->LinkEndChild(makeXEmptyFrame(lastActionIdx, frameIdx - lastActionIdx));
						xActionsFrameShell->LinkEndChild(makeXCmdFrame(frameIdx, actionCmd));
						lastActionIdx = frameIdx + kNo2;
					}
					if (!isFirstAudioCmd) {
						if (lastAudioIdx != frameIdx - lastAudioIdx)
							xAudioFrameShell->LinkEndChild(makeXEmptyFrame(lastAudioIdx, frameIdx - lastAudioIdx));
						xAudioFrameShell->LinkEndChild(makeXCmdFrame(frameIdx, audioCmd));
						lastAudioIdx = frameIdx + kNo2;
					}
				}
			}
			if (frameList.$sz > kSzN)
				xLabelsFrameShell->LinkEndChild(makeXLabelFrame(curLabel, curLabelBegIdx, frameList.$sz - curLabelBegIdx));
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

		inline auto makeXSymbolItem(Str const & name, TiXmlElement * xTimeLine) {
			auto t = new TiXmlElement("DOMSymbolItem");
			t->SetAttribute("name", name.$data);
			t->LinkEndChild(xTimeLine);
			return t;
		}
		inline auto makeXBitmapItem(Str const & path) {
			auto t = new TiXmlElement("DOMBitmapItem");
			t->SetAttribute("name", path.$data);
			t->SetAttribute("href", (path + ".png"_r).$data);
			return t;
		}
		inline auto makeXInclude(Str const & href) {
			auto t = new TiXmlElement("Include");
			t->SetAttribute("href", href.$data);
			return t;
		}
		inline auto makeFlashDocInfo(TiXmlElement * t) {
			t->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			t->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			return t;
		}

		Void cnv($Info::Info const & info, Path const & xflDir, Sz const & imgSz) {
			gResScaleRate = static_cast<F64>(kStdImgSz) / static_cast<F64>(imgSz);
			XFLExtraInfo extraInfo;
			extraInfo.$unk = info.$unk;
			extraInfo.$origin = info.$origin;
			Path parentDir; $File::getCurDirThenChgDirForce(parentDir, xflDir);
			$File::makeDir("LIBRARY"_path);
			auto xDocument = new TiXmlElement("DOMDocument");
			auto xSymbols = new TiXmlElement("symbols");
			auto xMedia = new TiXmlElement("media");
		#if defined ENABLE_XFL_INFO_VALIDATE
			xDocument->SetAttribute(kXFLInfoValidateTextKey.$data, kXFLInfoValidateTextVal.$data);
		#endif
			extraInfo.$imgSz.alloc(info.$img.$sz);
			extraInfo.$imgMapper.alloc(info.$img.$sz);
			extraInfo.$animMapper.alloc(info.$anim.$sz);
			List<Str> imageIDList(info.$img.$sz), animLabelList(info.$anim.$sz);
			for (auto i = kNo1; i < info.$img.$sz; ++i) {
				auto const & e = info.$img[i];
				auto id = $Str::printFmt("i%u", i);
				extraInfo.$imgSz.appendByKV(e.$id, e.$sz);
				extraInfo.$imgMapper.appendByKV(id, e.$id);
				xSymbols->LinkEndChild(makeXInclude(id + ".xml"_r));
				xMedia->LinkEndChild(makeXBitmapItem(e.$src));
				$XML::write(makeFlashDocInfo(makeXSymbolItem(id, makeImgXTimeLineWithShell(id, e))), "LIBRARY"_path + (id + ".xml"_r));
				imageIDList.append(id);
			}
			for (auto i = kNo1; i < info.$anim.$sz; ++i) {
				auto const & e = info.$anim[i];
				auto id = $Str::printFmt("a%u", i);
				extraInfo.$animMapper.appendByKV(id, e.$label);
				xSymbols->LinkEndChild(makeXInclude(id + ".xml"_r));
				$XML::write(makeFlashDocInfo(makeXSymbolItem(id, makeAnimXTimeLineWithShell(id, e.$frame, imageIDList, animLabelList))), "LIBRARY"_path + (id + ".xml"_r));
				animLabelList.append(id);
			}
			xSymbols->LinkEndChild(makeXInclude("main.xml"_r));
			$XML::write(makeFlashDocInfo(makeXSymbolItem("main"_r, makeAnimXTimeLineWithShell("main"_r, info.$main.$frame, imageIDList, animLabelList))), "LIBRARY"_path + "main.xml"_r);
			xDocument->LinkEndChild(xMedia);
			xDocument->LinkEndChild(xSymbols);
			{
				TiXmlElement * xLabelsLayer, * xActionsLayer, * xAudioLayer;
				makeOtherXLayer(info.$main.$frame, xLabelsLayer, xActionsLayer, xAudioLayer);
				auto xTimeLine = new TiXmlElement("DOMTimeline");
				auto xLayerShell = new TiXmlElement("layers");
				auto xAnimLayer = new TiXmlElement("DOMLayer");
				auto xAnimFrame = new TiXmlElement("DOMFrame");
				auto xAnimSymbolInstance = new TiXmlElement("DOMSymbolInstance");

				xTimeLine->SetAttribute("name", "main");
				xAnimLayer->SetAttribute("name", "animation");
				xAnimFrame->SetAttribute("index", static_cast<int>(kNo1));
				xAnimFrame->SetAttribute("duration", info.$main.$info.$sum);
				xAnimSymbolInstance->SetAttribute("libraryItemName", "main");

				xAnimFrame->LinkEndChild($XML::setShell("elements", xAnimSymbolInstance));
				xAnimLayer->LinkEndChild($XML::setShell("frames", xAnimFrame));
				xLayerShell->LinkEndChild(xLabelsLayer);
				xLayerShell->LinkEndChild(xActionsLayer);
				xLayerShell->LinkEndChild(xAudioLayer);
				xLayerShell->LinkEndChild(xAnimLayer);
				xTimeLine->LinkEndChild(xLayerShell);
				xDocument->LinkEndChild($XML::setShell("timelines", xTimeLine));
			}
			xDocument->SetAttribute("frameRate", info.$main.$info.$rate);
			xDocument->SetAttribute("width", 0);
			xDocument->SetAttribute("height", 0);
			xDocument->SetAttribute("xflVersion", "2.97");
			$XML::write(makeFlashDocInfo(xDocument), "DOMDocument.xml"_path);
			$File::writeFile("main.xfl"_path, ByteSection().ref(reinterpret_cast<Byte const *>(kXFLContent.$data), kXFLContent.$sz));
			$JSON::write("extra.json"_path, extraInfo.write(crv($JSON::Val())));
			$File::chgDirThenFree(parentDir);
			return;
		}

	}

	namespace $From {

		inline auto & loadXMatrixWithShell(TiXmlElement const & xElement, ChgMatrix & matrix) {
			auto & t = *xElement.FirstChildElement("Matrix");
			t.QueryDoubleAttribute("a", &matrix.$a);
			t.QueryDoubleAttribute("b", &matrix.$b);
			t.QueryDoubleAttribute("c", &matrix.$c);
			t.QueryDoubleAttribute("d", &matrix.$d);
			t.QueryDoubleAttribute("tx", &matrix.$x);
			t.QueryDoubleAttribute("ty", &matrix.$y);
			return matrix;
		}
		inline auto & loadXColorWithShell(TiXmlElement const & xElement, ChgColor & color) {
			auto & t = *xElement.FirstChildElement("Color");
			t.QueryDoubleAttribute("redMultiplier", &color.$r);
			t.QueryDoubleAttribute("greenMultiplier", &color.$g);
			t.QueryDoubleAttribute("blueMultiplier", &color.$b);
			t.QueryDoubleAttribute("alphaMultiplier", &color.$a);
			return color;
		}

		inline auto & loadImgXTimeLine(TiXmlElement const & xTimeLine, Str & name, $Info::Image & img) {
			cpyCStr(name, xTimeLine.Attribute("name"));
			auto xDOMBitmapInstance = *xTimeLine
				.FirstChildElement()->FirstChildElement()
				->FirstChildElement()->FirstChildElement()
				->FirstChildElement()->FirstChildElement();
			cpyCStr(img.$src, xDOMBitmapInstance.Attribute("libraryItemName"));
			//auto xTransformationPoint = xDOMBitmapInstance->FirstChildElement("matrix")->FirstChildElement("Matrix");
			//Str(xTransformationPoint->Attribute("x")).scanFmt("%lf", image.$posX);
			//Str(xTransformationPoint->Attribute("y")).scanFmt("%lf", image.$posY);
			auto xMatrix = *xDOMBitmapInstance.FirstChildElement("matrix")->FirstChildElement("Matrix");
			xMatrix.QueryDoubleAttribute("a", &img.$scale.$x);
			xMatrix.QueryDoubleAttribute("d", &img.$scale.$y);
			xMatrix.QueryDoubleAttribute("tx", &img.$pos.$x);
			xMatrix.QueryDoubleAttribute("ty", &img.$pos.$y);
			img.$scale.$x /= gResScaleRate;
			img.$scale.$y /= gResScaleRate;
			return img;
		}

		inline auto loadXFrameOnlySymbolInstance(TiXmlElement const & xFrame, Sz & idx, Sz & duration, Str & library, ChgMatrix & matrix, ChgColor & color) {
			duration = kSz1;
			matrix.init();
			color.init();
			xFrame.QueryUnsignedAttribute("index", &idx);
			xFrame.QueryUnsignedAttribute("duration", &duration);
			auto xDOMSymbolInstance = xFrame.FirstChildElement("elements")->FirstChildElement("DOMSymbolInstance");
			cpyCStr(library, xDOMSymbolInstance->Attribute("libraryItemName"));
			if (xDOMSymbolInstance->FirstChildElement("matrix") != kNullPtr)
				loadXMatrixWithShell(*xDOMSymbolInstance->FirstChildElement("matrix"), matrix);
			if (xDOMSymbolInstance->FirstChildElement("color") != kNullPtr)
				loadXColorWithShell(*xDOMSymbolInstance->FirstChildElement("color"), color);
			return;
		}

		static auto loadAnimXTimeLine(TiXmlElement const & xTimeLine, Str & name, List<$Info::Frame> & frameList, Sz & frameSum) {
			cpyCStr(name, xTimeLine.Attribute("name"));
			auto xLayerShell = xTimeLine.FirstChildElement("layers");
			List<TiXmlElement const *> xLayerList;
			auto layerSum = kSzN;
			frameSum = kSzN;
			for (auto e = xLayerShell->FirstChildElement("DOMLayer"); e != kNullPtr; e = e->NextSiblingElement()) {
				++layerSum;
				auto xFrame = e->FirstChildElement("frames")->LastChild("DOMFrame")->ToElement();
				auto thisFrameIdx = kNo1, thisFrameDuration = kSz1;
				xFrame->QueryUnsignedAttribute("index", &thisFrameIdx);
				xFrame->QueryUnsignedAttribute("duration", &thisFrameDuration);
				if (frameSum < thisFrameIdx + thisFrameDuration)
					frameSum = thisFrameIdx + thisFrameDuration;
			}
			xLayerList.alloc(layerSum);
			for (auto e = xLayerShell->FirstChildElement("DOMLayer"); e != kNullPtr; e = e->NextSiblingElement()) {
				xLayerList.append(e);
			}
			frameList.allocThenUse(frameSum);
			for (auto layerIdx = kNo1; layerIdx < layerSum; ++layerIdx) {
				auto & xLayer = xLayerList[xLayerList.lastIdx() - layerIdx];
				log.txt("\r%.9s", xLayer->Attribute("name"));
				Sz frameIdxOfBeErase;
				auto xFrameShell = xLayer->FirstChildElement("frames");
				auto xFirstRealFrame = xFrameShell->FirstChildElement("DOMFrame");
				while (xFirstRealFrame != kNullPtr) {
					if (!(xFirstRealFrame->NoChildren() || xFirstRealFrame->FirstChildElement("elements")->NoChildren()))
						break;
					xFirstRealFrame = xFirstRealFrame->NextSiblingElement();
				}
				if (xFirstRealFrame == kNullPtr) {
					log.war("this layer can't found valid-frame.");
				} else {
					auto firstFrameIdx = kNo1;
					{
						Sz frameIdx; Str library;
						loadXFrameOnlySymbolInstance(*xFirstRealFrame, frameIdx, crv(Sz()), library, crv(ChgMatrix()), crv(ChgColor()));
						auto & frame = frameList[frameIdx];
						if (!frame.$append.$state) {
							frame.$append.$state = kBoT;
							frame.$append.$data.alloc();
						}
						auto & layerAppend = frame.$append.$data.appendX().last();
						layerAppend.$idx = layerIdx;
						if (library.first() == 'a')
							layerAppend.$isAnim = kBoT;
						else if (library.first() == 'i')
							layerAppend.$isAnim = kBoF;
						else
							throw VAMGException("PopcapAnimation-Converter-XFL-From", "unknown XFL-Library type : %c", library.first());
						$Str::scanFmt(Str().ref(library, kNo2), "%u", &layerAppend.$refIdx);
						firstFrameIdx = frameIdx;
					}
					for (auto xFrame = xFirstRealFrame; xFrame != kNullPtr; xFrame = xFrame->NextSiblingElement()) {
						Sz frameIdx, frameDuration; ChgMatrix matrix; ChgColor color;
						loadXFrameOnlySymbolInstance(*xFrame, frameIdx, frameDuration, crv(Str()), matrix, color);
						frameIdxOfBeErase = frameIdx + frameDuration;
						auto & frame = frameList[frameIdx];
						if (!frame.$chg.$state) {
							frame.$chg.$state = kBoT;
							frame.$chg.$data.alloc();
						}
						auto & layerChg = frame.$chg.$data.appendX().last();
						layerChg.$idx = layerIdx;
						layerChg.$matrix.$state = kBoT;
						layerChg.$pos.$state = kBoT;
						layerChg.$color.$state = kBoT;
						layerChg.$matrix.$data.$a = matrix.$a;
						layerChg.$matrix.$data.$b = matrix.$b;
						layerChg.$matrix.$data.$c = matrix.$c;
						layerChg.$matrix.$data.$d = matrix.$d;
						layerChg.$pos.$data.$pos.$x = matrix.$x;
						layerChg.$pos.$data.$pos.$y = matrix.$y;
						layerChg.$color.$data.$r = color.$r;
						layerChg.$color.$data.$g = color.$g;
						layerChg.$color.$data.$b = color.$b;
						layerChg.$color.$data.$a = color.$a;
					}
					if (frameIdxOfBeErase < frameSum) {
						auto & frame = frameList[frameIdxOfBeErase];
						if (!frame.$erase.$state) {
							frame.$erase.$state = kBoT;
							frame.$erase.$data.alloc();
						}
						auto & layerErase = frame.$erase.$data.appendX().last();
						layerErase.$idx = layerIdx;
						layerErase.$unk = 0;
					}
				}
			}
			log.txtln("");
			return;
		}

		static auto & analysisFSCmdStr(Str const & src, List<$Info::FSCmd> & dst) {
			List<Str> cmdStrList;
			$Str::split(src, cmdStrList, ';');
			dst.alloc(cmdStrList.$sz);
			for (auto const & cmdStr : cmdStrList) {
				if (cmdStr.range().existSub("fscommand"_r.range())) {
					auto & cmd = dst.appendX().last();
					auto pos = cmdStr.range().find('(');
					++pos;
					while (cmdStr[pos++] != '\"')
						;
					for (auto pos2 = pos; pos2 < cmdStr.$sz; ++pos2) {
						if (cmdStr[pos2] == '\"') {
							cmd.$cmd.cpy(cmdStr.$data + pos, pos2 - pos);
							break;
						}
					}
					if (!cmdStr.range().find(',', pos)) {
						cmd.$arg.setNull();
					} else {
						++pos;
						while (cmdStr[pos++] != '\"')
							;
						for (auto pos2 = pos; pos2 < cmdStr.$sz; ++pos2) {
							if (cmdStr[pos2] == '\"') {
								cmd.$arg.cpy(cmdStr.$data + pos, pos2 - pos);
								break;
							}
						}
					}
				}
			}
			return dst;
		}

		static auto loadOtherLayer(TiXmlElement const & xLabels, TiXmlElement const & xActions, TiXmlElement const & xAudio, List<$Info::Frame> & frameList) {
			for (auto e = xLabels.FirstChildElement("frames")->FirstChildElement("DOMFrame"); e != kNullPtr; e = e->NextSiblingElement()) {
				Sz index;
				e->QueryUnsignedAttribute("index", &index);
				frameList[index].$label.$state = kBoT;
				cpyCStr(frameList[index].$label.$data, e->Attribute("name"));
			}
			for (auto e = xActions.FirstChildElement("frames")->FirstChildElement("DOMFrame"); e != kNullPtr; e = e->NextSiblingElement()) {
				if (!e->NoChildren() && refCStr(crv(Str()), e->FirstChildElement()->Value()) == "Actionscript"_r) {
					Sz index; Str allCmd;
					e->QueryUnsignedAttribute("index", &index);
					cpyCStr(allCmd, e->FirstChildElement("Actionscript")->FirstChildElement("script")->GetText());
					if (allCmd.range().existSub("stop();"_r.range())) {
						frameList[index].$stop.$state = kBoT;
						frameList[index].$stop.$data = kBoT;
					}
					if (allCmd.range().existSub("fscommand"_r.range())) {
						frameList[index].$cmd.$state = kBoT;
						analysisFSCmdStr(allCmd, frameList[index].$cmd.$data);
					}
				}
			}
			for (auto e = xAudio.FirstChildElement("frames")->FirstChildElement("DOMFrame"); e != kNullPtr; e = e->NextSiblingElement()) {
				if (!e->NoChildren() && refCStr(crv(Str()), e->FirstChildElement()->Value()) == "Actionscript"_r) {
					Sz index; Str allCmd;
					e->QueryUnsignedAttribute("index", &index);
					cpyCStr(allCmd, e->FirstChildElement("Actionscript")->FirstChildElement("script")->GetText());
					frameList[index].$cmd.$state = kBoT;
					analysisFSCmdStr(allCmd, frameList[index].$cmd.$data);
				}
			}
			return;
		}

		$Info::Info & cnv($Info::Info & info, Path const & xflDir, Sz const & imgSz) {
			Path parentDir; $File::getCurDirThenChgDir(parentDir, xflDir);
			gResScaleRate = static_cast<F64>(kStdImgSz) / static_cast<F64>(imgSz);
			XFLExtraInfo extraInfo;
			extraInfo.read($JSON::read("extra.json"_path, crv($JSON::Val())));
			info.$unk = extraInfo.$unk;
			info.$origin = extraInfo.$origin;
			TiXmlDocument mainDoc;
			$XML::read(mainDoc, "DOMDocument.xml"_path);
		#if defined ENABLE_XFL_INFO_VALIDATE
			if (mainDoc.RootElement()->Attribute(kXFLInfoValidateTextKey.$data) == kNullPtr || refCStr(crv(Str()), mainDoc.RootElement()->Attribute(kXFLInfoValidateTextKey.$data)) != kXFLInfoValidateTextVal)
				throw VAMGException("PopcapAnimation-Converter-XFL-From", "document attribute %s does not exist, or its value is wrong. (attr-name is '%s' and value is '%s')", kXFLInfoValidateTextKey.$data, kXFLInfoValidateTextKey.$data, kXFLInfoValidateTextVal.$data);
		#endif
			Sz frameRate;
			mainDoc.RootElement()->QueryUnsignedAttribute("frameRate", &frameRate);
			List<Str> imageNameList(kSzN), symbolHrefList(kSzN);
			for (auto e = mainDoc.RootElement()->FirstChildElement("media")->FirstChildElement("DOMBitmapItem"); e != kNullPtr; e = e->NextSiblingElement()) {
				imageNameList.append(refCStr(crv(Str()), e->Attribute("name")));
			}
			for (auto e = mainDoc.RootElement()->FirstChildElement("symbols")->FirstChildElement("Include"); e != kNullPtr; e = e->NextSiblingElement()) {
				symbolHrefList.append(refCStr(crv(Str()), e->Attribute("href")));
			}
			info.$img.allocThenUse(symbolHrefList.$sz - kSz1);
			info.$anim.allocThenUse(symbolHrefList.$sz - kSz1);
			auto imgSum = kSzN, animSum = kSzN;
			for (auto const & symbolHref : symbolHrefList) {
				TiXmlDocument xDoc;
				$XML::read(xDoc, "LIBRARY"_path + symbolHref);
				log.txtln("in %s", symbolHref.$data);
				if (symbolHref == "main.xml"_r) {
					Str name; Sz frameSum;
					loadAnimXTimeLine(*xDoc.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, info.$main.$frame, frameSum);
					info.$main.$info.$rate = frameRate;
					info.$main.$info.$sum = frameSum;
					info.$main.$info.$beg = kNo1;
					info.$main.$info.$end = frameSum - kNo2;
				} else if (symbolHref.first() == 'i') {
					Sz i;
					$Str::scanFmt(Str().ref(symbolHref, kNo2), "%u", &i);
					auto & t = info.$img[i];
					Str name;
					loadImgXTimeLine(*xDoc.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, t);
					auto const & id = extraInfo.$imgMapper[name];
					t.$id = id;
					t.$sz = extraInfo.$imgSz[id];
					++imgSum;
				} else if (symbolHref.first() == 'a') {
					Sz i;
					$Str::scanFmt(Str().ref(symbolHref, kNo2), "%u", &i);
					auto & t = info.$anim[i];
					Str name; Sz frameSum;
					loadAnimXTimeLine(*xDoc.RootElement()->FirstChildElement("timeline")->FirstChildElement("DOMTimeline"), name, t.$frame, frameSum);
					t.$info.$rate = frameRate;
					t.$info.$sum = frameSum;
					t.$info.$beg = kNo1;
					t.$info.$end = frameSum - kNo2;
					t.$label = extraInfo.$animMapper[name];
					++animSum;
				} else {
					throw VAMGException("PopcapAnimation-Converter-XFL-Form", "unknown XFL-Library type : %c", symbolHref.first());
				}
				xDoc.Clear();
			}
			info.$img.eraseAfter(imgSum);
			info.$anim.eraseAfter(animSum);
			{
				auto xLabelsLayer = mainDoc.RootElement()->FirstChildElement("timelines")->FirstChildElement("DOMTimeline")->FirstChildElement("layers")->FirstChildElement("DOMLayer");
				auto xActionsLayer = xLabelsLayer->NextSiblingElement();
				auto xAudioLayer = xActionsLayer->NextSiblingElement();
				loadOtherLayer(*xLabelsLayer, *xActionsLayer, *xAudioLayer, info.$main.$frame);
			}
			mainDoc.Clear();
			$File::chgDirThenFree(parentDir);
			info.$main.$block1.$v1.alloc().setEmpty();
			info.$main.$block1.$v2.alloc().setEmpty();
			return info;
		}

	}

	namespace $ScaleCnv {

		Void cnv(Path const & xflDir, Sz const & srcSz, Sz const & dstSz) {
			auto srcScale = static_cast<F64>(kStdImgSz) / static_cast<F64>(srcSz);
			auto dstScale = static_cast<F64>(kStdImgSz) / static_cast<F64>(dstSz);
			List<Path> fileList;
			for (auto const & libFile : $File::getPathListOfFile(xflDir + "LIBRARY"_r, fileList, kBoT)) {
				if (libFile.cmpExtension("xml"_r) && libFile.fileName().first() == 'i') {
					TiXmlDocument xDoc;
					$XML::read(xDoc, libFile);
					auto mat = xDoc.RootElement()
						->FirstChildElement("timeline")->FirstChildElement("DOMTimeline")
						->FirstChildElement("layers")->FirstChildElement("DOMLayer")
						->FirstChildElement("frames")->FirstChildElement("DOMFrame")
						->FirstChildElement("elements")->FirstChildElement("DOMBitmapInstance")
						->FirstChildElement("matrix")->FirstChildElement("Matrix");
					F64 oriA, oriD;
					mat->QueryDoubleAttribute("a", &oriA);
					mat->QueryDoubleAttribute("d", &oriD);
					mat->SetDoubleAttribute("a", oriA / srcScale * dstScale);
					mat->SetDoubleAttribute("d", oriD / srcScale * dstScale);
					$XML::write(xDoc.RootElement()->Clone(), libFile);
				}
			}
			return;
		}

	}

}
