#include <common/aglTextureData.h>
#include <detail/aglTextureDataUtil.h>
#include <g3d/aglTextureDataInitializerG3D.h>

#include <nw/g3d/res/g3d_ResFile.h>

namespace {

agl::TextureCompSel GetAglTexCompSel(GX2Component comp)
{
    switch (comp)
    {
    case GX2_COMPONENT_X_R:
        return agl::cTextureCompSel_R;
    case GX2_COMPONENT_Y_G:
        return agl::cTextureCompSel_G;
    case GX2_COMPONENT_Z_B:
        return agl::cTextureCompSel_B;
    case GX2_COMPONENT_W_A:
        return agl::cTextureCompSel_A;
    case GX2_COMPONENT_C_0:
        return agl::cTextureCompSel_0;
    case GX2_COMPONENT_C_1:
        return agl::cTextureCompSel_1;
    }

    return agl::cTextureCompSel_0;
}

}

namespace agl { namespace g3d {

void TextureDataInitializerG3D::initialize(TextureData* p_texture_data, void* p_bfres_file_data, u32 index)
{
    RIO_ASSERT(p_bfres_file_data != nullptr);
    RIO_ASSERT(nw::g3d::res::ResFile::IsValid( p_bfres_file_data ));

    nw::g3d::res::ResFile* p_res_file = nw::g3d::res::ResFile::ResCast(p_bfres_file_data);
  //p_res_file->Setup(); // <--- Nintendo forgot to do this

    nw::g3d::res::ResTexture* p_res_texture = p_res_file->GetTexture(index);
    if (p_res_texture == nullptr)
    {
        RIO_LOG("invalid texture index : %d\n", index);
        RIO_ASSERT(false);
    }

    initialize(p_texture_data, *p_res_texture);
}

void TextureDataInitializerG3D::initialize(TextureData* p_texture_data, void* p_bfres_file_data, const char* name)
{
    RIO_ASSERT(p_bfres_file_data != nullptr);
    RIO_ASSERT(nw::g3d::res::ResFile::IsValid( p_bfres_file_data ));

    nw::g3d::res::ResFile* p_res_file = nw::g3d::res::ResFile::ResCast(p_bfres_file_data);
  //p_res_file->Setup(); // <--- Nintendo forgot to do this

    nw::g3d::res::ResTexture* p_res_texture = p_res_file->GetTexture(name);
    if (p_res_texture == nullptr)
    {
        RIO_LOG("invalid texture name : \"%s\"\n", name);
        RIO_ASSERT(false);
    }

    initialize(p_texture_data, *p_res_texture);
}

void TextureDataInitializerG3D::initialize(TextureData* p_texture_data, nw::g3d::res::ResTexture& res_texture)
{
    RIO_ASSERT(p_texture_data != nullptr);

    GX2Texture texture = *res_texture.GetGfxTexture()->GetGX2Texture();
    texture.surface.imagePtr.set(res_texture.GetData());
    texture.surface.mipPtr.set(res_texture.GetMipData());

    detail::TextureDataUtil::initializeFromSurface(p_texture_data, texture.surface);

    p_texture_data->setTextureCompSel(
        GetAglTexCompSel(GX2Component(GX2_GET_COMPONENT_X_R(texture.compSel))),
        GetAglTexCompSel(GX2Component(GX2_GET_COMPONENT_Y_G(texture.compSel))),
        GetAglTexCompSel(GX2Component(GX2_GET_COMPONENT_Z_B(texture.compSel))),
        GetAglTexCompSel(GX2Component(GX2_GET_COMPONENT_W_A(texture.compSel)))
    );
}

} }
