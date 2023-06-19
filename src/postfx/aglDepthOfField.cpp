#include <common/aglShaderProgram.h>
#include <detail/aglShaderHolder.h>
#include <gpu/rio_RenderState.h>
#include <math/rio_Matrix.h>
#include <postfx/aglDepthOfField.h>
#include <utility/aglDynamicTextureAllocator.h>
#include <utility/aglImageFilter2D.h>
#include <utility/aglPrimitiveShape.h>
#include <utility/aglVertexAttributeHolder.h>

namespace agl { namespace pfx {

DepthOfField::DepthOfField()
    : utl::IParameterIO("agldof")
    , mContext()
    , _1e8(0)
    , _1ec(1.0f)
    , mEnableColorBlur(true)
    , _1f4(1.0f)
  //, mGraphicsContext()
    , mParameterObj()
    , mEnable                   (true,                                              "enable",                    "有効",              &mParameterObj)
    , mNearEnable               (false,                                             "near_enable",               "近景ぼけ有効",       &mParameterObj)
    , mFarEnable                (true,                                              "far_enable",                "遠景ぼけ有効",       &mParameterObj)
    , mDepthBlur                (false,                                             "depth_blur",                "近景ぼけの深度ぼかし", &mParameterObj)
    , mEnableVignettingColor    (false,                                             "enable_vignetting_color",   "周辺減光有効",      &mParameterObj)
    , mEnableVignettingBlur     (false,                                             "enable_vignetting_blur",    "周辺ぼけ有効",       &mParameterObj)
    , mEnableVignetting2Shape   (false,                                             "enable_vignetting_2_shape", "形状設定をわける",    &mParameterObj)
    , mEnableColorControl       (false,                                             "enable_color_control",      "深度による色調整",    &mParameterObj)
    , mIndirectEnable           (false,                                             "indirect_enable",           "インダイレクト処理有効", &mParameterObj)
    , mStart                    (100.0f,                                            "start",                     "ぼけ開始深度",       &mParameterObj)
    , mEnd                      (200.0f,                                            "end",                       "ぼけ終了深度",       &mParameterObj)
    , mFarStart                 (120.0f,                                            "far_start",                 "近景ぼけ開始深度",   &mParameterObj)
    , mFarEnd                   (0.0f,                                              "far_end",                   "近景ぼけ終了深度",   &mParameterObj)
    , mLevel                    (2.0f,                                              "level",                     "ぼかしレベル",         &mParameterObj)
    , mDepthBlurAdd             (0.0f,                                              "depth_blur_add",            "深度ぼかし加算",      &mParameterObj)
    , mSaturateMin              (1.0f,                                              "saturate_min",              "彩度",              &mParameterObj)
    , mColorCtrlDepth           (rio::Vector4f{1000.0f, 2000.0f, 3000.0f, 4000.0f}, "color_ctrl_depth",          "色調整距離",        &mParameterObj)
    , mIndirectTexTrans         (rio::Vector2f{0.0f, 0.0f},                         "indirect_tex_trans",        "テクスチャ行列Trans",  &mParameterObj)
    , mIndirectTexScale         (rio::Vector2f{1.0f, 1.0f},                         "indirect_tex_scale",        "テクスチャ行列Scale",  &mParameterObj)
    , mIndirectTexRotate        (0.0f,                                              "indirect_tex_rotate",       "テクスチャ行列Rotate", &mParameterObj)
    , mIndirectScale            (0.2f,                                              "indirect_scale",            "ゆがみ幅",           &mParameterObj)
  //, mVignettingBlur           (1.0f,                                              "vignetting_blur",           "周辺ぼけ強さ",        &mParameterObj)
    , mVignettingBlend          (0,                                                 "vignetting_blend",          "ブレンド",            &mParameterObj)
    , mVignettingColor          (rio::Color4f{0.0f, 0.0f, 0.0f, 0.75f},             "vignetting_color",          "周辺減光色",        &mParameterObj)
    , mTempVignetting0(this, "vignetting_shape_0")
    , mTempVignetting1(this, "vignetting_shape_1")
    , mFarMulColor              (rio::Color4f{1.0f, 1.0f, 1.0f, 1.0f},              "far_mul_color",             "乗算色",            &mParameterObj)
  //, mDebugTexturePage()
    , mpIndirectTextureData(nullptr)
    , mIndirectTextureSampler()
    , mIndirectTexParam{0.0f, 0.0f, 0.0f, 0.0f}
    , mIndirectTexMtx0{1.0f, 0.0f, 0.0f}
    , mIndirectTexMtx1{0.0f, 1.0f, 0.0f}
  //, _10e8(1)
{
    addObj(&mParameterObj, "dof");

    // 被写界深度

  //detail::RootNode::setNodeMeta(this, "Icon=SNAPSHOT");
}

DepthOfField::~DepthOfField()
{
    for (s32 i = 0; i < mContext.size(); i++)
        freeBuffer(i);

    mContext.freeBuffer();
}

void DepthOfField::initialize(s32 ctx_num)
{
    mContext.allocBuffer(ctx_num);

    assignShaderProgram_();
    initVertex_();
    initIndex_();

    mIndirectTextureSampler.setWrap(
        cTextureWrapType_Repeat,
        cTextureWrapType_Repeat,
        cTextureWrapType_Repeat
    );

    for (Buffer<Context>::iterator itr_ctx = mContext.begin(), itr_ctx_end = mContext.end(); itr_ctx != itr_ctx_end; ++itr_ctx)
    {
        itr_ctx->mIsInitialized = true;
        itr_ctx->mIndex = itr_ctx.getIndex();

        itr_ctx->mColorTargetTextureSampler.setFilter(
            cTextureFilterType_Linear,
            cTextureFilterType_Linear,
            cTextureMipFilterType_None
        );
        itr_ctx->mColorTargetTextureSampler.setUnk1(1);

        itr_ctx->mDepthTargetTextureSampler.setFilter(
            cTextureFilterType_Linear,
            cTextureFilterType_Linear,
            cTextureMipFilterType_None
        );
        itr_ctx->mDepthTargetTextureSampler.setUnk1(1);

        itr_ctx->mpColorTextureData = nullptr;

        itr_ctx->mColorTextureSampler.setFilter(
            cTextureFilterType_Linear,
            cTextureFilterType_Linear,
            cTextureMipFilterType_Point
        );

        itr_ctx->mpDepthTextureData = nullptr;

        itr_ctx->mDepthTextureSampler.setFilter(
            cTextureFilterType_Linear,
            cTextureFilterType_Linear,
            cTextureMipFilterType_Point
        );
    }

  //mDebugTexturePage.setUp(ctx_num, "DepthOfField");
}

#define MACRO_DOF_MIPMAP_COLOR_MIP 0
#define MACRO_DOF_MIPMAP_DEPTH_MIP 1

#define MACRO_DOF_DEPTH_MASK_DOF_FAR    0
#define MACRO_DOF_DEPTH_MASK_DOF_NEAR   1
#define MACRO_DOF_DEPTH_MASK_VIGNETTING 2
#define MACRO_DOF_DEPTH_MASK_VIEW_DEPTH 3

#define MACRO_DOF_FINAL_DOF_FAR     0
#define MACRO_DOF_FINAL_DOF_NEAR    1
#define MACRO_DOF_FINAL_VIGNETTING  2
#define MACRO_DOF_FINAL_VIEW_DEPTH  3

#define MACRO_DOF_VIGNETTING_VIGNETTING         0
#define MACRO_DOF_VIGNETTING_VIGNETTING_BLEND   1

static inline ShaderProgram* GetVariation(ShaderProgram& program, s32 macro_index, s32 value_index)
{
    /*  Getter of variation where macro at macro_index is set to value at value_index,
        and all other macros are set to values at index 0 respectively.                 */

    return program.getVariation(program.getVariationMacroValueVariationNum(macro_index) * value_index);
}

void DepthOfField::assignShaderProgram_()
{
    // MipMap
    {
        ShaderProgram& program_mipmap = detail::ShaderHolder::instance()->getShader(detail::ShaderHolder::cShader_DOFMipmap);

        mpCurrentProgramMipMap[0] = GetVariation(program_mipmap, MACRO_DOF_MIPMAP_COLOR_MIP, mEnableColorBlur ? 2 : 1);
        mpCurrentProgramMipMap[1] = GetVariation(program_mipmap, MACRO_DOF_MIPMAP_DEPTH_MIP, 1);
    }
    // DepthMask + Final
    {
        bool enable_depth_blur = enableDepthBlur_();
        s32 dof_near_idx =
            *mNearEnable
                ? (enable_depth_blur ? 2 : 1)
                : 0;

        s32 vignetting_idx = 0;
        if (*mEnableVignettingBlur)
        {
            vignetting_idx = 1;
            if (*mEnableVignettingColor && !enableSeparateVignettingPass_())
            {
                bool color_is_black = mVignettingColor->r == 0.0f &&
                                      mVignettingColor->g == 0.0f &&
                                      mVignettingColor->b == 0.0f;

                s32 idx = 2;
                if  (color_is_black)
                    idx = 1;

                vignetting_idx = idx;
                vignetting_idx++;
            }
        }

        s32 dof_far_idx = 0;
        if (*mFarEnable)
        {
            dof_far_idx = 1;
            if (*mEnableColorControl)
            {
                if (*mSaturateMin != 1.0f)
                    dof_far_idx = 2;

                if (    mFarMulColor->r != 1.0f ||
                        mFarMulColor->g != 1.0f ||
                        mFarMulColor->b != 1.0f     )
                    dof_far_idx += 2;
            }

            if (mpIndirectTextureData && *mIndirectEnable)
                dof_far_idx += 4;
        }

        const ShaderProgram& program_depth_mask = detail::ShaderHolder::instance()->getShader(detail::ShaderHolder::cShader_DOFDepthMask);

        s32 depth_mask_variation_idx = program_depth_mask.getVariationMacroValueVariationNum(MACRO_DOF_DEPTH_MASK_DOF_FAR)      * s32(dof_far_idx != 0) +
                                       program_depth_mask.getVariationMacroValueVariationNum(MACRO_DOF_DEPTH_MASK_DOF_NEAR)     * dof_near_idx +
                                       program_depth_mask.getVariationMacroValueVariationNum(MACRO_DOF_DEPTH_MASK_VIGNETTING)   * s32(*mEnableVignettingBlur);

        mpCurrentProgramDepthMask[0] = program_depth_mask.getVariation(depth_mask_variation_idx);
        mpCurrentProgramDepthMask[1] = program_depth_mask.getVariation(depth_mask_variation_idx | 1);

        ShaderProgram& program_final = detail::ShaderHolder::instance()->getShader(detail::ShaderHolder::cShader_DOFFinal);

        s32 final_variation_idx = program_final.getVariationMacroValueVariationNum(MACRO_DOF_FINAL_DOF_FAR)     * dof_far_idx +
                                  program_final.getVariationMacroValueVariationNum(MACRO_DOF_FINAL_DOF_NEAR)    * dof_near_idx +
                                  program_final.getVariationMacroValueVariationNum(MACRO_DOF_FINAL_VIGNETTING)  * vignetting_idx;

        mpCurrentProgramFinal[0] = program_final.getVariation(final_variation_idx);
        mpCurrentProgramFinal[1] = program_final.getVariation(final_variation_idx | 1);
    }
    // Vignetting
    {
        ShaderProgram& program_vignetting = detail::ShaderHolder::instance()->getShader(detail::ShaderHolder::cShader_DOFVignetting);

        VignettingBlendType vignetting_blend = getVignettingBlendType();

        bool vignetting_color_is_black = mVignettingColor->r == 0.0f &&
                                         mVignettingColor->g == 0.0f &&
                                         mVignettingColor->b == 0.0f;

        s32 vignetting_blend_idx;
        switch (vignetting_blend)
        {
        default:                            vignetting_blend_idx = 0; break;
        case cVignettingBlendType_Mult:     vignetting_blend_idx = 1; break;
        case cVignettingBlendType_Screen:   vignetting_blend_idx = 2; break;
        }

        s32 vignetting_variation_idx = program_vignetting.getVariationMacroValueVariationNum(MACRO_DOF_VIGNETTING_VIGNETTING)       * s32(!vignetting_color_is_black) +
                                       program_vignetting.getVariationMacroValueVariationNum(MACRO_DOF_VIGNETTING_VIGNETTING_BLEND) * vignetting_blend_idx;

        mpCurrentProgramVignetting = program_vignetting.getVariation(vignetting_variation_idx);
    }
}

void DepthOfField::initVertex_()
{
    static const UnsafeArray<s32, 2> sShapeNum = { 32, 4 };

    const UnsafeArray<f32, 4> cTexcoordX = { 1.0f, 1.0f, 0.0f, 0.0f };

    for (u32 i_shape = 0; i_shape < 2; i_shape++)
    {
        VignettingShape* p_shape = &(mVignettingShape[i_shape]);

        s32 num = sShapeNum[i_shape];

        p_shape->mVertex.allocBuffer(4 * num); // They forgot to align the data lol

        s32 vtx_index = 0;

        for (u32 j = 0; j < 4; j++)
        {
            switch(i_shape)
            {
            case 0: // Circle
                for (s32 k = 0; k < num; k++)
                {
                    f32 rad = k * rio::Mathf::pi2() / num;

                    p_shape->mVertex[vtx_index].position.set(
                        std::cos(rad),
                        std::sin(rad)
                    );

                    p_shape->mVertex[vtx_index].texcoord.set(
                        cTexcoordX[j],
                        s32(j)
                    );

                    vtx_index++;
                }
                break;
            case 1: // Quad
                {
                    p_shape->mVertex[vtx_index].position.set(
                        -1.0f,
                         1.0f
                    );

                    p_shape->mVertex[vtx_index].texcoord.set(
                        cTexcoordX[j],
                        s32(j)
                    );

                    vtx_index++;

                    p_shape->mVertex[vtx_index].position.set(
                        -1.0f,
                        -1.0f
                    );

                    p_shape->mVertex[vtx_index].texcoord.set(
                        cTexcoordX[j],
                        s32(j)
                    );

                    vtx_index++;

                    p_shape->mVertex[vtx_index].position.set(
                         1.0f,
                        -1.0f
                    );

                    p_shape->mVertex[vtx_index].texcoord.set(
                        cTexcoordX[j],
                        s32(j)
                    );

                    vtx_index++;

                    p_shape->mVertex[vtx_index].position.set(
                         1.0f,
                         1.0f
                    );

                    p_shape->mVertex[vtx_index].texcoord.set(
                        cTexcoordX[j],
                        s32(j)
                    );

                    vtx_index++;
                }
                break;
            }
        }

        RIO_ASSERT(vtx_index == p_shape->mVertex.size());

        p_shape->mVertexBuffer.setUpBuffer(
            p_shape->mVertex.getBufferPtr(),
            sizeof(Vertex),
            p_shape->mVertex.size() * sizeof(Vertex)
        );

        p_shape->mVertexBuffer.setUpStream(0, cVertexStreamFormat_32_32_float, offsetof(Vertex, position));
        p_shape->mVertexBuffer.setUpStream(1, cVertexStreamFormat_32_32_float, offsetof(Vertex, texcoord));

        p_shape->mVertexAttribute.create(1);

        p_shape->mVertexAttribute.setVertexStream(0, &p_shape->mVertexBuffer, 0);
        p_shape->mVertexAttribute.setVertexStream(1, &p_shape->mVertexBuffer, 1);

        p_shape->mVertexAttribute.setUp();
    }
}

void DepthOfField::initIndex_()
{
    static const UnsafeArray<s32, 2> sShapeNum = { 32, 4 };

    for (u32 i_shape = 0; i_shape < 2; i_shape++)
    {
        VignettingShape* p_shape = &(mVignettingShape[i_shape]);

        s32 num = sShapeNum[i_shape];

        p_shape->mIndex.allocBuffer(3 * num * 6); // They forgot to align the data lol

        s32 idx_index = 0;

        for (u32 j = 0; j < 3; j++)
        {
            for (s32 k = 0; k < num; k++)
            {
                p_shape->mIndex[idx_index++] = j * num + k;
                p_shape->mIndex[idx_index++] = j * num + num + ((k + 1) % num);
                p_shape->mIndex[idx_index++] = j * num + ((k + 1) % num);
                p_shape->mIndex[idx_index++] = j * num + k;
                p_shape->mIndex[idx_index++] = j * num + num + k;
                p_shape->mIndex[idx_index++] = j * num + num + ((k + 1) % num);
            }
        }

        RIO_ASSERT(idx_index == p_shape->mIndex.size());

        p_shape->mIndexStream.setUpStream(
            p_shape->mIndex.getBufferPtr(),
            p_shape->mIndex.size(),
            rio::Drawer::TRIANGLES
        );
    }
}

ShaderMode DepthOfField::draw(s32 ctx_index, const RenderBuffer& render_buffer, f32 near, f32 far, ShaderMode mode) const
{
    RIO_ASSERT(render_buffer.getRenderTargetDepth() != nullptr);
    return draw(ctx_index, render_buffer, render_buffer.getRenderTargetDepth()->getTextureData(), false, near, far, mode);
}

ShaderMode DepthOfField::draw(s32 ctx_index, const RenderBuffer& render_buffer, const TextureData& depth, bool view_depth, f32 near, f32 far, ShaderMode mode) const
{
    // Custom assert that actually matters
    [[maybe_unused]] const TextureFormat& depth_format = depth.getTextureFormat();
#if RIO_IS_CAFE
    RIO_ASSERT(depth_format == cTextureFormat_Depth_32);
#else
    RIO_ASSERT(depth_format >= cTextureFormat_Depth_16 && depth_format <= cTextureFormat_Depth_24_uNorm_Stencil_8);
#endif // RIO_IS_CAFE

    if (isEnable())
    {
        RIO_ASSERT(render_buffer.getRenderTargetColor() != nullptr);
        if (mContext[ctx_index].mIsInitialized)
        {
            DrawArg arg(mContext[ctx_index], render_buffer, depth, view_depth, near, far);
            if (enableBlurMipMapPass_())
            {
                allocBuffer(ctx_index, render_buffer);
                {
                    // Pass 0
                    {
                        arg.pass = 0;
                        mode = drawColorMipMap_(arg, mode);
                    }

                    if (enableDepthBlur_()) // Pass 1
                    {
                        arg.pass = 1;
                        mode = drawDepthMipMap_(arg, mode);
                    }

                    // Pass 2
                    {
                        arg.pass = 2;
                        mode = drawCompose_(arg, mode);
                    }
                }
                freeBuffer(ctx_index);
            }

            if (enableSeparateVignettingPass_()) // Pass 3
            {
                arg.pass = 3;
                mode = drawVignetting_(arg, mode);
            }
        }
    }

    return mode;
}

void DepthOfField::allocBuffer(s32 ctx_index, const RenderBuffer& render_buffer) const
{
    const RenderTargetColor* color = render_buffer.getRenderTargetColor();
    RIO_ASSERT(color != nullptr);

    allocBuffer(ctx_index, color->getTextureData().getTextureFormat(), color->getTextureData().getWidth(), color->getTextureData().getHeight());
}

void DepthOfField::allocBuffer(s32 ctx_index, TextureFormat format, s32 width, s32 height) const
{
    Context* p_ctx = &(mContext[ctx_index]);

    s32 color_blur_mipmap_num;
    s32 color_blur_width;
    s32 color_blur_height;

    if (*mIndirectEnable && roundDown_(*mLevel) == 0)   // enableMipFromZeroLevel_()
    {
        color_blur_mipmap_num = roundUp_(*mLevel) + 1;

        color_blur_width  = width;
        color_blur_height = height;
    }
    else
    {
        color_blur_mipmap_num = std::max<s32>(1, roundUp_(*mLevel));

        color_blur_width  = width  / 2;
        color_blur_height = height / 2;
    }

    p_ctx->mpColorTextureData = utl::DynamicTextureAllocator::alloc(
        "dof_blur_mipmap",
        format,
        color_blur_width,
        color_blur_height,
        color_blur_mipmap_num
    );
    RIO_ASSERT(p_ctx->mpColorTextureData != nullptr);
    p_ctx->mColorTextureSampler.applyTextureData(*p_ctx->mpColorTextureData);

    if (enableDepthBlur_())
    {
        p_ctx->mpDepthTextureData = utl::DynamicTextureAllocator::alloc(
            "dof_depth_mipmap",
            cTextureFormat_R8_uNorm,
            width  / 2,
            height / 2,
            roundUp_(*mDepthBlurAdd) + 1
        );
        RIO_ASSERT(p_ctx->mpDepthTextureData != nullptr);
        p_ctx->mDepthTextureSampler.applyTextureData(*p_ctx->mpDepthTextureData);
    }
}

void DepthOfField::freeBuffer(s32 ctx_index) const
{
    Context* p_ctx = &(mContext[ctx_index]);

    if (p_ctx->mpColorTextureData)
    {
        utl::DynamicTextureAllocator::free(p_ctx->mpColorTextureData);
        p_ctx->mpColorTextureData = nullptr;
    }
    if (p_ctx->mpDepthTextureData)
    {
        utl::DynamicTextureAllocator::free(p_ctx->mpDepthTextureData);
        p_ctx->mpDepthTextureData = nullptr;
    }
}

bool DepthOfField::enableDepthOfField_() const
{
    return *mNearEnable || *mFarEnable;
}

bool DepthOfField::enableBlurMipMapPass_() const
{
    return enableDepthOfField_() || *mEnableVignettingBlur;
}

bool DepthOfField::enableDepthBlur_() const
{
    return *mNearEnable && *mDepthBlur;
}

bool DepthOfField::enableDifferntShape_() const
{
    return *mEnableVignettingColor && *mEnableVignettingBlur && *mEnableVignetting2Shape;
}

bool DepthOfField::enableSeparateVignettingPass_() const
{
    // Implementing this in a single line mismatches :^)

    if (*mEnableVignettingColor)
        if (enableDifferntShape_() || getVignettingBlendType() != cVignettingBlendType_Normal || !*mEnableVignettingBlur)
            return true;

    return false;
}

void DepthOfField::bindRenderBuffer_(RenderBuffer& render_buffer, s32 mip_level_color, s32 mip_level_depth) const
{
    s32 w = -1;
    s32 h = -1;

    if (render_buffer.getRenderTargetColor() != nullptr)
    {
        u32 mip_level = mip_level_color;
        render_buffer.getRenderTargetColor()->setMipLevel(mip_level);

        w = render_buffer.getRenderTargetColor()->getTextureData().getWidth(mip_level);
        h = render_buffer.getRenderTargetColor()->getTextureData().getHeight(mip_level);
    }

    if (render_buffer.getRenderTargetDepth() != nullptr)
    {
        u32 mip_level = mip_level_color + mip_level_depth;
        render_buffer.getRenderTargetDepth()->setMipLevel(mip_level);

        s32 w_ = render_buffer.getRenderTargetDepth()->getTextureData().getWidth(mip_level);
        s32 h_ = render_buffer.getRenderTargetDepth()->getTextureData().getHeight(mip_level);

        RIO_ASSERT(w == -1 || w == w_);
        RIO_ASSERT(h == -1 || h == h_);

        w = w_;
        h = h_;
    }

    render_buffer.setSize(w, h);

    render_buffer.bind();
}

void DepthOfField::drawKick_(const DrawArg& arg) const
{
    switch (arg.pass)
    {
    case 3: // Vignetting
        {
            s32 type = enableDifferntShape_()
                ? *mTempVignetting1.mType
                : *mTempVignetting0.mType;

            mVignettingShape[type].mVertexAttribute.activate();
            mVignettingShape[type].mIndexStream.draw();
        }
        break;
    case 2: // Compose
        if (*mEnableVignettingBlur)
        {
            mVignettingShape[*mTempVignetting0.mType].mVertexAttribute.activate();
            mVignettingShape[*mTempVignetting0.mType].mIndexStream.draw();
        }
        else
        {
            utl::VertexAttributeHolder::instance()->getVertexAttribute(utl::VertexAttributeHolder::cAttribute_QuadTriangle).activate();
            utl::PrimitiveShape::instance()->getIdxStreamQuadTriangle().draw();
        }
        break;
    default:
        {
            utl::VertexAttributeHolder::instance()->getVertexAttribute(utl::VertexAttributeHolder::cAttribute_QuadTriangle).activate();
            utl::PrimitiveShape::instance()->getIdxStreamQuadTriangle().draw();
        }
        break;
    }
}

ShaderMode DepthOfField::drawColorMipMap_(const DrawArg& arg, ShaderMode mode) const
{
    bool enable_mip_from_zero_level = *mIndirectEnable && roundDown_(*mLevel) == 0; // enableMipFromZeroLevel_()

    TextureSampler& color_sampler = arg.p_ctx->mColorTextureSampler;
    RenderBuffer& render_buffer = arg.p_ctx->mRenderBuffer;
    RenderTargetColor& color_target = arg.p_ctx->mColorTarget;

    rio::RenderState context;
    {
        context.setDepthEnable(false, false);
        context.setColorMask(true, true, true, false);
        context.setBlendEnable(false);
    }
    context.apply();

    render_buffer.setRenderTargetColorNull();
    render_buffer.setRenderTargetDepthNull();

    render_buffer.setRenderTargetColor(&color_target);
    color_target.applyTextureData(color_sampler.getTextureData(), 0);

    const ShaderProgram* p_program_mipmap = mpCurrentProgramMipMap[0];
    mode = p_program_mipmap->activate(mode);

    color_sampler.setFilter(cTextureFilterType_Linear, cTextureFilterType_Linear, cTextureMipFilterType_Point);

    u32 mip_level_num = color_sampler.getTextureData().getMipLevelNum();

    for (u32 mip_level = 0; mip_level < mip_level_num; mip_level++)
    {
        const TextureSampler* p_sampler;
        if (mip_level != 0)
        {
            color_sampler.setMipParam(mip_level - 1, mip_level - 1, 0.0f);
            p_sampler = &color_sampler;
        }
        else
        {
            if (enable_mip_from_zero_level)
            {
                bindRenderBuffer_(render_buffer, 0, 0);
                mode = utl::ImageFilter2D::drawTextureQuadTriangle(arg.p_ctx->mColorTargetTextureSampler, mode);
                arg.p_ctx->mRenderBuffer.getRenderTargetColor()->invalidateGPUCache();
                continue;
            }
            p_sampler = &arg.p_ctx->mColorTargetTextureSampler;
        }

        p_sampler->activate(p_program_mipmap->getSamplerLocation(cSampler_TexColor), cSampler_TexColor);

        const rio::Vector4f& param = getTexParam_(
            color_sampler.getTextureData(),
            mip_level
        );
        rio::Vector4f tex_param = param * _1ec;

        p_program_mipmap->getUniformLocation(cUniform_TexParam).setUniform(tex_param);

        bindRenderBuffer_(render_buffer, mip_level, 0);
        drawKick_(arg);
        arg.p_ctx->mRenderBuffer.getRenderTargetColor()->invalidateGPUCache();
    }

    color_sampler.setFilter(cTextureFilterType_Linear, cTextureFilterType_Linear, cTextureMipFilterType_Linear);
    color_sampler.setMipParam(0.0f, color_sampler.getTextureData().getMipLevelNum() - 1.0f, 0.0f);

    return mode;
}

ShaderMode DepthOfField::drawDepthMipMap_(const DrawArg& arg, ShaderMode mode) const
{
    TextureSampler& depth_sampler = arg.p_ctx->mDepthTextureSampler;
    RenderBuffer& render_buffer = arg.p_ctx->mRenderBuffer;
    RenderTargetColor& color_target = arg.p_ctx->mColorTarget;

    rio::RenderState context;
    {
        context.setDepthEnable(false, false);
        context.setColorMask(true, false, false, false);
        context.setBlendEnable(false);
    }
    context.apply();

    render_buffer.setRenderTargetColorNull();
    render_buffer.setRenderTargetDepthNull();

    render_buffer.setRenderTargetColor(&color_target);
    color_target.applyTextureData(depth_sampler.getTextureData(), 0);

    depth_sampler.setFilter(cTextureFilterType_Linear, cTextureFilterType_Linear, cTextureMipFilterType_Point);

    // NearMask
    {
        const ShaderProgram* p_program_near_mask =
            detail::ShaderHolder::instance()->getShader(detail::ShaderHolder::cShader_DOFNearMask)
                .getVariation(arg.view_depth); // VIEW_DEPTH = 1

        mode = p_program_near_mask->activate(mode);

        f32 far_end   = std::min<f32>(*mFarEnd, *mFarStart);
        f32 far_start = std::max<f32>(*mFarStart, *mFarEnd);

        if (far_start == far_end)
            far_end += std::numeric_limits<f32>::epsilon();

        f32 far_range = far_end - far_start;
        if (arg.view_depth)
        {
            f32 z = arg.far - arg.near;
            far_range = far_range               / z;
            far_start = (far_start - arg.near)  / z;
        }

        rio::Vector4f param;
        param.x = 1.0f / arg.near;
        param.y = (1.0f - arg.near / arg.far) / arg.near;
        param.z = 1.0f / far_range;
        param.w = -(far_start * param.z);

        p_program_near_mask->getUniformLocation(cUniform_NearFarParam).setUniform(param);

        arg.p_ctx->mDepthTargetTextureSampler.activate(p_program_near_mask->getSamplerLocation(cSampler_TexDepth), cSampler_TexDepth);

        param = getTexParam_(depth_sampler.getTextureData());
        rio::Vector4f tex_param = param * _1ec;

        p_program_near_mask->getUniformLocation(cUniform_TexParam).setUniform(tex_param);

        bindRenderBuffer_(render_buffer, 0, 0);
        drawKick_(arg);
        arg.p_ctx->mRenderBuffer.getRenderTargetColor()->invalidateGPUCache();
    }

    //MipMap
    {
        const ShaderProgram* p_program_mipmap = mpCurrentProgramMipMap[1];
        mode = p_program_mipmap->activate(mode);

        u32 mip_level_num = depth_sampler.getTextureData().getMipLevelNum();

        for (u32 mip_level = 1; mip_level < mip_level_num; mip_level++)
        {
            depth_sampler.setMipParam(mip_level - 1, mip_level - 1, 0.0f);

            depth_sampler.activate(p_program_mipmap->getSamplerLocation(cSampler_TexDepth), cSampler_TexDepth);

            const rio::Vector4f& param = getTexParam_(
                depth_sampler.getTextureData(),
                mip_level
            );
            rio::Vector4f tex_param = param * _1ec;

            p_program_mipmap->getUniformLocation(cUniform_TexParam).setUniform(tex_param);

            bindRenderBuffer_(render_buffer, mip_level, 0);
            drawKick_(arg);
            arg.p_ctx->mRenderBuffer.getRenderTargetColor()->invalidateGPUCache();
        }
    }

    depth_sampler.setFilter(cTextureFilterType_Linear, cTextureFilterType_Linear, cTextureMipFilterType_Point);
    depth_sampler.setMipParam(*mDepthBlurAdd, *mDepthBlurAdd, 0.0f);

    return mode;
}

static bool always_true = true; // shrug

ShaderMode DepthOfField::drawCompose_(const DrawArg& arg, ShaderMode mode) const
{
    bool depth_far  =  *mFarEnable && !*mNearEnable                 && always_true && !*mEnableVignettingBlur;
    bool depth_near = !*mFarEnable &&  *mNearEnable && !*mDepthBlur && always_true && !*mEnableVignettingBlur;

    rio::RenderState context;
    {
        context.setDepthWriteEnable(false);
        if (depth_far)
        {
            context.setDepthTestEnable(true);
            context.setDepthFunc(rio::Graphics::COMPARE_FUNC_LESS);
        }
        else if (depth_near)
        {
            context.setDepthTestEnable(true);
            context.setDepthFunc(rio::Graphics::COMPARE_FUNC_GREATER);
        }
        else
        {
            context.setDepthTestEnable(false);
        }
        context.setColorMask(true, true, true, false);
    }
    context.apply();

    arg.p_render_buffer->bind();

    const agl::ShaderProgram* program =
        _1e8 == 1
            ? mpCurrentProgramDepthMask[arg.view_depth]
            : mpCurrentProgramFinal[arg.view_depth];

    mode = program->activate(mode);

    uniformComposeParam_(arg, program);

    if (*mEnableVignettingBlur)
    {
        // TODO
      //uniformVignettingParam_(arg, program);
        RIO_LOG("Tried to set vignetting param\n");
        RIO_ASSERT(false);
    }

    drawKick_(arg);

    return mode;
}

ShaderMode DepthOfField::drawVignetting_(const DrawArg& arg, ShaderMode mode) const
{
    rio::RenderState context;
    {
        context.setDepthEnable(false, false);
        context.setColorMask(true, true, true, false);

        switch (getVignettingBlendType())
        {
        case cVignettingBlendType_Normal:
            context.setBlendFactor(rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_ONE_MINUS_SRC_ALPHA);
            context.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
            break;
        case cVignettingBlendType_Add:
            context.setBlendFactor(rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_ONE);
            context.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
            break;
        case cVignettingBlendType_Mult:
            context.setBlendFactor(rio::Graphics::BLEND_MODE_ZERO, rio::Graphics::BLEND_MODE_SRC_COLOR);
            context.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
            break;
        case cVignettingBlendType_Screen:
            context.setBlendFactor(rio::Graphics::BLEND_MODE_ONE_MINUS_DST_COLOR, rio::Graphics::BLEND_MODE_ONE);
            context.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
            break;
        }
    }
    context.apply();

    arg.p_render_buffer->bind();

    // TODO
    /*
    mode = mpCurrentProgramVignetting->activate(mode);

    uniformVignettingParam_(arg, mpCurrentProgramVignetting);

    drawKick_(arg);
    */
    RIO_LOG("Tried to draw vignetting\n");
    RIO_ASSERT(false);

    return mode;
}

void DepthOfField::uniformComposeParam_(const DrawArg& arg, const ShaderProgram* program) const
{
    const f32& arg_far = arg.far;
    const f32& arg_near = arg.near;

    f32 w_inv = 1.0f / arg.width;
    f32 h_inv = 1.0f / arg.height;

    f32 end   = std::max<f32>(*mEnd, *mStart);
    f32 start = std::min<f32>(*mStart, *mEnd);

    if (start == end)
        end += std::numeric_limits<f32>::epsilon();

    rio::Vector4f param;

    param.x = *mLevel;              // cMipLevelMax0
    param.y = *mLevel;              // cMipLevelMax1
    param.z = *mLevel;              // cMipLevelMax2
    param.w = 1.0f - *mSaturateMin; // cSaturate

    program->getUniformLocation(cUniform_Param0).setUniform(param);

    param.x = w_inv * _1f4;
    param.y = h_inv * _1f4;
    param.z = w_inv;
    param.w = h_inv;

    program->getUniformLocation(cUniform_TexParam).setUniform(param);

    f32 range = end - start;

    rio::Vector4f near_far_param;
    rio::Vector4f mul_param;
    rio::Vector4f add_param;

    if (arg.view_depth)
    {
        f32 z = arg_far - arg_near;
        mul_param.x = 1.0f / (range / z);
        add_param.x = -(((start - arg_near) / z) * mul_param.x);
    }
    else
    {
        mul_param.x = 1.0f / range;
        add_param.x = -(start * mul_param.x);
    }

    f32 v0 = arg_far - arg_near;
    f32 v1 = arg_far + arg_near;

    f32 near_far_param_z = (arg_near * start * v1 - 2 * arg_near * arg_near * arg_far) / (arg_near * start * v0);
    f32 near_far_param_w = 0.0f;

    if (*mNearEnable)
    {
        f32 far_end   = std::min<f32>(*mFarEnd, *mFarStart);
        f32 far_start = std::max<f32>(*mFarStart, *mFarEnd);

        if (far_start == far_end)
            far_end += std::numeric_limits<f32>::epsilon();

        f32 far_range = far_end - far_start;
        if (arg.view_depth)
        {
            mul_param.y = 1.0f / (far_range / v0);
            add_param.y = -(((far_start - arg_near) / v0) * mul_param.y);
        }
        else
        {
            mul_param.y = 1.0f / far_range;
            add_param.y = -(far_start * mul_param.y);
        }

        f32 v2 = arg_far - arg_near;
        f32 v3 = arg_far + arg_near;

        near_far_param_w = (arg_near * far_start * v3 - 2 * arg_near * arg_near * arg_far) / (arg_near * far_start * v2);
    }

    near_far_param.x = 1.0f / arg_near;
    near_far_param.y = (1.0f - arg_near / arg_far) / arg_near;
    near_far_param.z = near_far_param_z;
    near_far_param.w = near_far_param_w;

    program->getUniformLocation(cUniform_NearFarParam).setUniform(near_far_param);

    if (*mEnableColorControl)
    {
        f32 color_ctrl_depth_0_end   = std::max<f32>(mColorCtrlDepth->y, mColorCtrlDepth->x);
        f32 color_ctrl_depth_0_start = std::min<f32>(mColorCtrlDepth->x, mColorCtrlDepth->y);

        if (color_ctrl_depth_0_start == color_ctrl_depth_0_end)
            color_ctrl_depth_0_end += std::numeric_limits<f32>::epsilon();

        f32 color_ctrl_depth_1_end   = std::min<f32>(mColorCtrlDepth->w, mColorCtrlDepth->z);
        f32 color_ctrl_depth_1_start = std::max<f32>(mColorCtrlDepth->z, mColorCtrlDepth->w);

        if (color_ctrl_depth_1_start == color_ctrl_depth_1_end)
            color_ctrl_depth_1_end += std::numeric_limits<f32>::epsilon();

        f32 color_ctrl_depth_0_range = color_ctrl_depth_0_end - color_ctrl_depth_0_start;
        f32 color_ctrl_depth_1_range = color_ctrl_depth_1_end - color_ctrl_depth_1_start;

        if (arg.view_depth)
        {
            f32 z = arg_far - arg_near;
            color_ctrl_depth_0_range = color_ctrl_depth_0_range               / z;
            color_ctrl_depth_1_range = color_ctrl_depth_1_range               / z;
            color_ctrl_depth_0_start = (color_ctrl_depth_0_start - arg_near)  / z;
            color_ctrl_depth_1_start = (color_ctrl_depth_1_start - arg_near)  / z;
        }

        mul_param.z = 1.0f / color_ctrl_depth_0_range;
        mul_param.w = 1.0f / color_ctrl_depth_1_range;
        add_param.z = -(color_ctrl_depth_0_start * mul_param.z);
        add_param.w = -(color_ctrl_depth_1_start * mul_param.w);
    }

    program->getUniformLocation(cUniform_MulParam).setUniform(mul_param);
    program->getUniformLocation(cUniform_AddParam).setUniform(add_param);

    program->getUniformLocation(cUniform_FarMulColor).setUniform(mFarMulColor->v);

    arg.p_ctx->mColorTextureSampler.activate(program->getSamplerLocation(cSampler_TexMipMap), cSampler_TexMipMap);

    if (enableDepthBlur_())
        arg.p_ctx->mDepthTextureSampler.activate(program->getSamplerLocation(cSampler_TexMipMapDepth), cSampler_TexMipMapDepth);

    arg.p_ctx->mDepthTargetTextureSampler.activate(program->getSamplerLocation(cSampler_TexDepth), cSampler_TexDepth);

    if (mpIndirectTextureData != nullptr)
    {
        mIndirectTextureSampler.activate(program->getSamplerLocation(cSampler_TexIndirect), cSampler_TexIndirect);

        mIndirectTexParam.z = 1.0f / (w_inv * arg.height); // Aspect ratio?
        program->getUniformLocation(cUniform_IndirectTexParam).setUniform(mIndirectTexParam);

        program->getUniformLocation(cUniform_IndirectTexMtx0).setUniform(reinterpret_cast<const rio::Vector3f&>(mIndirectTexMtx0)); // Passing Vec3f to Vec4f... whoops!
        program->getUniformLocation(cUniform_IndirectTexMtx1).setUniform(reinterpret_cast<const rio::Vector3f&>(mIndirectTexMtx1)); // ^^^
    }
}

void DepthOfField::setIndirectTextureData(const TextureData* p_texture_data)
{
    mpIndirectTextureData = p_texture_data;
    applyIndirectTextureData_();
    assignShaderProgram_();
}

void DepthOfField::setIndirectEnable(bool enable)
{
    if (*mIndirectEnable != enable)
    {
        *mIndirectEnable = enable;
        assignShaderProgram_();
    }
}

void DepthOfField::setIndirectTextureTrans(const rio::Vector2f& trans)
{
    *mIndirectTexTrans = trans;
    updateIndirectMatrix_();
}

void DepthOfField::applyIndirectTextureData_()
{
    if (mpIndirectTextureData == nullptr)
        return;

    mIndirectTextureSampler.applyTextureData(*mpIndirectTextureData);

    mIndirectTexParam.x = mpIndirectTextureData->getWidth();
    mIndirectTexParam.y = mpIndirectTextureData->getHeight();
    mIndirectTexParam.w = *mIndirectScale;
}

void DepthOfField::updateIndirectMatrix_()
{
    rio::Matrix34f mtx;

    mtx.makeR(rio::Vector3f{0.0f, 0.0f, *mIndirectTexRotate});
    mtx.applyScaleLocal(rio::Vector3f{mIndirectTexScale->x, mIndirectTexScale->y, 0.0f});
    mtx.applyTranslationLocal(rio::Vector3f{mIndirectTexTrans->x, mIndirectTexTrans->y, 0.0f});

    mIndirectTexMtx0.x = mtx.m[0][0];
    mIndirectTexMtx0.y = mtx.m[0][1];
    mIndirectTexMtx0.z = mtx.m[0][3];

    mIndirectTexMtx1.x = mtx.m[1][0];
    mIndirectTexMtx1.y = mtx.m[1][1];
    mIndirectTexMtx1.z = mtx.m[1][3];
}

void DepthOfField::postRead_()
{
    assignShaderProgram_();
    applyIndirectTextureData_();
    updateIndirectMatrix_();
}

DepthOfField::DrawArg::DrawArg(Context& ctx, const RenderBuffer& render_buffer, const TextureData& depth, bool view_depth_, f32 near_, f32 far_)
    : p_ctx(&ctx)
    , p_render_buffer(&render_buffer)
    , near(near_)
    , far(far_)
    , width(p_render_buffer->getRenderTargetColor()->getTextureData().getWidth())
    , height(p_render_buffer->getRenderTargetColor()->getTextureData().getHeight())
    , view_depth(view_depth_)
{
    p_ctx->mColorTargetTextureSampler.applyTextureData(p_render_buffer->getRenderTargetColor()->getTextureData());
    p_ctx->mDepthTargetTextureSampler.applyTextureData(depth);
}

DepthOfField::TempVignetting::TempVignetting(DepthOfField* p_dof, const char* param_name)
    : utl::IParameterObj()
    , mType (0,                             "type",  "形状",    this)
    , mRange(rio::Vector2f{0.25f, 1.0f},    "range", "変化幅",  this)
    , mScale(rio::Vector2f{1.0f,  1.0f},    "scale", "スケール",  this)
    , mTrans(rio::Vector2f{0.0f,  0.0f},    "trans", "オフセット", this)
{
    p_dof->addObj(this, param_name);
}

DepthOfField::VignettingShape::VignettingShape() = default;
DepthOfField::VignettingShape::~VignettingShape() = default;

} }
