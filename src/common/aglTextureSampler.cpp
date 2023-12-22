#include <common/aglTextureSampler.h>
#include <misc/rio_MemUtil.h>

namespace agl {

TextureSampler::TextureSampler()
    : mTextureData()
    , mSamplerInner()
    , _e9(0)
{
}

TextureSampler::TextureSampler(const TextureData& texture_data)
    : mTextureData()
    , mSamplerInner()
    , _e9(0)
{
    applyTextureData(texture_data);
}

void TextureSampler::applyTextureData_(const TextureData& texture_data)
{
    mTextureData = texture_data;

#if RIO_IS_CAFE

    rio::MemUtil::copy(&mGX2Texture.surface, &mTextureData.getSurface(), sizeof(GX2Surface));
    mGX2Texture.viewFirstMip = 0;
    mGX2Texture.viewNumMips = mGX2Texture.surface.numMips;
    mGX2Texture.viewFirstSlice = 0;
    mGX2Texture.viewNumSlices = mGX2Texture.surface.depth;
    mGX2Texture.compSel = GX2_GET_COMP_SEL(mTextureData.getComponentR(),
                                           mTextureData.getComponentG(),
                                           mTextureData.getComponentB(),
                                           mTextureData.getComponentA());
    GX2InitTextureRegs(&mGX2Texture);

    mSamplerInner.linkNativeTexture2D(&mGX2Texture);

#elif RIO_IS_WIN

    const std::shared_ptr<TextureHandle>& handle = mTextureData.getHandle();
    RIO_ASSERT(handle);
    handle->bind();

    rio::Texture2DUtil::setSwizzleCurrent(
        mTextureData.getComponentR() << 24 |
        mTextureData.getComponentG() << 16 |
        mTextureData.getComponentB() <<  8 |
        mTextureData.getComponentA() <<  0
    );

    mSamplerInner.linkNativeTexture2D(handle->mHandle);

#endif
}

bool TextureSampler::activate(const SamplerLocation& location, s32 slot) const
{
    return activate(location.getVertexLocation(), location.getFragmentLocation(), location.getGeometryLocation(), slot);
}

bool TextureSampler::activate(s32 vs, s32 fs, s32 gs, s32 slot) const
{
    RIO_ASSERT(gs == -1);
    return mSamplerInner.tryBind(vs, fs, slot != -1 ? slot : 0);
}

}
