#pragma once

#include <common/aglShaderLocation.h>
#include <common/aglTextureData.h>
#include <gfx/rio_Color.h>
#include <gpu/rio_TextureSampler.h>
#include <misc/rio_BitFlag.h>


#if RIO_IS_CAFE
#include <cafe/gx2/gx2Texture.h>
#endif // RIO_IS_CAFE

namespace agl {

class TextureSampler
{
public:
    TextureSampler();
    TextureSampler(const TextureData& texture_data);
    ~TextureSampler() { }

    const TextureData& getTextureData() const { return mTextureData; }
    void applyTextureData(const TextureData& texture_data);

    void setWrapX(TextureWrapType wrap_x);
    void setWrapY(TextureWrapType wrap_y);
    void setWrapZ(TextureWrapType wrap_z);

    void setWrap(
        TextureWrapType wrap_x,
        TextureWrapType wrap_y,
        TextureWrapType wrap_z
    );

    void setFilterMag(TextureFilterType filter_mag);
    void setFilterMin(TextureFilterType filter_min);
    void setFilterMip(TextureMipFilterType filter_mip);

    void setFilter(
        TextureFilterType    filter_mag,
        TextureFilterType    filter_min,
        TextureMipFilterType filter_mip
    );

    void setMaxAnisoRatio(TextureAnisoRatio max_aniso);

    void setMipParam(f32 lod_min, f32 lod_max, f32 lod_bias);

    void setBorderColor(f32 r, f32 g, f32 b, f32 a);
    void setBorderColor(const rio::Color4f& color);

    void setUnk1(u8 v) { _e9 = v; }
    u8 getUnk1() const { return _e9; }

    void setDepthCompareEnable(bool enable);
    void setDepthCompareFunc(rio::Graphics::CompareFunc func);

    bool activate(const SamplerLocation& location, s32 slot) const;
    bool activate(s32 vs, s32 fs, s32 gs, s32 slot) const;

private:
    void applyTextureData_(const TextureData& texture_data);

    void initRegs_() const;

    void initTexture_() const;
    void initWrap_() const;
    void initFilter_() const;
    void initMipParam_() const;
    void initDepthComp_() const;
    void initBorderColor_() const;

private:
    TextureData mTextureData;
    rio::TextureSampler2D mSamplerInner;
    u8 _e9;
#if RIO_IS_CAFE
    mutable GX2Texture mGX2Texture;
#endif // RIO_IS_CAFE
};
//static_assert(sizeof(TextureSampler) == 0x1A0, "agl::TextureSampler size mismatch");

}

#ifdef __cplusplus

#include <common/aglTextureSampler.hpp>

#endif // __cplusplus
