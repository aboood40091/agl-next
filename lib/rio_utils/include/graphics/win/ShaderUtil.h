#pragma once

#include <misc/rio_Types.h>

#include <cafe/gx2/gx2Shaders.h>

#include <string>

class GFDFile;

class ShaderUtil
{
public:
    static std::string sTempPath;
    static std::string sGx2ShaderDecompilerPath;
    static std::string sSpirvCrossPath;

public:
    static bool decompileGsh(
        const GX2VertexShader& vertex_shader,
        const GX2PixelShader& pixel_shader,
      //const GX2GeometryShader* p_geometry_shader, // <-- TODO
        const std::string& out_vert_fname, const std::string& out_frag_fname /* ,
      //const std::string& out_geom_fname, // <-- TODO */
    );
};
