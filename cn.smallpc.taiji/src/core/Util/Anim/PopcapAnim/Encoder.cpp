#include "./Encoder.hpp"

#include "../../../Type/JSON.hpp"
#include "../../File/File.hpp"

namespace $TJ::$Util::$Anim::$PopcapAnim::$Encoder {

	namespace $EnCode {

		extern Void enc(ByteSection & data, $Info::Info const & info) {
			ByteStream stm;
			stm.ref(data.$data, data.$cap).setPosBeg();
			$Struct::Struct stu;
			info.parseTo(stu);
			stu.write(stm);
			data.setSz(stm.$pos);
			return;
		}

		extern Void encExport(Path const & dataFile, Path const & infoFile, Sz const & memSz) {
			ByteSection data;
			$Info::Info info;
			data.alloc(memSz);
			info.read($JSON::read(infoFile, crv($JSON::Val())));
			enc(data, info);
			$File::writeFile(dataFile, data);
			data.free();
			return;
		}

	}

	namespace $DeCode {

		extern $Info::Info & dec(ByteSection const & data, $Info::Info & info) {
			return info.parse($Struct::Struct().read(ByteStream().ref(data).setPosBeg()));
		}

		extern Void decExport(Path const & dataFile, Path const & infoFile) {
			ByteSection data;
			$Info::Info info;
			$File::readFileForce(dataFile, data);
			dec(data, info);
			if (!infoFile.isNull())
				$JSON::write(infoFile, info.write(crv($JSON::Val())));
			data.free();
			return;
		}

	}

}
