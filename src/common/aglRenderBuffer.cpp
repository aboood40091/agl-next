#include <common/aglRenderBuffer.h>
#include <common/aglRenderTarget.h>
#include <gfx/rio_Graphics.h>

#include <cmath>

namespace agl {

RenderBuffer::RenderBuffer()
    : mSize{1, 1}
    , mScissorPos{ 0.0f, 0.0f }
    , mScissorSize{ 1.0f, 1.0f }
    , mColorTarget(nullptr)
    , mDepthTarget(nullptr)
#if RIO_IS_DESKTOP
    , mHandle(std::make_unique<RenderBufferHandle>())
#endif // RIO_IS_DESKTOP
{
}

RenderBuffer::RenderBuffer(const rio::BaseVec2i& size)
    : mSize{size}
    , mScissorPos{ 0.0f, 0.0f }
    , mScissorSize{ 1.0f, 1.0f }
    , mColorTarget(nullptr)
    , mDepthTarget(nullptr)
#if RIO_IS_DESKTOP
    , mHandle(std::make_unique<RenderBufferHandle>())
#endif // RIO_IS_DESKTOP
{
}

void RenderBuffer::bind() const
{
    rio::Graphics::setViewport(0, 0, mSize.x, mSize.y, 0.0f, 1.0f, mSize.y);
    rio::Graphics::setScissor(
        mScissorPos.x * mSize.x,
        mScissorPos.y * mSize.y,
        std::min<s32>(std::lround(mScissorSize.x * mSize.x), mSize.x),
        std::min<s32>(std::lround(mScissorSize.y * mSize.y), mSize.y),
        mSize.y
    );

#if RIO_IS_DESKTOP
    mHandle->bind();
#endif // RIO_IS_DESKTOP

    if (mColorTarget)
        mColorTarget->bind();

    if (mDepthTarget)
        mDepthTarget->bind();
}

}
