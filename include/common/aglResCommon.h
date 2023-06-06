#pragma once

#include <misc/rio_Types.h>

namespace agl {

void ModifyEndianU32(bool is_le, void* ptr, size_t size);

template <typename _DataType>
class ResCommon
{
public:
    typedef _DataType DataType;

public:
    ResCommon()
         : mpData(NULL)
     {
     }

     ResCommon(const void* data)
         : mpData(static_cast<const DataType*>(data))
     {
     }

    bool isValid() const
    {
        return mpData != NULL;
    }

    void verify() const
    {
        if (ref().mSigWord != DataType::cSignature)
        {
            const char* signature = ptr()->mSignature;
            RIO_LOG("Wrong binary. [%c%c%c%c].\n",
                signature[0], signature[1],
                signature[2], signature[3]);
            RIO_ASSERT(false);
        }

        if (ref().mVersion != DataType::cVersion)
        {
            RIO_LOG("Version error.current:%d binary:%d\n",
                DataType::cVersion,
                ref().mVersion);
            RIO_ASSERT(false);
        }
    }

    DataType* ptr()
    {
        return const_cast<DataType*>(mpData);
    }

    const DataType* ptr() const
    {
        return mpData;
    }

    DataType& ref()
    {
        RIO_ASSERT(isValid());
        return *ptr();
    }

    const DataType& ref() const
    {
        RIO_ASSERT(isValid());
        return *ptr();
    }

private:
    const DataType* mpData;
};

#define AGL_RES_COMMON(class_name)                              \
    public:                                                     \
        class_name()                                            \
             : ResCommon<typename class_name::DataType>()       \
         {                                                      \
         }                                                      \
                                                                \
         class_name(const void* data)                           \
             : ResCommon<typename class_name::DataType>(data)   \
         {                                                      \
         }

template <typename DataType>
struct ResArrayData
{
    u32 mSize;
    u32 mNum;
    // DataType mData[];

    typedef DataType ElemType;
};

template <typename Type>
class ResArray : public ResCommon< ResArrayData<typename Type::DataType> >
{
    AGL_RES_COMMON(ResArray<Type>)

public:
    typedef Type ElemType;
    typedef typename Type::DataType ElemDataType;
    using DataType = typename ResArray<Type>::DataType;
    typedef ResCommon<DataType> Base;

public:
    class iterator
    {
    public:
        iterator(s32 index, ElemDataType* elem)
            : mIndex(index)
            , mElem(elem)
        {
        }

        bool operator==(const iterator& rhs) const
        {
            return mIndex == rhs.mIndex;
        }

        bool operator!=(const iterator& rhs) const { return !operator==(rhs); }

        iterator& operator++()
        {
            ++mIndex;
            mElem = (ElemDataType*)((uintptr_t)mElem + mElem->mSize);
            return *this;
        }

        ElemDataType& operator*() const { return *mElem; }
        ElemDataType* operator->() const { return mElem; }
        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        ElemDataType* mElem;
    };

    class constIterator
    {
    public:
        constIterator(s32 index, const ElemDataType* elem)
            : mIndex(index)
            , mElem(elem)
        {
        }

        bool operator==(const constIterator& rhs) const
        {
            return mIndex == rhs.mIndex;
        }

        bool operator!=(const constIterator& rhs) const { return !operator==(rhs); }

        constIterator& operator++()
        {
            ++mIndex;
            mElem = (const ElemDataType*)((uintptr_t)mElem + mElem->mSize);
            return *this;
        }

        const ElemDataType& operator*() const { return *mElem; }
        const ElemDataType* operator->() const { return mElem; }
        s32 getIndex() const { return mIndex; }

    private:
        s32 mIndex;
        const ElemDataType* mElem;
    };

public:
    iterator begin() { return iterator(0, (ElemDataType*)(Base::ptr() + 1)); }
    constIterator begin() const { return constIterator(0, (const ElemDataType*)(Base::ptr() + 1)); }

    iterator end() { return iterator(getNum(), NULL); }
    constIterator end() const { return constIterator(getNum(), NULL); }

public:
    u32 getNum() const
    {
        return Base::ref().mNum;
    }

    ElemType get(s32 n) const
    {
        RIO_ASSERT(0 <= n && n <= static_cast< int >( this->getNum() ));

        constIterator it = begin();
        constIterator it_end = constIterator(n, NULL);

        while (it != it_end)
            ++it;

        return &(*it);
    }

    void modifyEndianArray(bool is_le)
    {
        ModifyEndianU32(is_le, Base::ptr(), sizeof(DataType));

        for (iterator it = begin(), it_end = end(); it != it_end; ++it)
            ModifyEndianU32(is_le, &(*it), sizeof(ElemDataType));
    }
};

#define AGL_RES_ARRAY(class_name)                       \
    public:                                             \
        class_name()                                    \
             : ResArray<class_name::ElemType>()         \
         {                                              \
         }                                              \
                                                        \
         class_name(const void* data)                   \
             : ResArray<class_name::ElemType>(data)     \
         {                                              \
         }

}
