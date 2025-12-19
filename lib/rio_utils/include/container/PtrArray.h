#pragma once

#include <misc/rio_Types.h>

class PtrArrayImpl
{
protected:
    typedef s32 (*CompareCallbackImpl)(const void*, const void*);

public:
    PtrArrayImpl()
        : mPtrNum(0)
        , mPtrNumMax(0)
        , mPtrs(nullptr)
    {
    }

    void setBuffer(s32 ptrNumMax, void* buf);
    void allocBuffer(s32 ptrNumMax, s32 alignment = 4);
    bool tryAllocBuffer(s32 ptrNumMax, s32 alignment = 4);
    void freeBuffer();
    bool isBufferReady() const { return mPtrs != nullptr; }
    bool isEmpty() const { return mPtrNum == 0; }
    bool isFull() const { return mPtrNum >= mPtrNumMax; }
    s32 size() const { return mPtrNum; }
    s32 maxSize() const { return mPtrNumMax; }
    void erase(s32 pos) { return erase(pos, 1); }
    void erase(s32 pos, s32 num);
    void clear() { mPtrNum = 0; }
    void resize(s32);
    void unsafeResize(s32);
    void swap(s32 pos1, s32 pos2);
    void reverse();
    PtrArrayImpl& operator=(const PtrArrayImpl&);
    void shuffle();

protected:
    void* at(s32 n) const
    {
        if (u32(mPtrNum) <= u32(n))
        {
            RIO_LOG("index exceeded [%d/%d]\n", n, mPtrNum);
            RIO_ASSERT(false);
            return nullptr;
        }
        return mPtrs[n];
    }

    void* unsafeAt(s32 n) const { return mPtrs[n]; }
    void* front() const { return mPtrs[0]; }
    void* back() const { return mPtrs[mPtrNum - 1]; }

    void pushBack(void* ptr)
    {
        if (isFull())
        {
            RIO_LOG("list is full.\n");
            RIO_ASSERT(false);
            return;
        }

        mPtrs[mPtrNum] = ptr;
        ++mPtrNum;
    }

    void pushFront(void* ptr) { insert(0, ptr); }
    void* popBack();
    void* popFront();
    void replace(s32 pos, void* ptr);
    void* find(const void* ptr, CompareCallbackImpl cmp) const;
    s32 search(const void* ptr, CompareCallbackImpl cmp) const;
    bool equal(const PtrArrayImpl& o, CompareCallbackImpl cmp) const;

    s32 indexOf(const void* ptr) const
    {
        for (s32 i = 0; i < mPtrNum; ++i)
        {
            if (mPtrs[i] == ptr)
                return i;
        }
        return -1;
    }

    void createVacancy(s32 pos, s32 num);
    void insert(s32 pos, void* ptr);
    void insertArray(s32 pos, void* array, s32 array_length, s32 elem_size);
    bool checkInsert(s32 pos, s32 num);
    void sort(CompareCallbackImpl cmp);
    void heapSort(CompareCallbackImpl cmp);
    s32 compare(const PtrArrayImpl& o, CompareCallbackImpl cmp) const;
    void uniq(CompareCallbackImpl cmp);
    s32 binarySearch(const void* ptr, CompareCallbackImpl cmp) const;

protected:
    s32 mPtrNum;
    s32 mPtrNumMax;
    void** mPtrs;
};
#ifdef cafe
static_assert(sizeof(PtrArrayImpl) == 0xC, "PtrArrayImpl size mismatch");
#endif // cafe

template <typename T>
class PtrArray : public PtrArrayImpl
{
public:
    PtrArray()
        : PtrArrayImpl()
    {
    }

    T* at(s32 n) const { return static_cast<T*>(PtrArrayImpl::at(n)); }
    T* unsafeAt(s32 n) const { return static_cast<T*>(PtrArrayImpl::unsafeAt(n)); }
    T* operator[](s32 n) const { return at(n); }
    T* front() const { return static_cast<T*>(PtrArrayImpl::front()); }
    T* back() const { return static_cast<T*>(PtrArrayImpl::back()); }
    void pushBack(T* ptr) { PtrArrayImpl::pushBack((void*)ptr); }
    void pushFront(T* ptr) { PtrArrayImpl::pushFront((void*)ptr); }
    T* popBack() { return static_cast<T*>(PtrArrayImpl::popBack()); }
    T* popFront() { return static_cast<T*>(PtrArrayImpl::popFront()); }
    void insert(s32 pos, T* ptr) { PtrArrayImpl::insert(pos, (void*)ptr); }
    void insert(s32 pos, T* array, s32 array_length) { PtrArrayImpl::insertArray(pos, (void*)array, array_length, sizeof(T)); }
    void replace(s32 pos, T* ptr) { PtrArrayImpl::replace(pos, (void*)ptr); }
    s32 indexOf(const T* ptr) const { return PtrArrayImpl::indexOf(ptr); }
    void* getWork() const { return mPtrs; }
    // void sort() { sort(compareT); }
    void sort(CompareCallbackImpl cmp) { PtrArrayImpl::sort(cmp); }
    // void heapSort() { heapSort(compareT); }
    void heapSort(CompareCallbackImpl cmp) { PtrArrayImpl::heapSort(cmp); }
    bool equal(const PtrArray<T>* o, CompareCallbackImpl cmp) const { return PtrArrayImpl::equal(*o, cmp); }
    s32 compare(const PtrArray<T>* o, CompareCallbackImpl cmp) const { return PtrArrayImpl::compare(*o, cmp); }
    T* find(const T* ptr) const;
    T* find(const T* ptr, CompareCallbackImpl cmp) const;
    s32 search(const T* ptr) const;
    s32 search(const T* ptr, CompareCallbackImpl cmp) const;
    s32 binarySearch(const T* ptr) const;
    s32 binarySearch(const T* ptr, CompareCallbackImpl cmp) const;
    bool operator==(const PtrArray<T>&) const;
    bool operator!=(const PtrArray<T>&) const;
    bool operator<(const PtrArray<T>&) const;
    bool operator<=(const PtrArray<T>&) const;
    bool operator>(const PtrArray<T>&) const;
    bool operator>=(const PtrArray<T>&) const;
    void uniq();
    void uniq(CompareCallbackImpl cmp);

public:
    class iterator
    {
    public:
        iterator(T* const* pptr)
            : mPPtr(pptr)
        {
        }

    public:
        iterator& operator++()
        {
            ++mPPtr;
            return *this;
        }

        T& operator*() const
        {
            return **mPPtr;
        }

        T* operator->() const
        {
            return *mPPtr;
        }

        friend bool operator==(const iterator& it1, const iterator& it2)
        {
            return it1.mPPtr == it2.mPPtr;
        }

        friend bool operator!=(const iterator& it1, const iterator& it2)
        {
            return !(it1 == it2);
        }

        T* getPtr() const
        {
            return *mPPtr;
        }

    protected:
        T* const* mPPtr;
    };

    class constIterator
    {
    public:
        constIterator(const T* const* pptr)
            : mPPtr(pptr)
        {
        }

    public:
        constIterator& operator++()
        {
            ++mPPtr;
            return *this;
        }

        const T& operator*() const
        {
            return **mPPtr;
        }

        const T* operator->() const
        {
            return *mPPtr;
        }

        friend bool operator==(const constIterator& it1, const constIterator& it2)
        {
            return it1.mPPtr == it2.mPPtr;
        }

        friend bool operator!=(const constIterator& it1, const constIterator& it2)
        {
            return !(it1 == it2);
        }

        const T* getPtr() const
        {
            return *mPPtr;
        }

    private:
        const T* const* mPPtr;
    };

public:
    iterator begin() const
    {
        return iterator(reinterpret_cast<T**>(mPtrs));
    }

    iterator end() const
    {
        return iterator(reinterpret_cast<T**>(mPtrs) + mPtrNum);
    }

    constIterator constBegin() const
    {
        return constIterator(reinterpret_cast<T**>(mPtrs));
    }
    constIterator constEnd() const
    {
        return constIterator(reinterpret_cast<T**>(mPtrs) + mPtrNum);
    }

// protected:
//     static s32 compareT(const T*, const T*);
};

template <typename T, s32 N>
class FixedPtrArray : public PtrArray<T>
{
public:
    FixedPtrArray()
        : PtrArray<T>()
    {
        PtrArray<T>::setBuffer(N, mWork);
    }

protected:
    void setBuffer(s32 ptrNumMax, void* buf);
    void allocBuffer(s32 ptrNumMax, s32 alignment);
    bool tryAllocBuffer(s32 ptrNumMax, s32 alignment);
    void freeBuffer();

protected:
    u8 mWork[N*sizeof(void*)];
};
