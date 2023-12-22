#pragma once

#include <common/aglShaderProgramArchive.h>
#include <container/PtrArray.h>
#include <container/SafeArray.h>

#if RIO_IS_WIN
#include <gpu/rio_UniformBlock.h>
#endif // RIO_IS_WIN

class SharcArchiveRes;

namespace agl { namespace detail {

class ShaderHolder
{
public:
    static bool createSingleton();
    static void destroySingleton();
    static ShaderHolder* instance() { return sInstance; }

private:
    static ShaderHolder* sInstance;

    ShaderHolder();
    ~ShaderHolder();

    ShaderHolder(const ShaderHolder&);
    ShaderHolder& operator=(const ShaderHolder&);

public:
    enum ShaderType
    {
        cShader_DevUtil = 0,                        // Archive = agl_common
        cShader_DepthVisualize,                     // Archive = agl_common
        cShader_Texture,                            // Archive = agl_common
        cShader_Reduce2,                            // Archive = agl_common
        cShader_Reduce4,                            // Archive = agl_common
        cShader_Reduce8,                            // Archive = agl_common
        cShader_Reduce16,                           // Archive = agl_common
        cShader_ColorCorrection,                    // Archive = agl_technique
        cShader_ColorCorrectionMap,                 // Archive = agl_technique
        cShader_XluSnap,                            // Archive = agl_common
        cShader_ImageFilterGaussian,                // Archive = agl_common
        cShader_ImageFilterCubic,                   // Archive = agl_common
        cShader_ImageFilterBlur,                    // Archive = agl_common
        cShader_TextureMultColor,                   // Archive = agl_common
        cShader_Red,                                // Archive = agl_common
        cShader_Green,                              // Archive = agl_common
        cShader_Blue,                               // Archive = agl_common
        cShader_Alpha,                              // Archive = agl_common
        cShader_DepthRaw,                           // Archive = agl_common
        cShader_DepthLinear,                        // Archive = agl_common
        cShader_Texture2DArray,                     // Archive = agl_common
        cShader_Texture3D,                          // Archive = agl_common
        cShader_TextureCubeMap,                     // Archive = agl_common
        cShader_TextureCubeMapArray,                // Archive = agl_common
        cShader_TextureTexCoord,                    // Archive = agl_common
        cShader_TextureMultiSample1x,               // Archive = agl_common
        cShader_TextureMultiSample2x,               // Archive = agl_common
        cShader_TextureMultiSample4x,               // Archive = agl_common
        cShader_TextureMultiSample8x,               // Archive = agl_common
        cShader_AlphaModify,                        // Archive = agl_common
        cShader_TextureColorDrift,                  // Archive = agl_common
        cShader_ColorQuad,                          // Archive = agl_common
        cShader_TopBottomColor,                     // Archive = agl_common
        cShader_DrawImm,                            // Archive = agl_common
        cShader_DrawFan,                            // Archive = agl_common
        cShader_FrameBufferFlipY,                   // Archive = agl_common
        cShader_FrameBufferNoFlip,                  // Archive = agl_common
        cShader_BloomMask,                          // Archive = agl_technique
        cShader_BloomGaussian,                      // Archive = agl_technique
        cShader_BloomCompose,                       // Archive = agl_technique
        cShader_HDRCompose,                         // Archive = agl_technique
        cShader_DOFNearMask,                        // Archive = agl_technique
        cShader_DOFMipmap,                          // Archive = agl_technique
        cShader_DOFDepthMask,                       // Archive = agl_technique
        cShader_DOFFinal,                           // Archive = agl_technique
        cShader_DOFVignetting,                      // Archive = agl_technique
        cShader_FXAA,                               // Archive = agl_technique
        cShader_FXAAColorOnly,                      // Archive = agl_technique
        cShader_FXAALuma,                           // Archive = agl_technique
        cShader_FXAAReprojection,                   // Archive = agl_technique
        cShader_FilterAAReprojection,               // Archive = agl_technique
        cShader_ReduceAA,                           // Archive = agl_technique
        cShader_Lightmap,                           // Archive = agl_technique
        cShader_LightmapClear,                      // Archive = agl_technique <-- Not in Nintendo Land
        cShader_LightPrePassPointLight,             // Archive = agl_technique
        cShader_LightPrePassSpotLight,              // Archive = agl_technique
        cShader_LightPrePassProjLight,              // Archive = agl_technique
        cShader_LightPrePassDoubleSamePosWithBack,  // Archive = agl_technique
        cShader_LightPrePassTBR,                    // Archive = agl_technique
        cShader_LightPrePassTBRSp,                  // Archive = agl_technique
        cShader_LightPrePassQuad,                   // Archive = agl_technique
        cShader_LightPrePassDev,                    // Archive = agl_technique
        cShader_MultiFilterReduce,                  // Archive = agl_technique
        cShader_MultiFilterExpand,                  // Archive = agl_technique
        cShader_StaticDepthShadow,                  // Archive = agl_technique
        cShader_CubeMapGaussian,                    // Archive = agl_common
        cShader_CubeMapColorCorrection,             // Archive = agl_common
        cShader_CubeMapHeadExtract,                 // Archive = agl_common
        cShader_CubeMapConvoluteFirst,              // Archive = agl_common
        cShader_CubeMapConvoluteOther,              // Archive = agl_common
        cShader_CubeMapHDRCompose,                  // Archive = agl_common
        cShader_CubeMapDrawIlluminant,              // Archive = agl_common
        cShader_NV12Decode,                         // Archive = agl_common
        cShader_ScreenPick,                         // Archive = agl_technique
        cShader_DebugCubeMap,                       // Archive = agl_common
        cShader_DepthShadowDebug,                   // Archive = agl_technique
        cShader_VSM,                                // Archive = agl_technique
        cShader_SSAOBuffer,                         // Archive = agl_technique
        cShader_SSAOBlur,                           // Archive = agl_technique
        cShader_SSAOReduce,                         // Archive = agl_technique
        cShader_SSAOMask,                           // Archive = agl_technique
        cShader_OcclusionQuery,                     // Archive = agl_technique
        cShader_OcclusionRendererClearBuf,          // Archive = agl_technique
        cShader_OcclusionRenderer,                  // Archive = agl_technique
        cShader_OccludedEffectLensFlare,            // Archive = agl_technique
        cShader_TextureCompress,                    // Archive = agl_common
        cShader_ShadowMask,                         // Archive = agl_technique
        cShader_RadialBlur,                         // Archive = agl_technique
        cShader_RadialBlurCompose,                  // Archive = agl_technique
        cShader_DebugPrimitive,                     // Archive = agl_common
        cShader_DebugShapeInstanced,                // Archive = agl_common
        cShader_DebugPointInstanced,                // Archive = agl_common
        cShader_DebugLineInstanced,                 // Archive = agl_common
        cShader_DebugTriangleInstanced,             // Archive = agl_common
        cShader_Num
    };
    static_assert(cShader_Num == 94);

    enum ArchiveType
    {
        cArchiveType_Common = 0,
        cArchiveType_Technique,
        cArchiveType_SimpleModel,
        cArchiveType_Num
    };
    static_assert(cArchiveType_Num == 3);

public:
#if RIO_IS_CAFE
    void setCreateDisplayLists(bool create)
    {
        mCreateDisplayLists = create;
    }
#endif // RIO_IS_CAFE

    void initialize(SharcArchiveRes* p_arc);

    ShaderProgram& getShader(ShaderType shader)
    {
        RIO_ASSERT(mProgram[ shader ] != nullptr);
        return *mProgram[shader];
    }

    ShaderProgramArchive& getArchive(ArchiveType archive)
    {
        return mProgramArchive[archive];
    }

private:
    FixedPtrArray<ShaderProgram, cShader_Num> mProgram;
    UnsafeArray<ShaderProgramArchive, cArchiveType_Num> mProgramArchive;
#if RIO_IS_CAFE
    bool mCreateDisplayLists;
#elif RIO_IS_WIN
public:
    class Cfile
    {
    public:
        static constexpr u32 cUniformElemCount = 256;
        static constexpr u32 cUniformElemSize = 4 * 4;
        static constexpr u32 cUniformSize = cUniformElemCount * cUniformElemSize;

    public:
        Cfile(rio::UniformBlock::ShaderStage stage)
            : mUniformBlock(stage)
        {
            mUniformBlock.setData(nullptr, cUniformSize);
        }

        void setSubData(const void* data, u32 offset, u32 size)
        {
            mUniformBlock.setSubData(data, offset, size);
        }

        void bind(s32 index) const
        {
            mUniformBlock.setIndex(index);
            mUniformBlock.bind();
        }

    private:
        mutable rio::UniformBlock mUniformBlock;
    };

    Cfile mVsCfile;
    Cfile mPsCfile;
#endif
};
//static_assert(sizeof(ShaderHolder) == 0x280, "agl::detail::ShaderHolder size mismatch");

} }
