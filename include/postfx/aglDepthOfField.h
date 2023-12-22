#pragma once

#include <common/aglIndexStream.h>
#include <common/aglRenderBuffer.h>
#include <common/aglRenderTarget.h>
#include <common/aglShaderEnum.h>
#include <common/aglTextureSampler.h>
#include <common/aglVertexAttribute.h>
#include <common/aglVertexBuffer.h>
#include <container/Buffer.h>
#include <utility/aglParameter.h>
#include <utility/aglParameterIO.h>
#include <utility/aglParameterObj.h>

namespace agl {

class RenderBuffer;
class ShaderProgram;
class TextureData;

namespace pfx {

class DepthOfField : public utl::IParameterIO
{
    struct Context
    {
        bool mIsInitialized;
        s32 mIndex;

        // In
        TextureSampler      mColorTargetTextureSampler;
        TextureSampler      mDepthTargetTextureSampler;

        // Render utility
        RenderBuffer        mRenderBuffer;
        RenderTargetColor   mColorTarget;

        // Out
        TextureData*        mpColorTextureData;     // for "blur_mipmap"
        TextureSampler      mColorTextureSampler;
        TextureData*        mpDepthTextureData;     // for "depth_mipmap"
        TextureSampler      mDepthTextureSampler;
    };
  //static_assert(sizeof(Context) == 0x834, "agl::pfx::DepthOfField::Context size mismatch");

    struct DrawArg
    {
        DrawArg(Context& ctx, const RenderBuffer& render_buffer, const TextureData& depth, bool view_depth_, f32 near_, f32 far_);

        s32 pass;
        Context* p_ctx;
        const RenderBuffer* p_render_buffer;
        f32 near;
        f32 far;
        s32 width;
        s32 height;
        bool view_depth;
    };
    static_assert(sizeof(DrawArg) == 0x20, "agl::pfx::DepthOfField::DrawArg size mismatch");

    struct TempVignetting : utl::IParameterObj
    {
        TempVignetting(DepthOfField* p_dof, const char* param_name);

        utl::Parameter<s32> mType;
        utl::Parameter<rio::Vector2f> mRange;
        utl::Parameter<rio::Vector2f> mScale;
        utl::Parameter<rio::Vector2f> mTrans;
    };
  //static_assert(sizeof(TempVignetting) == 0xCC, "agl::pfx::DepthOfField::TempVignetting size mismatch");

    struct Vertex
    {
        rio::Vector2f position;
        rio::Vector2f texcoord;
    };
    static_assert(sizeof(Vertex) == 0x10);

    struct VignettingShape
    {
        VignettingShape() { }
        ~VignettingShape() { }

        Buffer<Vertex> mVertex;
        Buffer<u16> mIndex;
        VertexBuffer mVertexBuffer;
        VertexAttribute mVertexAttribute;
        IndexStream mIndexStream;
    };
  //static_assert(sizeof(VignettingShape) == 0x2F0, "agl::pfx::DepthOfField::VignettingShape size mismatch");

public:
    enum Uniform
    {
        cUniform_Param0 = 0,
        cUniform_IndirectTexParam,
        cUniform_IndirectTexMtx0,
        cUniform_IndirectTexMtx1,
        cUniform_MulParam,
        cUniform_AddParam,
        cUniform_NearFarParam,
        cUniform_TexParam,
        cUniform_VignettingRadius,
        cUniform_VignettingParam,
        cUniform_VignettingTrans,
        cUniform_VignettingColor,
        cUniform_FarMulColor,
        cUniform_Num
    };
    static_assert(cUniform_Num == 13);

    enum Sampler
    {
        cSampler_TexColor = 0,
        cSampler_TexDepth,
        cSampler_TexMipMap,
        cSampler_TexMipMapDepth,
        cSampler_TexIndirect,
        cSampler_Num
    };
    static_assert(cSampler_Num == 5);

    enum VignettingBlendType
    {
        cVignettingBlendType_Normal = 0,    // op = Add,    src = SrcAlpha,     dst = InvSrcAlpha
        cVignettingBlendType_Add,           // op = Add,    src = SrcAlpha,     dst = One
        cVignettingBlendType_Mult,          // op = Add,    src = Zero,         dst = SrcColor
        cVignettingBlendType_Screen         // op = Add,    src = InvDstColor,  dst = One
    };

public:
    DepthOfField();
    virtual ~DepthOfField();

    DepthOfField(const DepthOfField&) = delete;
    DepthOfField(DepthOfField&&) = delete;
    DepthOfField& operator=(const DepthOfField&) = delete;
    DepthOfField& operator=(DepthOfField&&) = delete;

    void initialize(s32 ctx_num = 1);

private:
    void assignShaderProgram_();

    void initVertex_();
    void initIndex_();

public:
    bool isEnable() const
    {
        return *mEnable;
    }

    void setEnable(bool enable)
    {
        *mEnable = enable;
    }

    VignettingBlendType getVignettingBlendType() const { return VignettingBlendType(*mVignettingBlend); }

    ShaderMode draw(s32 ctx_index, const RenderBuffer& render_buffer, f32 near, f32 far, ShaderMode mode = cShaderMode_Invalid) const;
    ShaderMode draw(s32 ctx_index, const RenderBuffer& render_buffer, const TextureData& depth, bool view_depth, f32 near, f32 far, ShaderMode mode = cShaderMode_Invalid) const;

    void allocBuffer(s32 ctx_index, const RenderBuffer& render_buffer) const;
    void allocBuffer(s32 ctx_index, TextureFormat format, s32 width, s32 height) const;
    void freeBuffer(s32 ctx_index) const;

private:
    bool enableDepthOfField_() const;
    bool enableBlurMipMapPass_() const;
    bool enableDepthBlur_() const;
    bool enableDifferntShape_() const;
    bool enableSeparateVignettingPass_() const;

    void bindRenderBuffer_(RenderBuffer& render_buffer, s32 mip_level_color, s32 mip_level_depth) const;

    void drawKick_(const DrawArg& arg) const;

    ShaderMode drawColorMipMap_(const DrawArg& arg, ShaderMode mode) const;
    ShaderMode drawDepthMipMap_(const DrawArg& arg, ShaderMode mode) const;
    ShaderMode drawCompose_(const DrawArg& arg, ShaderMode mode) const;
    ShaderMode drawVignetting_(const DrawArg& arg, ShaderMode mode) const;

    void uniformComposeParam_(const DrawArg& arg, const ShaderProgram* program) const;
    void uniformVignettingParam_(const DrawArg& arg, const ShaderProgram* program) const;

    static s32 roundUp_(f32 value)
    {
        s32 ivalue = s32(value);
        if (ivalue != value)
            if (value >= 0.0f)
                ivalue++;

        return ivalue;
    }

    static s32 roundDown_(f32 value)
    {
        s32 ivalue = s32(value);
        if (ivalue != value)
            if (value < 0.0f)
                ivalue--;

        return ivalue;
    }

    rio::Vector4f getTexParam_(const TextureData& data, u32 mip_level = 0) const
    {
        return rio::Vector4f {
            0.5f / data.getWidth(mip_level),
            0.5f / data.getHeight(mip_level),
            0.0f, // Unused
            0.0f  // ^^
        };
    }

public:
    void setIndirectTextureData(const TextureData* p_texture_data);
    void setIndirectEnable(bool enable);

    void setIndirectTextureTrans(const rio::Vector2f& trans);

private:
    void applyIndirectTextureData_();
    void updateIndirectMatrix_();

    void postRead_() override;

private:
    mutable Buffer<Context> mContext;
    s32 _1e8;
    f32 _1ec;
    bool mEnableColorBlur;
    f32 _1f4;
  //sead::GraphicsContextMRT mGraphicsContext;
    utl::IParameterObj mParameterObj;
    utl::Parameter<bool> mEnable;
    utl::Parameter<bool> mNearEnable;
    utl::Parameter<bool> mFarEnable;
    utl::Parameter<bool> mDepthBlur;
    utl::Parameter<bool> mEnableVignettingColor;
    utl::Parameter<bool> mEnableVignettingBlur;
    utl::Parameter<bool> mEnableVignetting2Shape;
    utl::Parameter<bool> mEnableColorControl;
    utl::Parameter<bool> mIndirectEnable;
    utl::Parameter<f32> mStart;
    utl::Parameter<f32> mEnd;
    utl::Parameter<f32> mFarStart;
    utl::Parameter<f32> mFarEnd;
    utl::Parameter<f32> mLevel;
    utl::Parameter<f32> mDepthBlurAdd;
    utl::Parameter<f32> mSaturateMin;
    utl::Parameter<rio::Vector4f> mColorCtrlDepth;
    utl::Parameter<rio::Vector2f> mIndirectTexTrans;
    utl::Parameter<rio::Vector2f> mIndirectTexScale;
    utl::Parameter<f32> mIndirectTexRotate;
    utl::Parameter<f32> mIndirectScale;
    utl::Parameter<f32> mVignettingBlur;
    utl::Parameter<s32> mVignettingBlend; // -> enum VignettingBlendType
    utl::Parameter<rio::Color4f> mVignettingColor;
    TempVignetting mTempVignetting0;
    TempVignetting mTempVignetting1;
    utl::Parameter<rio::Color4f> mFarMulColor;
  //utl::DebugTexturePage mDebugTexturePage;
  //sead::SafeArray<ShaderProgram*, 2> mpCurrentProgramNearMask;
    SafeArray<ShaderProgram*, 2> mpCurrentProgramMipMap;
    SafeArray<const ShaderProgram*, 2> mpCurrentProgramDepthMask;
    SafeArray<ShaderProgram*, 2> mpCurrentProgramFinal;
    ShaderProgram* mpCurrentProgramVignetting;
    const TextureData* mpIndirectTextureData;
    TextureSampler mIndirectTextureSampler;
    mutable rio::Vector4f mIndirectTexParam;
    rio::Vector3f mIndirectTexMtx0;
    rio::Vector3f mIndirectTexMtx1;
    SafeArray<VignettingShape, 2> mVignettingShape;
  //u8 _10e8;
};
//static_assert(sizeof(DepthOfField) == 0x10EC, "agl::pfx::DepthOfField size mismatch");

} }
