# pragma once

# include "../Type.h"
# include "./Iter.h"

# include "../Const.h"
# include "../UtilKit/PtrUtil.h"
# include "../UtilKit/MemoryUtil.h"

namespace $TJ {
	struct _ListOfVoidPtr {
		Void ** _item;
		SzI _sz;
		SzI _sz;

		_ListOfVoidPtr() :
			_item(kNullPtr),
			_sz(kSzNull),
			_sz(kSzNull) {
			return;
		}
		~_ListOfVoidPtr() {
			if (_item != kNullPtr)
				wErr("");
			return;
		}

		template < typename T >
		T & operator[](SzI const & Idx) {
			return *static_cast <T *> (_item[Idx]);
		}
		template < typename T >
		T const & operator[](SzI const & Idx) const {
			return *static_cast <T *> (_item[Idx]);
		}

		Bool isNull() const {
			return _item == kNullPtr;
		}
		Bool isFull() const {
			return _sz == _sz;
		}
		SzI getDiffOfFull() const {
			return _sz - _sz;
		}
		SzI lastIdx() const {
			return _sz - kSzOne;
		}

		template < typename T >
		T & get(SzI const & Idx) {
			return *static_cast <T *> (_item[Idx]);
		}
		template < typename T >
		T const & get(SzI const & Idx) const {
			return *static_cast <T const *> (_item[Idx]);
		}
		template < typename T >
		T & last() {
			return get< T>(lastIdx());
		}
		template < typename T >
		T const & last() const {
			return get<T>(lastIdx());
		}

		_ListOfVoidPtr & tfree() {
			if (!isNull()) {
				delPtrArr(_item);
				_sz = kSzNull;
				_sz = kSzNull;
			}
			return *this;
		}
		_ListOfVoidPtr & alloc(SzI const & Size) {
			tfree();
			newPtr(_item, Size);
			for (SzI Idx = kThFirst; Idx < Size; ++Idx)
				_item[Idx] = kNullPtr;
			_sz = kSzZero;
			_sz = Size;
			return *this;
		}
		_ListOfVoidPtr & reAlloc(SzI const & Size) {
			if (Size < _sz) {
				// Error ! should tfree
			} else {
				Void ** TmpItem(kNullPtr);
				movPtr(TmpItem, _item);
				newPtr(_item, Size);
				{
					SzI Idx;
					Idx = kThFirst;
					while (Idx < _sz) {
						movPtr(_item[Idx], TmpItem[Idx]);
						++Idx;
					}
					while (Idx < Size)
						_item[Idx++] = kNullPtr;
				}
				delPtrArr(TmpItem);
				_sz = Size;
			}
			return *this;
		}
		_ListOfVoidPtr & SizeAppend(SzI const & Size = kSzOne) {
			return reAlloc(_sz + Size);
		}
		_ListOfVoidPtr & EraseSurplusSize() {
			return reAlloc(_sz);
		}

		Bool maybeOverflow(SzI const & SizeOfAppend) const {
			return (_sz + SizeOfAppend) > _sz;
		}
		_ListOfVoidPtr & processOverflow(SzI const & SizeOfAppend) {
			if (maybeOverflow(SizeOfAppend))
				SizeAppend((_sz + SizeOfAppend) - _sz);
			return *this;
		}

		_ListOfVoidPtr & swp(SzI const & Idx_1, SzI const & Idx_2) {
			swpPtr(_item[Idx_1], _item[Idx_2]);
			return *this;
		}

		_ListOfVoidPtr & add(Void * Src = kNullPtr) {
			processOverflow(kSzOne);
			_item[_sz++] = Src;
			return *this;
		}

		template < typename T >
		_ListOfVoidPtr & erase(SzI const & Idx) {
			delVoidPtrSingle < T >(_item[Idx]);
			for (SzI Num = Idx; Num < lastIdx(); ++Num)
				_item[Num] = _item[Num + kSzOne];
			_item[lastIdx()] = kNullPtr;
			--_sz;
			return *this;
		}

		template < typename T >
		_ListOfVoidPtr & SetItemNull(SzI const & Idx) {
			delVoidPtrSingle < T >(_item[Idx]);
			return *this;
		}

		protected:
		_ListOfVoidPtr & getBase() {
			return *this;
		}
		_ListOfVoidPtr const & getBase() const {
			return *this;
		}
	};
}
