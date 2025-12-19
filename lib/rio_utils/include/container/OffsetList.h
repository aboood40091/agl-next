#pragma once

#include <container/rio_LinkList.h>

template <typename T>
class OffsetList : public rio::LinkList
{
private:
    typedef s32 (*CompareCallback)(const T*, const T*);

public:
    OffsetList()
        : rio::LinkList(), mOffset(-1)
    {
    }

    void initOffset(s32 offset) { mOffset = offset; }
    void clear();
    void unsafeClear();

    void pushBack(T* obj)
    {
        RIO_ASSERT(mOffset >= 0);
        rio::LinkList::pushBack(objToListNode(obj));
    }

    void pushFront(T* obj)
    {
        RIO_ASSERT(mOffset >= 0);
        rio::LinkList::pushFront(objToListNode(obj));
    }

    T* popBack() { return listNodeToObjWithNullCheck(rio::LinkList::popBack()); }
    T* popFront() { return listNodeToObjWithNullCheck(rio::LinkList::popFront()); }

    void insertBefore(const T* basis, T* obj)
    {
        rio::LinkList::insertBefore(objToListNode(basis), objToListNode(obj));
    }

    void insertAfter(const T* basis, T* obj)
    {
        rio::LinkList::insertAfter(objToListNode(basis), objToListNode(obj));
    }

    void erase(T* obj)
    {
        rio::LinkList::erase(objToListNode(obj));
    }

    T* front() const { return listNodeToObjWithNullCheck(rio::LinkList::front()); }
    T* back() const { return listNodeToObjWithNullCheck(rio::LinkList::back()); }

    T* prev(const T* obj) const
    {
        rio::LinkListNode* node = objToListNode(obj)->prev();
        if (node == &mStartEnd)
            return nullptr;

        return listNodeToObj(node);
    }

    T* next(const T* obj) const
    {
        rio::LinkListNode* node = objToListNode(obj)->next();
        if (node == &mStartEnd)
            return nullptr;

        return listNodeToObj(node);
    }

    T* nth(s32 index) const;
    s32 indexOf(const T* obj) const;
    bool isNodeLinked(const T* obj) const;
    void swap(T* obj1, T* obj2);
    void moveAfter(T* basis, T* obj);
    void moveBefore(T* basis, T* obj);
    void sort();
    void sort(CompareCallback cmp);
    void mergeSort();
    void mergeSort(CompareCallback cmp);
    T* find(const T* obj) const;
    T* find(const T* obj, CompareCallback cmp) const;
    void uniq();
    void uniq(CompareCallback cmp);

public:
    class iterator
    {
    public:
        iterator(T* ptr, s32 offset)
            : mPtr(ptr), mOffset(offset)
        {
        }

        iterator& operator++()
        {
            rio::LinkListNode* node = reinterpret_cast<rio::LinkListNode*>((uintptr_t)mPtr + mOffset)->next();
            mPtr = reinterpret_cast<T*>((uintptr_t)node - mOffset);
            return *this;
        }

        T& operator*() const
        {
            return *mPtr;
        }

        T* operator->() const
        {
            return mPtr;
        }

        friend bool operator==(const iterator& it1, const iterator& it2)
        {
            return it1.mPtr == it2.mPtr;
        }

        friend bool operator!=(const iterator& it1, const iterator& it2)
        {
            return !(it1 == it2);
        }

    protected:
        T* mPtr;
        s32 mOffset;
    };

public:
    iterator begin() const
    {
        return iterator(listNodeToObj(mStartEnd.next()), mOffset);
    }

    iterator end() const
    {
        return iterator(listNodeToObj(const_cast<rio::LinkListNode*>(&mStartEnd)), mOffset);
    }

protected:
    rio::LinkListNode* objToListNode(const T* obj) const
    {
        return reinterpret_cast<rio::LinkListNode*>((uintptr_t)obj + mOffset);
    }

    T* listNodeToObj(const rio::LinkListNode* node) const
    {
        return reinterpret_cast<T*>((uintptr_t)node - mOffset);
    }

    T* listNodeToObjWithNullCheck(const rio::LinkListNode* node) const
    {
        return node ? listNodeToObj(node) : nullptr;
    }

protected:
    s32 mOffset;
};
