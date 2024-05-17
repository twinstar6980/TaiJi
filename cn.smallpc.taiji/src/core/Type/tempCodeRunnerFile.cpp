auto pos = kNo1;
			auto & vInt = *reinterpret_cast<IU64 *>(dst);
			auto isNeg = kBoF;
			{
				auto ch = src[pos++];
				if (ch == '+') {
					type = NumType::kSInt;
					vInt = 0u;
				} else if (ch == '-') {
					type = NumType::kSInt;
					vInt = 0u;
					isNeg = kBoT;
				} else {
					type = NumType::kUInt;
					vInt = ch - '0';
				}
			}
			while (kBoT) {
				auto ch = src[pos++];
				if ($ChType::isDecNum(ch)) {
					vInt *= 10u;
					vInt += ch - '0';
				} else {
					if (ch == '.') {
						type = NumType::kFloat;
						auto & vFloat = *reinterpret_cast<F64 *>(dst);
						vFloat = static_cast<F64>(vInt);
						F64 vBase = 0.1;
						while (kBoT) {
							ch = src[pos++];
							if ($ChType::isDecNum(ch)) {
								vFloat += (ch - '0') * vBase;
								vBase /= 10.0;
							} else
								break;
						}
					}
					--pos;
					break;
				}
			}
			if (isNeg) {
				if (type == NumType::kSInt)
					(*reinterpret_cast<IS64 *>(dst)) = -*reinterpret_cast<IS64 *>(dst);
				else if (type == NumType::kFloat)
					(*reinterpret_cast<F64 *>(dst)) = -*reinterpret_cast<F64 *>(dst);
			}
			return pos;