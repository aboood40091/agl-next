#pragma once

#include <math/rio_MathTypes.h>
#if RIO_IS_WIN
#include <misc/gl/rio_GL.h>

#include <memory>
#endif // RIO_IS_WIN

namespace agl {

#if RIO_IS_WIN

class RenderBufferHandle
{
public:
    RenderBufferHandle()
    {
        RIO_GL_CALL(glGenFramebuffers(1, &mHandle));
        RIO_ASSERT(mHandle != GL_NONE);
    }

    ~RenderBufferHandle()
    {
        if (mHandle != GL_NONE)
        {
            RIO_GL_CALL(glDeleteFramebuffers(1, &mHandle));
            mHandle = GL_NONE;
        }
    }

    void bind() const
    {
        RIO_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mHandle));
    }

private:
    u32 mHandle;
};

#endif // RIO_IS_WIN

class RenderTargetColor;
class RenderTargetDepth;

class RenderBuffer
{
public:
    RenderBuffer();
    RenderBuffer(const rio::BaseVec2i& size);

public:
    RenderTargetColor* getRenderTargetColor() const
    {
        return mColorTarget;
    }

    RenderTargetDepth* getRenderTargetDepth() const
    {
        return mDepthTarget;
    }

    void setRenderTargetColor(RenderTargetColor* target)
    {
        mColorTarget = target;
    }

    void setRenderTargetDepth(RenderTargetDepth* target)
    {
        mDepthTarget = target;
    }

    void setRenderTargetColorNull() { setRenderTargetColor(nullptr); }
    void setRenderTargetDepthNull() { setRenderTargetDepth(nullptr); }

    const rio::BaseVec2i& getSize() const
    {
        return mSize;
    }

    void setSize(u32 w, u32 h)
    {
        mSize.x = w;
        mSize.y = h;
        resetScissor();
    }

    void setSize(const rio::BaseVec2i& size)
    {
        mSize = size;
        resetScissor();
    }

    const rio::BaseVec2f& getScissorPos() const
    {
        return mScissorPos;
    }

    const rio::BaseVec2f& getScissorSize() const
    {
        return mScissorSize;
    }

    void setScissor(f32 x, f32 y, f32 w, f32 h)
    {
        mScissorPos.x = x;
        mScissorPos.y = y;
        mScissorSize.x = w;
        mScissorSize.y = h;
    }

    void setScissor(const rio::BaseVec2f& pos, const rio::BaseVec2f& size)
    {
        mScissorPos = pos;
        mScissorSize = size;
    }

    void resetScissor()
    {
        mScissorPos.x = 0.0f;
        mScissorPos.y = 0.0f;
        mScissorSize.x = 1.0f;
        mScissorSize.y = 1.0f;
    }

    void bind() const;

private:
    rio::BaseVec2i      mSize;
    rio::BaseVec2f      mScissorPos;
    rio::BaseVec2f      mScissorSize;
    RenderTargetColor*  mColorTarget;
    RenderTargetDepth*  mDepthTarget;
#if RIO_IS_WIN
    std::unique_ptr<RenderBufferHandle> mHandle;
#endif // RIO_IS_WIN
};
//static_assert(sizeof(RenderBuffer) == 0x50, "agl::RenderBuffer size mistmatch");

}
