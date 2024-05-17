
# include "./MyType/OSD.h"
# include "./MyType/Str.h"
# include "./MyType/File.h"
# include "./MyType/JSON.h"

#include "./ResPkgUtil/DZUtil/Struct.h"
#include "./ResPkgUtil/PAKUtil/Struct.h"
#include "./ResPkgUtil/RSGPUtil/Struct.h"
#include "./ResPkgUtil/RSBUtil/Struct.h"
#include "./AudioUtil/WaveStruct.h"
#include "./AudioUtil/SoundBankUtil/Struct.h"
#include "./AnimUtil/PopAnimUtil/Struct.h"

namespace $TJ {

	Str const kNullStr;
	Str const kEmptyStr("");

	Str const kEmptyPathStr("");
#if defined PLATFORM_WINDOWS
	Str const kRootPathStr("\\");
#elif defined PLATFORM_LINUX
	Str const kRootPathStr("/");
#endif

	Path const kEmptyPath("");
#if defined PLATFORM_WINDOWS
	Path const kStorageDir("\\");
#elif defined PLATFORM_LINUX
#if defined PLATFORM_ANDROID
	Path const kStorageDir("/storage/emulated/0");
#else
	Path const kStorageDir("/");
#endif
#endif

	namespace $JSON {
		Val const kDfltErrVal(ValType::kUndef);
		Member const kDfltErrMember(ValType::kUndef);
	}

	namespace $ResPkgUtil::$DZUtil::$Struct {
		namespace $Header {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kDirSum,
				&$MemberOSD::kNameSum,
			};
		}
		namespace $ResPathInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kDirIdx,
				&$MemberOSD::kNameIdx,
				&$MemberOSD::kEnd,
			};
		}
		namespace $ResDataInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kOfs,
				&$MemberOSD::kSize,
				&$MemberOSD::kSize_2,
				&$MemberOSD::kFlag,
			};
		}
		namespace $UnKnownBlock {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kUnKnown_1,
				&$MemberOSD::kUnKnown_e,
			};
		}
		namespace $CompressHeader_512 {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kProps,
				&$MemberOSD::kSize,
				&$MemberOSD::kNullBlock,
			};
		}
		namespace $CompressHeader_8 {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kSize,
				&$MemberOSD::kNullBlock,
			};
		}
	}
	namespace $ResPkgUtil::$PAKUtil::$Struct {
		namespace $Header {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kMagic,
				&$MemberOSD::kVer,
			};
		}
		namespace $ResInfo_Base {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kSize,
				&$MemberOSD::kTime,
			};
		}
	}
	namespace $ResPkgUtil::$RSGPUtil::$Struct {
		namespace $Header {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kHeaderType,
				&$MemberOSD::kHeaderAndResInfoListSize,
				&$MemberOSD::kResDataCompressMethod,
				&$MemberOSD::kResInfoList_Ofs,
				&$MemberOSD::kResInfoList_Size,
				&$MemberOSD::kNormalResData_Ofs,
				&$MemberOSD::kNormalResData_Size,
				&$MemberOSD::kNormalResData_InitSize,
				&$MemberOSD::kAtlasResData_Ofs,
				&$MemberOSD::kAtlasResData_Size,
				&$MemberOSD::kAtlasResData_InitSize,
			};
		}
		namespace $ResDataInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kResType,
				&$MemberOSD::kOfs,
				&$MemberOSD::kSize,
			};
		}
		namespace $ResAtlasInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kIdx,
				&$MemberOSD::kWidth,
				&$MemberOSD::kHeight,
			};
		}
	}
	namespace $ResPkgUtil::$RSBUtil::$Struct {
		namespace $Header {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kHeaderType,
				&$MemberOSD::kSize,
				&$MemberOSD::kGroupIDList_Ofs,
				&$MemberOSD::kGroupIDList_Size,
				&$MemberOSD::kGroupInfoList_Ofs,
				&$MemberOSD::kGroupInfoList_Size,
				&$MemberOSD::kGroupInfoList_Sum,
				&$MemberOSD::kSubGroupIDList_Ofs,
				&$MemberOSD::kSubGroupIDList_Size,
				&$MemberOSD::kSubGroupInfoList_Ofs,
				&$MemberOSD::kSubGroupInfoList_Size,
				&$MemberOSD::kSubGroupInfoList_Sum,
				&$MemberOSD::kResPathList_Ofs,
				&$MemberOSD::kResPathList_Size,
				&$MemberOSD::kAtlasResInfoList_Ofs,
				&$MemberOSD::kAtlasResInfoList_Size,
				&$MemberOSD::kAtlasResInfoList_Sum,
				&$MemberOSD::kAutoPoolInfoList_Ofs,
				&$MemberOSD::kAutoPoolInfoList_Size,
				&$MemberOSD::kAutoPoolInfoList_Sum,
				&$MemberOSD::kResInfoList_Group_Ofs,
				&$MemberOSD::kResInfoList_Res_Ofs,
				&$MemberOSD::kResInfoList_Str_Ofs
			};
		}
		namespace $GroupInfo {
			namespace $SubGroupInfoItem {
				OSD const * const kMemberOSDPtr[kMemberSum]{
					&$MemberOSD::kIdx,
					&$MemberOSD::kResID,
					&$MemberOSD::kLocID,
				};
			}
		}
		namespace $SubGroupInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kRSGPOfs,
				&$MemberOSD::kRSGPSize,
				&$MemberOSD::kIdx,
				&$MemberOSD::kRSGPResDataCompressMethod,
				&$MemberOSD::kRSGPHeaderAndResInfoListSize,
				&$MemberOSD::kRSGPNormalResData_Ofs,
				&$MemberOSD::kRSGPNormalResData_Size,
				&$MemberOSD::kRSGPNormalResData_InitSize,
				&$MemberOSD::kRSGPNormalResData_InitSize_2,
				&$MemberOSD::kRSGPAtlasResData_Ofs,
				&$MemberOSD::kRSGPAtlasResData_Size,
				&$MemberOSD::kRSGPAtlasResData_InitSize,
				&$MemberOSD::kRSGPAtlasResSum,
				&$MemberOSD::kRSGPAtlasResIdxBegin,
			};
		}
		namespace $AutoPoolInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kAtlasResData_Ofs,
				&$MemberOSD::kAtlasResData_Size,
				&$MemberOSD::kUnKnown_1,
			};
		}
		namespace $AtlasResInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kWidth,
				&$MemberOSD::kHeight,
				&$MemberOSD::kTexFmt,
				&$MemberOSD::kUnKnown_w4,
			};
		}
		namespace $AtlasResInfo_Append {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kAlphaSize,
				&$MemberOSD::kUnKnown_100,
			};
		}
		namespace $StrIdxInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kIdx,
			};
		}
		namespace $ResList_Res_AppendMember {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kKey_Ofs,
				&$MemberOSD::kUnKnown,
				&$MemberOSD::kVal_Ofs,
			};
		}
		namespace $ResList_Res_ExpandMember {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kUnKnown_A,
				&$MemberOSD::kFlag,
				&$MemberOSD::kPaddingX,
				&$MemberOSD::kPaddingY,
				&$MemberOSD::kX,
				&$MemberOSD::kY,
				&$MemberOSD::kWidth,
				&$MemberOSD::kHeight,
				&$MemberOSD::kUnKnown_C,
				&$MemberOSD::kUnKnown_D,
				&$MemberOSD::kParent_Ofs,
			};
		}
		namespace $ResList_Res_Base {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kUnKnown,
				&$MemberOSD::kType,
				&$MemberOSD::kAppendBlock_RelaOfs,
				&$MemberOSD::kAppendMember_Ofs,
				&$MemberOSD::kExpandMember_Ofs,
				&$MemberOSD::kID_Ofs,
				&$MemberOSD::kPath_Ofs,
				&$MemberOSD::kAppendMember_Sum,
			};
			Str const kTypeStr[8] = {
				Str("Image"),
				Str(kNullStr),
				Str(kNullStr),
				Str("PrimeFont"),
				Str("PopAnim"),
				Str(kNullStr),
				Str("RenderEffect"),
				Str("File")
			};
		}
		namespace $ResList_ResSlot {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kInfo_Ofs,
			};
		}
		namespace $ResList_SubGroup_Base {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID_Ofs,
				&$MemberOSD::kRes_Sum,
				&$MemberOSD::kResID,
				&$MemberOSD::kLocID,
			};
		}
		namespace $ResList_Group_Base {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID_Ofs,
				&$MemberOSD::kSubGroup_Sum,
				&$MemberOSD::kUnKnown,
			};
		}
	}
	namespace $AudioUtil::$WaveStruct {
		namespace $ChunkSign {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kSize,
			};
		}
		namespace $RIFF_ChunkData {
			namespace $Base {
				OSD const * const kMemberOSDPtr[kMemberSum]{
					&$MemberOSD::kFmt,
				};
			}
			namespace $WaveChunk {
				namespace $Fmt_ChunkData_Base {
					OSD const * const kMemberOSDPtr[kMemberSum]{
						&$MemberOSD::kAudioFmt,
						&$MemberOSD::kNumChannels,
						&$MemberOSD::kSampleRate,
						&$MemberOSD::kByteRate,
						&$MemberOSD::kBlockAlign,
						&$MemberOSD::kBitsPerSample,
					};
				}
				namespace $Fact_ChunkData {
					OSD const * const kMemberOSDPtr[kMemberSum]{
						&$MemberOSD::kSampleTotal,
					};
				}
			}
		}
	}
	namespace $AudioUtil::$SoundBankUtil::$Struct {
		namespace $BlockSign {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kSize,
			};
		}
		namespace $BKHD_Begin {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kVer,
				&$MemberOSD::kID,
			};
		}
		namespace $DIDX_Sub {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kID,
				&$MemberOSD::kOfs,
				&$MemberOSD::kSize,
			};
		}
		namespace $STID_Begin {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kUnKnown_1,
				&$MemberOSD::kItemSum,
			};
		}
		namespace $HIRC_Begin {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kSum,
			};
		}
		namespace $HIRC_Sub_Begin {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kIdent,
				&$MemberOSD::kSize,
				&$MemberOSD::kID,
			};
		}
	}
	namespace $AnimUtil::$PopAnimUtil::$Struct {
		namespace $ImageInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kWidth,
				&$MemberOSD::kHeight,
				&$MemberOSD::kPosX,
				&$MemberOSD::kPosY,
				&$MemberOSD::kScaleX,
				&$MemberOSD::kScaleY,
			};
		}
		namespace $LayerChgType {
			namespace $Pos {
				OSD const * const kMemberOSDPtr[kMemberSum]{
					&$MemberOSD::kX,
					&$MemberOSD::kY,
				};
			}
			namespace $Matrix {
				OSD const * const kMemberOSDPtr[kMemberSum]{
					&$MemberOSD::kA,
					&$MemberOSD::kB,
					&$MemberOSD::kC,
					&$MemberOSD::kD,
				};
			}
			namespace $ColorRate {
				OSD const * const kMemberOSDPtr[kMemberSum]{
					&$MemberOSD::kRed,
					&$MemberOSD::kGreen,
					&$MemberOSD::kBlue,
					&$MemberOSD::kAlpha,
				};
			}
			namespace $Rotate {
				OSD const * const kMemberOSDPtr[kMemberSum]{
					&$MemberOSD::kAngle,
				};
			}
		}
		namespace $AnimFrameInfo {
			OSD const * const kMemberOSDPtr[kMemberSum]{
				&$MemberOSD::kRate,
				&$MemberOSD::kSum,
				&$MemberOSD::kBegin,
				&$MemberOSD::kEnd,
			};
		}
	}
}
