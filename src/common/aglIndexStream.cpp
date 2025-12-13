#include <common/aglIndexStream.h>

#if RIO_IS_CAFE
#include <coreinit/cache.h>
#endif // RIO_IS_CAFE

namespace agl {

IndexStream::IndexStream()
    : mFormat(cIndexStreamFormat_u16)
    , mPrimitiveType(rio::Drawer::TRIANGLES)
    , mpBuffer(nullptr)
    , mCount(0)
    , mStride(0)
#if RIO_IS_DESKTOP
    , mHandle(GL_NONE)
#endif // RIO_IS_DESKTOP
{
}

IndexStream::~IndexStream()
{
    cleanUp_();
}

void IndexStream::cleanUp_()
{
    mpBuffer = nullptr;
    mCount = 0;
    mStride = 0;

#if RIO_IS_DESKTOP
    if (mHandle != GL_NONE)
    {
        RIO_GL_CALL(glDeleteBuffers(1, &mHandle));
        mHandle = GL_NONE;
    }
#endif // RIO_IS_DESKTOP
}

void IndexStream::setUpStream_(const void* addr, IndexStreamFormat format, u32 count)
{
    RIO_ASSERT(count != 0);
    RIO_ASSERT(addr != nullptr);

    cleanUp_();

    RIO_ASSERT(format == cIndexStreamFormat_u16 || format == cIndexStreamFormat_u32);

    mpBuffer = addr;
    mCount = count;
    mFormat = format;
    mStride = mFormat == cIndexStreamFormat_u16 ? sizeof(u16) : sizeof(u32);

#if RIO_IS_CAFE
    DCFlushRangeNoSync(const_cast<void*>(mpBuffer), mStride * mCount);
#elif RIO_IS_DESKTOP
    RIO_GL_CALL(glGenBuffers(1, &mHandle));
    RIO_ASSERT(mHandle != GL_NONE);
    RIO_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandle));
    RIO_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mStride * mCount, mpBuffer, GL_STATIC_DRAW));
#endif // RIO_IS_DESKTOP
}

}
