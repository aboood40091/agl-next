#include <common/aglVertexBuffer.h>

#if RIO_IS_CAFE
#include <coreinit/cache.h>
#elif RIO_IS_DESKTOP
#include <misc/gl/rio_GL.h>
#endif

namespace agl {

VertexBuffer::VertexBuffer()
    : mpBuffer(nullptr)
    , mStride(0)
    , mVertexNum(0)
    , mBufferByteSize(0)
#if RIO_IS_DESKTOP
    , mHandle(GL_NONE)
#endif // RIO_IS_DESKTOP
{
}

VertexBuffer::~VertexBuffer()
{
    cleanUp_();
}

void VertexBuffer::cleanUp_()
{
    for (u32 i = 0; i < cVertexStreamMax; i++)
        mStream[i].mEnable = false;

    mpBuffer = nullptr;
    mBufferByteSize = 0;
    mStride = 0;
    mVertexNum = 0;

#if RIO_IS_DESKTOP
    if (mHandle != GL_NONE)
    {
        RIO_GL_CALL(glDeleteBuffers(1, &mHandle));
        mHandle = GL_NONE;
    }
#endif // RIO_IS_DESKTOP
}

void VertexBuffer::setUpBuffer(const void* buffer, u32 stride, u32 buffer_byte_size)
{
    cleanUp_();

    RIO_ASSERT(buffer != nullptr);
    RIO_ASSERT(buffer_byte_size != 0);

    mpBuffer = buffer;
    mStride = stride;

    if (stride == 0)
    {
        mVertexNum = 1;
        mBufferByteSize = buffer_byte_size;
    }
    else
    {
        mVertexNum = buffer_byte_size / stride;
        mBufferByteSize = stride * mVertexNum;
    }

    RIO_ASSERT(buffer_byte_size == mBufferByteSize);

#if RIO_IS_CAFE
    DCFlushRangeNoSync(const_cast<void*>(mpBuffer), mBufferByteSize);
#elif RIO_IS_DESKTOP
    RIO_GL_CALL(glGenBuffers(1, &mHandle));
    RIO_ASSERT(mHandle != GL_NONE);
    RIO_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mHandle));
    RIO_GL_CALL(glBufferData(GL_ARRAY_BUFFER, mBufferByteSize, mpBuffer, GL_STATIC_DRAW));
#endif
}

void VertexBuffer::setUpStream(s32 index, VertexStreamFormat format, u32 offset)
{
    mStream[index].mFormat = format;
    mStream[index].mOffset = offset;
    mStream[index].mEnable = true;

#if RIO_IS_CAFE

    const GX2CompSel comp_sel[] = {
        GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01,
        GX2_COMP_SEL_X001, GX2_COMP_SEL_X001,
        GX2_COMP_SEL_XY01, GX2_COMP_SEL_X001,
        GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01,
        GX2_COMP_SEL_XY01, GX2_COMP_SEL_XYZ1,
        GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW,
        GX2_COMP_SEL_XY01, GX2_COMP_SEL_XY01,
        GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW,
        GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZ1,
        GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW
    };

    mStream[index].mCompSel = comp_sel[format & 0xff];
    mStream[index].mDivisor = 0;
    mStream[index].mEndianSwap = GX2_ENDIANSWAP_DEFAULT;
    mStream[index].mIndexType = GX2_ATTRIB_INDEX_VERTEX_ID;

#elif RIO_IS_DESKTOP

    switch (format)
    {
    case cVertexStreamFormat_8_uNorm:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_8_uNorm:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_8_8_8_uNorm:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_uInt:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_8_uInt:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_8_8_8_uInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_uInt_to_float:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_8_uInt_to_float:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_8_8_8_uInt_to_float:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_BYTE;
        break;
    case cVertexStreamFormat_8_sNorm:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_8_sNorm:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_8_8_8_sNorm:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_sInt:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_8_sInt:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_8_8_8_sInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_sInt_to_float:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_8_sInt_to_float:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_8_8_8_8_sInt_to_float:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_BYTE;
        break;
    case cVertexStreamFormat_10_11_11_float:
        mStream[index].mInternalFormat.elem_count = 3;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT_10F_11F_11F_REV;
        break;
    case cVertexStreamFormat_10_10_10_2_uNorm:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT_2_10_10_10_REV;
        break;
    case cVertexStreamFormat_10_10_10_2_uInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT_2_10_10_10_REV;
        break;
    case cVertexStreamFormat_10_10_10_2_sNorm:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_INT_2_10_10_10_REV;
        break;
    case cVertexStreamFormat_10_10_10_2_sInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_INT_2_10_10_10_REV;
        break;
    case cVertexStreamFormat_16_uNorm:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_16_uNorm:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_16_16_16_uNorm:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_uInt:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_16_uInt:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_16_16_16_uInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_uInt_to_float:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_16_uInt_to_float:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_16_16_16_uInt_to_float:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_SHORT;
        break;
    case cVertexStreamFormat_16_sNorm:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_16_sNorm:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_16_16_16_sNorm:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = true;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_sInt:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_16_sInt:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_16_16_16_sInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_sInt_to_float:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_16_sInt_to_float:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_16_16_16_sInt_to_float:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_SHORT;
        break;
    case cVertexStreamFormat_16_float:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_HALF_FLOAT;
        break;
    case cVertexStreamFormat_16_16_float:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_HALF_FLOAT;
        break;
    case cVertexStreamFormat_16_16_16_16_float:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_HALF_FLOAT;
        break;
    case cVertexStreamFormat_32_uInt:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT;
        break;
    case cVertexStreamFormat_32_32_uInt:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT;
        break;
    case cVertexStreamFormat_32_32_32_uInt:
        mStream[index].mInternalFormat.elem_count = 3;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT;
        break;
    case cVertexStreamFormat_32_32_32_32_uInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_UNSIGNED_INT;
        break;
    case cVertexStreamFormat_32_sInt:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_INT;
        break;
    case cVertexStreamFormat_32_32_sInt:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_INT;
        break;
    case cVertexStreamFormat_32_32_32_sInt:
        mStream[index].mInternalFormat.elem_count = 3;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_INT;
        break;
    case cVertexStreamFormat_32_32_32_32_sInt:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = true;
        mStream[index].mInternalFormat.type = GL_INT;
        break;
    case cVertexStreamFormat_32_float:
        mStream[index].mInternalFormat.elem_count = 1;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_FLOAT;
        break;
    case cVertexStreamFormat_32_32_float:
        mStream[index].mInternalFormat.elem_count = 2;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_FLOAT;
        break;
    case cVertexStreamFormat_32_32_32_float:
        mStream[index].mInternalFormat.elem_count = 3;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_FLOAT;
        break;
    case cVertexStreamFormat_32_32_32_32_float:
        mStream[index].mInternalFormat.elem_count = 4;
        mStream[index].mInternalFormat.normalized = false;
        mStream[index].mInternalFormat.integer = false;
        mStream[index].mInternalFormat.type = GL_FLOAT;
        break;
    default:
        RIO_ASSERT(false);
    }

#endif
}

}
