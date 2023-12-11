#include <common/aglRenderBuffer.h>
#include <common/aglRenderTarget.h>
#include <gfx/rio_Graphics.h>

namespace agl {

RenderBuffer::RenderBuffer()
    : mSize{1, 1}
    , mColorTarget(nullptr)
    , mDepthTarget(nullptr)
#if RIO_IS_WIN
    , mHandle(std::make_unique<RenderBufferHandle>())
#endif // RIO_IS_WIN
{
}

RenderBuffer::RenderBuffer(const rio::Vector2i& size)
    : mSize{size}
    , mColorTarget(nullptr)
    , mDepthTarget(nullptr)
#if RIO_IS_WIN
    , mHandle(std::make_unique<RenderBufferHandle>())
#endif // RIO_IS_WIN
{
}

void RenderBuffer::bind() const
{
    rio::Graphics::setViewport(0, 0, mSize.x, mSize.y, 0.0f, 1.0f, mSize.y);
    rio::Graphics::setScissor(0, 0, mSize.x, mSize.y, mSize.y);

#if RIO_IS_WIN
    mHandle->bind();
#endif // RIO_IS_WIN

    if (mColorTarget)
        mColorTarget->bind();

    if (mDepthTarget)
        mDepthTarget->bind();
}

}
