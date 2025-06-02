#pragma once

#include <common/aglTextureEnum.h>
#include <gpu/rio_Texture.h>

#include <cafe/gx2/gx2Enum.h>

namespace agl {

class TextureFormatInfo
{
public:
    static u8 getPixelByteSize(TextureFormat format);
    static u8 getComponentNum(TextureFormat format);
    static u8 getComponentBitSize(TextureFormat format, s32 component);
    static u8 getComponentOrder(TextureFormat format, s32 component);
    static bool isCompressed(TextureFormat format);
    static bool isNormalized(TextureFormat format);
    static bool isFloat(TextureFormat format);
    static bool isUnsigned(TextureFormat format);
    static bool isSRGB(TextureFormat format);
    static bool isUsableAsRenderTargetColor(TextureFormat format);
    static bool isUsableAsRenderTargetDepth(TextureFormat format);
    static const char* getString(TextureFormat format);
    static TextureCompSel getDefaultCompSel(TextureFormat format, s32 component);
    static TextureFormat convFormatGX2ToAGL(GX2SurfaceFormat format, bool color_target, bool depth_target);
    static GX2SurfaceFormat convFormatAGLToGX2(TextureFormat format);

#if RIO_IS_DESKTOP
    static bool setNativeTextureFormat(rio::NativeTextureFormat* p_native_format, TextureFormat format);
#endif // RIO_IS_DESKTOP
};

}
