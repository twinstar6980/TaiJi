#pragma once

#include "../Type.hpp"
#include "../Const.hpp"
#include "../TypeUtil.hpp"
#include "./List.hpp"
#include "./Str.hpp"

namespace $TJ {

	inline auto isLegalPath(Str const & s) {
		if (s.isEmpty())
			return kBoF;
		for (auto i = kNo1; i < s.$sz; ++i)
			if (s[i] == ' ' && (i + kNo2 == s.$sz || $ChType::isPathDelimiter(s[i + kNo2])))
				return kBoF;
		return kBoT;
	}
	inline auto makeLegalPath(Str const & s) {
		Str r(s.$sz);
		if (s.isEmpty()) {
			r = "."_s;
		}
		for (auto i = kNo1; i < s.$sz; ++i) {
			if (s[i] == ' ' && (i + kNo2 == s.$sz || $ChType::isPathDelimiter(s[i + kNo2]))) {
				r.append($Ch::kAlternativeChOfSpaceWhenPathEnd);
			} else {
				r.append(s[i]);
			}
		}
		return r;
	}

#if defined PLATFORM_WINDOWS
	class DriveLetter {

		Ch $letter;

	public:

		auto & set(Ch const & letter = $Ch::kNull) {
			$letter = letter;
			return thiz;
		}

		auto isNull() const {
			return $letter == $Ch::kNull;
		}

		auto toStr() const {
			return Str(kSz2).append($letter).append(':');
		}

		auto operator==(DriveLetter const & t) const {
			return $letter == t.$letter;
		}
		auto operator!=(DriveLetter const & t) const { return !operator==(t); }

		auto & operator=(DriveLetter const & t) {
			$letter = t.$letter;
			return thiz;
		}

		explicit DriveLetter(DriveLetter const &) = default;
		explicit DriveLetter(Ch const & t = $Ch::kNull) : $letter(t) {}

	};
#endif

	struct Path : List<Str> {

	private:

		using TT = Path;
		using TS = List;
		using TE = Str;

	#if defined PLATFORM_WINDOWS
		DriveLetter $drive;
	#endif

	public:

	#if defined PLATFORM_WINDOWS
		auto existDrive() const {
			return !$drive.isNull();
		}
		auto & setDrive(DriveLetter const & t = DriveLetter()) {
			$drive = t;
			return thiz;
		}
	#endif

		auto & appendX(Sz const & sz = kSz1) {
			TS::appendX(sz);
			return thiz;
		}
		auto & append(TE const & t, Sz const & sz = kSz1) {
			TS::append(t, sz);
			return thiz;
		}
		auto & append(TT const & t) {
		#if defined PLATFORM_WINDOWS
			if (t.existDrive()) {
				if (isEmpty())
					setDrive(t.$drive);
				else
					throw VAMGException("Path", "append arg(t) should no drive when this non-empty");
			}
		#endif
			TS::append(t);
			return thiz;
		}
		auto & append(TS const & t) {
			TS::append(t);
			return thiz;
		}

		auto & parse(Str const & s) {
			assert(!s.isNull());
			alloc();
			if (!s.isEmpty()) {
				auto beg = kNo1;
			#if defined PLATFORM_WINDOWS
				if (s.$sz >= kSz3 && s[kNo2] == ':' && $ChType::isPathDelimiter(s[kNo3])) {
					// "[c]:/" is three character
					setDrive(DriveLetter(s[kNo1]));
					beg += kSz2;
				} else {
					setDrive();
				}
			#endif
				auto isRoot = $ChType::isPathDelimiter(s[beg]);
				if (isRoot)
					++beg;
				$Str::split(Str().ref(s, beg), thiz, Range(&*$PathDelimiter::kSet, kSz2));
				if (isRoot)
					TS::prependX().first().alloc().setEmpty();
			}
			return thiz;
		}

		auto & setPrefix(Str const & s) {
			assert(!isNull());
			assert(!s.isNull());
			last().prepend(s);
			return thiz;
		}
		auto & setSuffix(Str const & s) {
			assert(!isNull());
			assert(!s.isNull());
			last().append(s);
			return thiz;
		}
		auto & setExtension(Str const & s) {
			assert(!isNull());
			assert(!s.isNull());
			last().append($Ch::kPeriod).append(s);
			return thiz;
		}

		auto getExtension() const {
			Str r;
			if (Sz i; last().range().findRev('.', i))
				r.cpy(Str().ref(last(), i + kNo2));
			return r;
		}
		auto cmpExtension(Str const & t) const {
			return getExtension() == t;
		}
		auto cmpExtensionIgnoreCase(Str const & t) const {
			return $Str::cmpIgnoreCase(getExtension(), t);
		}
		auto & eraseExtension() {
			if (Sz i; last().range().findRev('.', i))
				last().eraseAfter(i);
			return thiz;
		}

		auto dir() const {
			TT r;
		#if defined PLATFORM_WINDOWS
			r.$drive = $drive;
		#endif
			r.cpy(TS().ref(thiz, kNo1, $sz - kSz1));
			return r;
		}

		auto fileName() const {
			assert(!isNull());
			return last();
		}
		auto fullName(Ch const & delimiter = $PathDelimiter::kOnNative) const {
			assert(!isNull());
			if (isEmpty())
				return ""_s;
			auto isRoot = first().isEmpty();
			Str r;
			TT().ref(thiz, isRoot ? kNo2 : kNo1).concat<TT::ConcatOption::kInside>(r, delimiter);
			if (isRoot)
				r.prepend(delimiter);
		#if defined PLATFORM_WINDOWS
			if (existDrive())
				r.prepend($drive.toStr());
		#endif
			return r;
		}
		auto dirName(Ch const & delimiter = $PathDelimiter::kOnNative) const {
			assert(!isNull());
			if (isEmpty())
				throw VAMGException("Path::dirName()", "empty path");
			return dir().fullName();
		}

	public:

		auto & operator=(TT && t) {
			TS::operator=(t);
		#if defined PLATFORM_WINDOWS
			setDrive(t.$drive);
		#endif
			return thiz;
		}
		auto & operator=(TT const & t) {
			TS::operator=(t);
		#if defined PLATFORM_WINDOWS
			setDrive(t.$drive);
		#endif
			return thiz;
		}
		auto & operator=(TS const & t) {
			TS::operator=(t);
		#if defined PLATFORM_WINDOWS
			setDrive();
		#endif
			return thiz;
		}
		auto & operator=(Str & t) {
			parse(t);
			return thiz;
		}

		auto & operator+=(TT const & t) {
			assert(!isNull());
			assert(!t.isNull());
			return append(t);
		}

		auto operator+(TE const & t) const {
			assert(!isNull());
			TT r($sz + kSz1);
			r.append(thiz).append(t);
			return r;
		}
		auto operator+(TT const & t) const {
			assert(!isNull());
			assert(!t.isNull());
			TT r($sz + t.$sz);
			r.append(thiz).append(t);
			return r;
		}

	public:

		implicit Path() : TS()
		#if defined PLATFORM_WINDOWS
			, $drive()
		#endif
		{
		}
		implicit Path(TT && t) : TS(t)
		#if defined PLATFORM_WINDOWS
			, $drive(t.$drive)
		#endif
		{
		}
		implicit Path(TT const & t) : TS(t)
		#if defined PLATFORM_WINDOWS
			, $drive(t.$drive)
		#endif
		{
		}
		explicit Path(Sz const & cap, Sz const & sz = kSzN) :
			TS(cap, sz)
		#if defined PLATFORM_WINDOWS
			, $drive()
		#endif
		{
		}
		explicit Path(Str const & s) :
			TT() {
			parse(s);
		}

	};

	inline auto operator""_path(char const * const s, std::size_t const len) {
		return Path(Str().ref(s, static_cast<Sz>(len)));
	}

	inline Path const kEmptyPath(""_path);
	inline Path const kRootDir("/"_path);

	enum class FileType : IU8 { kNotExist, kFile, kDir, kOther };

	namespace $Path {

		Bool equal(Path const & a, Path const & b);

		List<Path> & filterByCommon(List<Path> const & srcA, List<Path> const & srcB, List<Path> & dst);
		List<Path> & filterByType(List<Path> const & src, List<Path> & dst, FileType const & ty);
		List<Path> & filterByExtension(List<Path> const & src, List<Path> & dst, Str const & extension);
		List<Path> & filterBySubStr(List<Path> const & src, List<Path> & dst, Str const & str);

		using Selector = List<$Str::Selector>;

		List<Path> & select(Selector const & selector, List<Path> const & ori, List<Path> & result);
		List<Path> & select(Selector const & selector, Path const & parentDir, List<Path> & result);

	}

}
