#pragma once

#include <detail/aglGX2.h>

namespace agl {

template <typename T>
void RenderTarget<T>::applyTextureData(const TextureData& texture_data)
{
    applyTextureData(texture_data, mMipLevel);
}

template <typename T>
void RenderTarget<T>::applyTextureData(const TextureData& texture_data, u32 mip_level)
{
    if (mTextureData != texture_data)
        applyTextureData_(texture_data, mip_level);

    else
    {
        mTextureData.shareMemory(texture_data);

#if RIO_IS_CAFE
        void* image_ptr = texture_data.getImagePtr();
        void* mip_ptr = texture_data.getMipPtr();

        static_cast<T*>(this)->mInnerBuffer.surface.imagePtr = image_ptr;
        static_cast<T*>(this)->mInnerBuffer.surface.mipPtr   = mip_ptr;
#endif // RIO_IS_CAFE

        setMipLevel(mip_level);
    }
}

template <typename T>
void RenderTarget<T>::applyTextureData_(const TextureData& texture_data, u32 mip_level)
{
    mTextureData = texture_data;
    mMipLevel = mip_level;
    static_cast<T*>(this)->onApplyTextureData_();
    mUpdateRegs = true;
}

inline void
RenderTargetColor::bind() const
{
    if (mUpdateRegs)
    {
        initRegs_();
        mUpdateRegs = false;
    }

#if RIO_IS_CAFE
    GX2SetColorBuffer(&mInnerBuffer, GX2_RENDER_TARGET_0);
#elif RIO_IS_DESKTOP
    const std::shared_ptr<TextureHandle>& handle = mTextureData.getHandle();
    RIO_ASSERT(handle);
    handle->bindTarget(GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER, mMipLevel);
#endif
}

inline void
RenderTargetDepth::bind() const
{
    if (mUpdateRegs)
    {
        initRegs_();
        mUpdateRegs = false;
    }

#if RIO_IS_CAFE
    GX2SetDepthBuffer(&mInnerBuffer);
#elif RIO_IS_DESKTOP
    const std::shared_ptr<TextureHandle>& handle = mTextureData.getHandle();
    RIO_ASSERT(handle);
    handle->bindTarget(
        mTextureData.getTextureFormat() == cTextureFormat_Depth_24_uNorm_Stencil_8
            ? GL_DEPTH_STENCIL_ATTACHMENT
            : GL_DEPTH_ATTACHMENT,
        GL_FRAMEBUFFER, mMipLevel
    );
#endif
}

}
