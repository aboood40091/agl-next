#include <common/aglVertexAttribute.h>
#include <common/aglVertexBuffer.h>

#if RIO_IS_CAFE
#include <coreinit/cache.h>
#elif RIO_IS_DESKTOP
#include <misc/gl/rio_GL.h>
#endif

namespace agl {

VertexAttribute::VertexAttribute()
    : mSetupFinish(false)
    , mCreateFinish(false)
#if RIO_IS_CAFE
    , mpFetchShaderBuf(nullptr)
#elif RIO_IS_DESKTOP
    , mHandle(0)
#endif
{
}

VertexAttribute::~VertexAttribute()
{
    destroy();
}

void VertexAttribute::create(u32 buffer_max)
{
    RIO_ASSERT(0 < buffer_max && buffer_max <= cVertexAttributeMax);
    mVertexBuffer.allocBuffer(buffer_max);

#if RIO_IS_CAFE
    mpFetchShaderBuf = static_cast<u8*>(rio::MemUtil::alloc(GX2CalcFetchShaderSize(cVertexAttributeMax), GX2_SHADER_ALIGNMENT));
#elif RIO_IS_DESKTOP
    RIO_GL_CALL(glGenVertexArrays(1, &mHandle));
    RIO_ASSERT(mHandle != GL_NONE);
#endif

    mCreateFinish = true;

    cleanUp();
}

void VertexAttribute::cleanUp()
{
    for (s32 i = 0; i < cVertexAttributeMax; i++)
        mAttribute[i].mpVertexBuffer = nullptr;

    for (s32 i = 0; i < mVertexBuffer.size(); i++)
        mVertexBuffer[i] = nullptr;

    mSetupFinish = false;
}

void VertexAttribute::destroy()
{
    if (!mCreateFinish)
        return;

    mVertexBuffer.freeBuffer();

#if RIO_IS_CAFE
    if (mpFetchShaderBuf)
    {
        rio::MemUtil::free(mpFetchShaderBuf);
        mpFetchShaderBuf = nullptr;
    }
#elif RIO_IS_DESKTOP
    if (mHandle != GL_NONE)
    {
        RIO_GL_CALL(glDeleteVertexArrays(1, &mHandle));
        mHandle = GL_NONE;
    }
#endif

    mCreateFinish = false;
    mSetupFinish = false;
}

void VertexAttribute::setVertexStream(s16 location, const VertexBuffer* buffer, u32 stream_index)
{
    RIO_ASSERT(mCreateFinish);
    RIO_ASSERT(buffer->isEnable( stream_index ));

    if (location == -1)
        return;

    if (mAttribute[location].mpVertexBuffer != nullptr)
        mAttribute[location].mBufferIndex = disableVertexBuffer_(&(mAttribute[location]));

    if (buffer != nullptr)
        mAttribute[location].mBufferIndex = enableVertexBuffer_(&(mAttribute[location]), buffer, stream_index);
}

s32 VertexAttribute::enableVertexBuffer_(Attribute_* attr, const VertexBuffer* buffer, u32 stream_index)
{
    RIO_ASSERT(attr->mpVertexBuffer == nullptr);

    attr->mpVertexBuffer = buffer;
    attr->mStreamIndex = stream_index;

    s32 null_buffer_index = -1;

    for (s32 i = mVertexBuffer.size() - 1; i >= 0; i--)
    {
        if (mVertexBuffer[i] == attr->mpVertexBuffer)
            return i;

        if (mVertexBuffer[i] == nullptr)
            null_buffer_index = i;
    }

    RIO_ASSERT(null_buffer_index != -1);
    mVertexBuffer[null_buffer_index] = buffer;
    return null_buffer_index;
}

s32 VertexAttribute::disableVertexBuffer_(Attribute_* attr)
{
    const VertexBuffer* buffer = attr->mpVertexBuffer;

    attr->mpVertexBuffer = nullptr;
    attr->mStreamIndex = 1;

    for (s32 i = 0; i < cVertexAttributeMax; i++)
        if (buffer == mAttribute[i].mpVertexBuffer)
            return -1;

    mVertexBuffer[attr->mBufferIndex] = nullptr;
    return -1;
}

void VertexAttribute::setUp()
{
    RIO_ASSERT(mCreateFinish);

#if RIO_IS_CAFE

    UnsafeArray<GX2AttribStream, cVertexAttributeMax> stream_array;
    u32 stream_num = 0;

    for (s32 i = 0; i < cVertexAttributeMax; i++)
    {
        const Attribute_* attr = &(mAttribute[i]);
        GX2AttribStream* stream = &(stream_array[i]);

        if (attr->mpVertexBuffer != nullptr)
        {
            stream->buffer = attr->mBufferIndex;
            stream->offset = attr->mpVertexBuffer->getStreamOffset(attr->mStreamIndex);
            stream->location = i;
            stream->format = GX2AttribFormat(attr->mpVertexBuffer->getStreamFormat(attr->mStreamIndex));
            stream->destSel = attr->mpVertexBuffer->getStreamCompSel(attr->mStreamIndex);
            stream->indexType = attr->mpVertexBuffer->getStreamIndexType(attr->mStreamIndex);
            stream->aluDivisor = attr->mpVertexBuffer->getStreamDivisor(attr->mStreamIndex);
            stream->endianSwap = attr->mpVertexBuffer->getStreamEndianSwap(attr->mStreamIndex);

            stream_num++;
        }
    }

    GX2InitFetchShader(&mFetchShader, mpFetchShaderBuf, stream_num, stream_array.getBufferPtr());
    DCFlushRangeNoSync(mpFetchShaderBuf, GX2CalcFetchShaderSize(stream_num));

#elif RIO_IS_DESKTOP

    RIO_GL_CALL(glBindVertexArray(mHandle));

    for (s32 i = 0; i < cVertexAttributeMax; i++)
    {
        const Attribute_* attr = &(mAttribute[i]);

        if (attr->mpVertexBuffer != nullptr)
        {
            const VertexBuffer* buffer = attr->mpVertexBuffer;
            RIO_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer->getHandle()));

            const VertexStreamInternalFormat& internal_format = buffer->getStreamInternalFormat(attr->mStreamIndex);

            RIO_GL_CALL(glEnableVertexAttribArray(i));
            if (internal_format.integer)
            {
                RIO_GL_CALL(glVertexAttribIPointer(
                    i,
                    internal_format.elem_count,
                    internal_format.type,
                    buffer->getStride(),
                    (void*)buffer->getStreamOffset(attr->mStreamIndex)
                ));
            }
            else
            {
                RIO_GL_CALL(glVertexAttribPointer(
                    i,
                    internal_format.elem_count,
                    internal_format.type,
                    internal_format.normalized,
                    buffer->getStride(),
                    (void*)buffer->getStreamOffset(attr->mStreamIndex)
                ));
            }
        }
    }

    RIO_GL_CALL(glBindVertexArray(GL_NONE));

#endif

    mSetupFinish = true;
}

void VertexAttribute::activate() const
{
    RIO_ASSERT(mSetupFinish);

#if RIO_IS_CAFE

    GX2SetFetchShader(&mFetchShader);

    for (s32 i = 0; i < mVertexBuffer.size(); i++)
    {
        const VertexBuffer* buffer = mVertexBuffer[i];
        if (buffer != nullptr)
            GX2SetAttribBuffer(i, buffer->getBufferByteSize(), buffer->getStride(), buffer->getBufferPtr());
    }

#elif RIO_IS_DESKTOP

    RIO_GL_CALL(glBindVertexArray(mHandle));

#endif
}

}
