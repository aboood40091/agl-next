#pragma once

#include <common/aglTextureEnum.h>
#include <gpu/rio_Texture.h>

#include <cafe/gx2/gx2Surface.h>

#include <algorithm>
#if RIO_IS_DESKTOP
#include <memory>
#endif // RIO_IS_DESKTOP

namespace agl {

class TextureSampler;

#if RIO_IS_DESKTOP

class TextureHandle
{
public:
    TextureHandle(u32 handle = GL_NONE)
    {
        if (handle != GL_NONE)
        {
            mHandle = handle;
            mSelfAlloc = false;
        }
        else
        {
            RIO_GL_CALL(glGenTextures(1, &mHandle));
            RIO_ASSERT(mHandle != GL_NONE);
            mSelfAlloc = true;
        }
    }

    ~TextureHandle()
    {
        if (mSelfAlloc && mHandle != GL_NONE)
        {
            RIO_GL_CALL(glDeleteTextures(1, &mHandle));
            mHandle = GL_NONE;
        }
    }

    u32 getInnerHandle() const
    {
        return mHandle;
    }

    void bind() const
    {
        RIO_GL_CALL(glBindTexture(GL_TEXTURE_2D, mHandle));
    }

    void bindTarget(GLenum attachment, GLenum target = GL_FRAMEBUFFER, u32 mip_level = 0) const
    {
        RIO_GL_CALL(glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, mHandle, mip_level));
    }

private:
    u32 mHandle;
    bool mSelfAlloc;

    friend class TextureSampler;
};

#endif // RIO_IS_DESKTOP

class RenderTargetColor;
class RenderTargetDepth;

class TextureData
{
public:
    TextureData();
    TextureData(const rio::Texture2D& texture) : TextureData(texture, false, false) {}
    TextureData(const rio::Texture2D& texture, bool color_target, bool depth_target);
    virtual ~TextureData() { }

    bool operator==(const TextureData& rhs) const;
    bool operator!=(const TextureData& rhs) const { return !(*this == rhs); }

    void shareMemory(const TextureData& from);

    TextureFormat getTextureFormat() const { return mFormat; }

    u32 getWidth(s32 mip_level = 0) const { return std::max<s32>(mSurface.width >> mip_level, mMinWidth); }
    u32 getHeight(s32 mip_level = 0) const { return std::max<s32>(mSurface.height >> mip_level, mMinHeight); }

#if RIO_IS_DESKTOP
    const std::shared_ptr<TextureHandle>& getHandle() const { return mHandle; }
    void setHandle(const std::shared_ptr<TextureHandle>& handle) { mHandle = handle; }
#endif // RIO_IS_DESKTOP

#if RIO_IS_CAFE
    void* getImagePtr() const { return mSurface.image; }
    void setImagePtr(void* ptr) { mSurface.image = ptr; }
#endif // RIO_IS_CAFE

    u32 getImageByteSize() const { return mSurface.imageSize; }

#if RIO_IS_CAFE
    void* getMipPtr() const { return mSurface.mipmaps; }
    void setMipPtr(void* ptr) { mSurface.mipmaps = ptr; }
#endif // RIO_IS_CAFE

    u32 getMipByteSize() const { return mSurface.mipmapSize; }

#if RIO_IS_CAFE
    u32 getAlignment() const { return mSurface.alignment; }
#endif // RIO_IS_CAFE

#if RIO_IS_DESKTOP
    const rio::NativeTextureFormat& getNativeTextureFormat() const { return mSurface.nativeFormat; }
#endif // RIO_IS_DESKTOP

    const rio::NativeSurface2D& getSurface() const { return mSurface; }

    TextureCompSel getComponentR() const { return mCompR; }
    TextureCompSel getComponentG() const { return mCompG; }
    TextureCompSel getComponentB() const { return mCompB; }
    TextureCompSel getComponentA() const { return mCompA; }

    void setTextureCompSel(TextureCompSel r, TextureCompSel g, TextureCompSel b, TextureCompSel a)
    {
        mCompR = r;
        mCompG = g;
        mCompB = b;
        mCompA = a;
    }

    void initialize(TextureFormat format, u32 width, u32 height, u32 mip_level_num);

    u32 getMipLevelNum() const { return mSurface.mipLevels; }
    void setMipLevelNum(u32 mip_level_num);

    u32 getMipLevelByteSize(s32 mip_level) const;

    void invalidateGPUCache() const;

#if RIO_IS_CAFE
    void setSurfaceSwizzle(u32 swizzle) { GX2SetSurfaceSwizzle(&mSurface, swizzle); }
#endif // RIO_IS_CAFE

private:
    void initializeSize_(u32 width, u32 height);

public:
    void initializeFromSurface(const GX2Surface& surface);

private:
    TextureFormat mFormat;
    rio::NativeSurface2D mSurface;
#if RIO_IS_DESKTOP
    std::shared_ptr<TextureHandle> mHandle;
#endif // RIO_IS_DESKTOP
    u32 mMinWidth;
    u32 mMinHeight;
    u32 mMaxMipLevel;
    TextureCompSel mCompR;
    TextureCompSel mCompG;
    TextureCompSel mCompB;
    TextureCompSel mCompA;
};
//static_assert(sizeof(TextureData) == 0x9C, "agl::TextureData size mismatch");

inline bool
TextureData::operator==(const TextureData& rhs) const
{
    return (
      //    mSurface.dim       == rhs.mSurface.dim
      /*&&*/mSurface.width     == rhs.mSurface.width
        &&  mSurface.height    == rhs.mSurface.height
      //&&  mSurface.depth     == rhs.mSurface.depth
        &&  mSurface.mipLevels == rhs.mSurface.mipLevels
        &&  mSurface.format    == rhs.mSurface.format
#if RIO_IS_CAFE
        &&  mSurface.swizzle   == rhs.mSurface.swizzle
        &&  mSurface.tileMode  == rhs.mSurface.tileMode
#endif // RIO_IS_CAFE
      //&&  mSurface.aa        == rhs.mSurface.aa
    );
}

inline void
TextureData::shareMemory(const TextureData& from)
{
    mSurface.image = from.mSurface.image;
    mSurface.mipmaps = from.mSurface.mipmaps;

#if RIO_IS_DESKTOP
    mHandle = from.mHandle;
#endif // RIO_IS_DESKTOP
}

inline u32
TextureData::getMipLevelByteSize(s32 mip_level) const
{
    RIO_ASSERT(mip_level >= 0);

    if (mip_level == 0)
        return getImageByteSize();

    mip_level--;
    s32 mip_level_num = getMipLevelNum() - 1;
    RIO_ASSERT(mip_level_num >= 0);

    if (mip_level >= mip_level_num)
        return 0;

    if (mip_level == mip_level_num - 1)
        return getMipByteSize() - mSurface.mipLevelOffset[mip_level];

    return mSurface.mipLevelOffset[mip_level + 1] - mSurface.mipLevelOffset[mip_level];
}

}
