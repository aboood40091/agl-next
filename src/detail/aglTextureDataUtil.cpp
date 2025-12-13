#include <common/aglTextureData.h>
#include <common/aglTextureFormatInfo.h>
#include <detail/aglTextureDataUtil.h>

#include <cafe/gx2/gx2Enum.h>

#if RIO_IS_DESKTOP
#include <misc/rio_MemUtil.h>
#endif // RIO_IS_DESKTOP

namespace agl { namespace detail {

namespace {

static GX2SurfaceFormat s_surface_format[cTextureFormat_Num] = {
    GX2_SURFACE_FORMAT_INVALID,
    GX2_SURFACE_FORMAT_TC_R8_UNORM,
    GX2_SURFACE_FORMAT_TC_R8_UINT,
    GX2_SURFACE_FORMAT_TC_R8_SNORM,
    GX2_SURFACE_FORMAT_TC_R8_SINT,
    GX2_SURFACE_FORMAT_TCD_R16_UNORM,
    GX2_SURFACE_FORMAT_TC_R16_UINT,
    GX2_SURFACE_FORMAT_TC_R16_SNORM,
    GX2_SURFACE_FORMAT_TC_R16_SINT,
    GX2_SURFACE_FORMAT_TC_R16_FLOAT,
    GX2_SURFACE_FORMAT_TC_R8_G8_UNORM,
    GX2_SURFACE_FORMAT_TC_R8_G8_UINT,
    GX2_SURFACE_FORMAT_TC_R8_G8_SNORM,
    GX2_SURFACE_FORMAT_TC_R8_G8_SINT,
    GX2_SURFACE_FORMAT_TCS_R5_G6_B5_UNORM,
    GX2_SURFACE_FORMAT_TC_A1_B5_G5_R5_UNORM,
    GX2_SURFACE_FORMAT_TC_R4_G4_B4_A4_UNORM,
    GX2_SURFACE_FORMAT_TC_R5_G5_B5_A1_UNORM,
    GX2_SURFACE_FORMAT_TC_R32_UINT,
    GX2_SURFACE_FORMAT_TC_R32_SINT,
    GX2_SURFACE_FORMAT_TCD_R32_FLOAT,
    GX2_SURFACE_FORMAT_TC_R16_G16_UNORM,
    GX2_SURFACE_FORMAT_TC_R16_G16_UINT,
    GX2_SURFACE_FORMAT_TC_R16_G16_SNORM,
    GX2_SURFACE_FORMAT_TC_R16_G16_SINT,
    GX2_SURFACE_FORMAT_TC_R16_G16_FLOAT,
    GX2_SURFACE_FORMAT_TC_R11_G11_B10_FLOAT,
    GX2_SURFACE_FORMAT_TCS_A2_B10_G10_R10_UNORM,
    GX2_SURFACE_FORMAT_TC_A2_B10_G10_R10_UINT,
    GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,
    GX2_SURFACE_FORMAT_TC_R8_G8_B8_A8_UINT,
    GX2_SURFACE_FORMAT_TC_R8_G8_B8_A8_SNORM,
    GX2_SURFACE_FORMAT_TC_R8_G8_B8_A8_SINT,
    GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_SRGB,
    GX2_SURFACE_FORMAT_TCS_R10_G10_B10_A2_UNORM,
    GX2_SURFACE_FORMAT_TC_R10_G10_B10_A2_UINT,
    GX2_SURFACE_FORMAT_TC_R32_G32_UINT,
    GX2_SURFACE_FORMAT_TC_R32_G32_SINT,
    GX2_SURFACE_FORMAT_TC_R32_G32_FLOAT,
    GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_UNORM,
    GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_UINT,
    GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_SNORM,
    GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_SINT,
    GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_FLOAT,
    GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_UINT,
    GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_SINT,
    GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_FLOAT,
    GX2_SURFACE_FORMAT_T_BC1_UNORM,
    GX2_SURFACE_FORMAT_T_BC1_SRGB,
    GX2_SURFACE_FORMAT_T_BC2_UNORM,
    GX2_SURFACE_FORMAT_T_BC2_SRGB,
    GX2_SURFACE_FORMAT_T_BC3_UNORM,
    GX2_SURFACE_FORMAT_T_BC3_SRGB,
    GX2_SURFACE_FORMAT_T_BC4_UNORM,
    GX2_SURFACE_FORMAT_T_BC4_SNORM,
    GX2_SURFACE_FORMAT_T_BC5_UNORM,
    GX2_SURFACE_FORMAT_T_BC5_SNORM,
    GX2_SURFACE_FORMAT_TCD_R16_UNORM,
    GX2_SURFACE_FORMAT_TCD_R32_FLOAT,
    GX2_SURFACE_FORMAT_D_D24_S8_UNORM
};

}

TextureFormat TextureDataUtil::convFormatGX2ToAGL(GX2SurfaceFormat format, bool color_target, bool depth_target)
{
    RIO_ASSERT(!(color_target && depth_target));

    for (s32 i = 0; i < cTextureFormat_Num; i++)
    {
        if (format == s_surface_format[i])
        {
            TextureFormat ret = TextureFormat(i);
            if (color_target)
            {
                if (TextureFormatInfo::isUsableAsRenderTargetColor(ret))
                    return ret;
            }
            else if (depth_target)
            {
                if (TextureFormatInfo::isUsableAsRenderTargetDepth(ret))
                    return ret;
            }
            else
            {
                return ret;
            }
        }
    }

    return cTextureFormat_Invalid;
}

GX2SurfaceFormat TextureDataUtil::convFormatAGLToGX2(TextureFormat format)
{
    return s_surface_format[format];
}

void TextureDataUtil::calcSizeAndAlignment(rio::NativeSurface2D* p_surface)
{
#if RIO_IS_CAFE

    GX2CalcSurfaceSizeAndAlignment(p_surface);

#elif RIO_IS_DESKTOP

    GX2Surface surface;
    rio::MemUtil::set(&surface, 0, sizeof(GX2Surface));

    surface.dim = GX2_SURFACE_DIM_2D;
    surface.width = p_surface->width;
    surface.height = p_surface->height;
    surface.depth = 1;
    surface.numMips = p_surface->mipLevels;
    surface.format = GX2SurfaceFormat(p_surface->format);
    surface.aa = GX2_AA_MODE_1X;
    surface.use = GX2_SURFACE_USE_TEXTURE;
    surface.tileMode = GX2_TILE_MODE_LINEAR_SPECIAL;

    GX2CalcSurfaceSizeAndAlignment(&surface);

    p_surface->imageSize = surface.imageSize;
    p_surface->mipmapSize = surface.mipSize;
    p_surface->mipLevelOffset[0] = 0;
    rio::MemUtil::copy(&p_surface->mipLevelOffset[1], &surface.mipOffset[1], sizeof(u32) * (13 - 1));

#endif
}

void TextureDataUtil::initializeFromSurface(TextureData* p_texture_data, const GX2Surface& surface)
{
    p_texture_data->initializeFromSurface(surface);
}

} }
