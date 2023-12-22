#pragma once

#include <common/aglVertexEnum.h>
#include <container/SafeArray.h>

#if RIO_IS_CAFE
#include <cafe/gx2.h>
#endif // RIO_IS_CAFE

namespace agl {

#if RIO_IS_WIN

struct VertexStreamInternalFormat
{
    u8      elem_count;
    bool    normalized;
    bool    integer;
    u32     type;
};

#endif // RIO_IS_WIN

class VertexBuffer
{
    struct Stream_
    {
        Stream_()
            : mFormat(cVertexStreamFormat_8_uNorm)
            , mOffset(0)
            , mEnable(false)
        {
        }

        VertexStreamFormat mFormat;
        u32 mOffset;

        bool mEnable;

#if RIO_IS_CAFE
        GX2CompSel mCompSel;
        u32 mDivisor;
        GX2EndianSwapMode mEndianSwap;
        GX2AttribIndexType mIndexType;
#elif RIO_IS_WIN
        VertexStreamInternalFormat mInternalFormat;
#endif
    };
#if RIO_IS_CAFE
    static_assert(sizeof(Stream_) == 0x1C);
#endif // RIO_IS_CAFE

public:
    static const u32 cVertexStreamMax = 16;

public:
    VertexBuffer();
    virtual ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) = delete;

    VertexStreamFormat getStreamFormat(s32 index) const { return mStream[index].mFormat; }
    u32 getStreamOffset(s32 index) const { return mStream[index].mOffset; }

    bool isEnable(s32 index) const { return mStream[index].mEnable; }

#if RIO_IS_CAFE
    GX2CompSel getStreamCompSel(s32 index) const { return mStream[index].mCompSel; }
    u32 getStreamDivisor(s32 index) const { return mStream[index].mDivisor; }
    GX2EndianSwapMode getStreamEndianSwap(s32 index) const { return mStream[index].mEndianSwap; }
    GX2AttribIndexType getStreamIndexType(s32 index) const { return mStream[index].mIndexType; }
#elif RIO_IS_WIN
    const VertexStreamInternalFormat& getStreamInternalFormat(s32 index) const { return mStream[index].mInternalFormat; }
#endif

    const void* getBufferPtr() const { return mpBuffer; }
    u32 getStride() const { return mStride; }
    u32 getVertexNum() const { return mVertexNum; }
    u32 getBufferByteSize() const { return mBufferByteSize; }

#if RIO_IS_WIN
    u32 getHandle() const { return mHandle; }
#endif // RIO_IS_WIN

    void setUpBuffer(const void* buffer, u32 stride, u32 buffer_byte_size);
    void setUpStream(s32 index, VertexStreamFormat format, u32 offset);

private:
    void cleanUp_();

private:
    SafeArray<Stream_, cVertexStreamMax> mStream;
    const void* mpBuffer;
    u32 mStride;
    u32 mVertexNum;
    u32 mBufferByteSize;
#if RIO_IS_WIN
    u32 mHandle;
#endif // RIO_IS_WIN
};
//static_assert(sizeof(VertexBuffer) == 0x1D4, "agl::VertexBuffer size mismatch");

}
