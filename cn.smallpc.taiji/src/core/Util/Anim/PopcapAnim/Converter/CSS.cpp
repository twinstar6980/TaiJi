#include "./CSS.hpp"

#include "../Struct.hpp"
#include "../Info.hpp"
#include "./CSSInfo.hpp"
#include "./Common.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Converter::$CSS {

	using $CSSInfo::LayerID;

	namespace $To {

		struct Layer {

			LayerID $id, $parent;

			struct {

				Sz $idx;
				ChgMatrix $matrix;
				ChgColor $color;

			} $img;

		};

		struct RefSlot {

			Sz $idx;
			Bool $isAnim;
			Sz $refIdx;
			Sz $begFrameNum;
			ChgMatrix $chgMatrix;
			ChgColor $chgColor;

		};

		static Void makeLayerListList(
			List<List<Layer>> & dst,
			$Info::Anim const & srcAnim,
			List<$Info::Image> const & imgList,
			List<List<List<Layer>>> const & animList
		) {
			List<RefSlot> refSlotList;
			refSlotList.alloc(srcAnim.$frame.range().eachSum([](auto & v) { return v.$append.$state ? v.$append.$data.$sz : kSzN; }, kSzN));
			dst.allocThenUse(srcAnim.$frame.$sz);
			dst.range().each([&refSlotList](auto & v) { v.alloc(refSlotList.$cap); });
			auto curFrameIdx = kNo1;
			for (auto const & frame : srcAnim.$frame) {
				if (frame.$append.$state) {
					for (auto const & e : frame.$append.$data) {
						auto & newRefSlot = refSlotList.appendX().last();
						newRefSlot.$begFrameNum = curFrameIdx;
						newRefSlot.$idx = e.$idx;
						newRefSlot.$isAnim = e.$isAnim;
						newRefSlot.$refIdx = e.$refIdx;
						newRefSlot.$chgMatrix.init();
						newRefSlot.$chgColor.init();
					}
				}
				if (frame.$chg.$state) {
					for (auto const & e : frame.$chg.$data) {
						for (auto i = kNo1; i < refSlotList.$sz; ++i) {
							if (refSlotList[i].$idx == e.$idx) {
								refSlotList[i].$chgMatrix.mix(e);
								refSlotList[i].$chgColor.load(e);
								break;
							}
						}
					}
				}
				if (frame.$erase.$state) {
					for (auto const & e : frame.$erase.$data) {
						for (auto i = kNo1; i < refSlotList.$sz; ++i) {
							if (refSlotList[i].$idx == e.$idx) {
								refSlotList.eraseOne(i);
								break;
							}
						}
					}
				}
				for (auto const & refSlot : refSlotList) {
					if (refSlot.$isAnim) {
						if (refSlot.$refIdx > animList.$sz)
							throw VAMGException("PopcapAnimation-Converter-CSS", "reference index too big : %u - %u", refSlot.$refIdx, animList.$sz);
						auto & refAnimImgList = animList[refSlot.$refIdx];
						Sz tgtFrameIdx;
						if (curFrameIdx - refSlot.$begFrameNum < refAnimImgList.$sz)
							tgtFrameIdx = curFrameIdx - refSlot.$begFrameNum;
						else
							tgtFrameIdx = refAnimImgList.lastIdx();
						for (auto const & curRefImgItem : refAnimImgList[tgtFrameIdx]) {
							auto & layer = dst[curFrameIdx].append(curRefImgItem).last();
							layer.$id.$data.prepend(refSlot.$idx);
							layer.$parent.$data.append(refSlot.$refIdx);
							mixChgMatrix(layer.$img.$matrix, refSlot.$chgMatrix);
							layer.$img.$color = refSlot.$chgColor;
						}
					} else {
						auto & newLayer = dst[curFrameIdx].appendX().last();
						newLayer.$id.$data.alloc(2).prepend(refSlot.$refIdx).prepend(refSlot.$idx);
						newLayer.$parent.$data.alloc();
						newLayer.$img.$idx = refSlot.$refIdx;
						newLayer.$img.$matrix = refSlot.$chgMatrix;
						newLayer.$img.$color = refSlot.$chgColor;
					}
				}
				++curFrameIdx;
			}
			return;
		}

		static auto & makeSubLabelList(List<$Info::AnimWithLabel> const & src, List<Str> & dst) {
			dst.alloc(src.$sz);
			for (auto const & e : src)
				dst.append(e.$label);
			return dst;
		}

		static auto & makeLabelRangeMap(List<$Info::Frame> const & src, InfoMap<$CSSInfo::RangeInfo> & dst) {
			dst.alloc(src.range().eachSum([](auto & v) { return v.$label.$state ? kSz1 : kSzN; }, kSzN) + kSz1);
			for (auto i = kNo1; i < src.$sz; ++i) {
				if (src[i].$label.$state) {
					if (!dst.isEmpty())
						dst.last().$v.$end = i - kNo2;
					dst.appendX();
					dst.last().$k = src[i].$label.$data;
					dst.last().$v.$beg = i;
				} else if (i == kNo1) {
					dst.appendX();
					dst.last().$k = ""_s;
					dst.last().$v.$beg = i;
				}
			}
			if (!dst.isEmpty())
				dst.last().$v.$end = src.lastIdx();
			return dst;
		}

		static auto & makeImageList(List<$Info::Image> const & src, List<$CSSInfo::Image> & dst) {
			dst.alloc(src.$sz);
			for (auto const & e : src) {
				auto & t = dst.appendX().last();
				t.$src = e.$src;
				t.$szW = e.$sz.$w * e.$scale.$x;
				t.$szH = e.$sz.$h * e.$scale.$y;
				t.$origin.$x = e.$pos.$x;
				t.$origin.$y = e.$pos.$y;
			}
			return dst;
		}

		static auto & makeAnimList(List<List<Layer>> const & src, List<$Info::Image> const & img, List<$CSSInfo::Layer> & dst) {
			List<LayerID> idList;
			idList.alloc(src.range().eachSum([](auto & v) { return v.$sz; }, kSzN));
			dst.alloc(idList.$cap);
			for (auto layerListIdx = kNo1; layerListIdx < src.$sz; ++layerListIdx) {
				for (auto layerIdx = kNo1; layerIdx < src[layerListIdx].$sz; ++layerIdx) {
					if (auto & id = src[layerListIdx][layerIdx].$id; !idList.range().exist(id)) {
						idList.append(id);
						LayerID const * parent(kNullPtr);
						auto & t = dst.appendX().last();
						t.$id = id;
						t.$range.$beg = layerListIdx;
						t.$range.$end = kNoX;
						t.$frame.alloc(src.$sz - layerListIdx);
						Sz layerListIdx2;
						for (layerListIdx2 = layerListIdx; layerListIdx2 < src.$sz; ++layerListIdx2) {
							if (Sz imgLayerIdx; src[layerListIdx2].range().findBy([&id](auto & v) { return v.$id == id; }, imgLayerIdx)) {
								auto & tt = t.$frame.appendX().last();
								tt.$matrix = src[layerListIdx2][imgLayerIdx].$img.$matrix;
								tt.$opacity = src[layerListIdx2][imgLayerIdx].$img.$color.$a;
								ChgMatrix lastChg;
								lastChg.init();
								lastChg.$x = img[id.$data.last()].$pos.$x;
								lastChg.$y = img[id.$data.last()].$pos.$y;
								mixChgMatrix(tt.$matrix, lastChg);
								if (parent == kNullPtr)
									parent = &src[layerListIdx2][imgLayerIdx].$parent;
							} else {
								t.$range.$end = layerListIdx2 - kNo2;
								break;
							}
						}
						if (t.$range.$end == kNoX)
							t.$range.$end = layerListIdx2;
						t.$parent = *parent;
					}
				}
			}
			/*for (Sz idx = kFirstItemIdx; idx < dst.$sz; ++idx)
				for (Sz Idx_2 = (idx + kThSecond); Idx_2 < dst.$sz; ++Idx_2) {
					if (CmpLayerID(dst[idx].$id, dst[Idx_2].$id))
						dst.swpItem(idx, Idx_2);
				}*/
			return dst;
		}

		$CSSInfo::Info & cnv($Info::Info const & info, $CSSInfo::Info & cssInfo) {
			/*List<$Info::ImageInfo> imageList;
			{
				for (auto & e : imageList)
					if (e.$src.range().existSub("add$"_r.range()))
						e.$src.eraseSub("add$"_r);
			}*/
			{
				cssInfo.$origin = info.$origin;
				cssInfo.$frameRate = info.$main.$info.$rate;
				cssInfo.$frameSum = info.$main.$info.$sum;
				makeSubLabelList(info.$anim, cssInfo.$sub);
				makeLabelRangeMap(info.$main.$frame, cssInfo.$label);
				makeImageList(info.$img, cssInfo.$img);
				{
					List<List<Layer>> layerListList;
					List<List<List<Layer>>> cacheLayer(info.$anim.$sz);
					for (auto const & e : info.$anim) {
						auto & t = *cacheLayer.end();
						makeLayerListList(t, e, info.$img, cacheLayer);
						cacheLayer.expandSz(kSz1);
					}
					makeLayerListList(layerListList, info.$main, info.$img, cacheLayer);
					makeAnimList(layerListList, info.$img, cssInfo.$layer);
				}
			}
			return cssInfo;
		}

	}

}
