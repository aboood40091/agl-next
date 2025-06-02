#include <common/aglTextureFormatInfo.h>
#include <detail/aglTextureDataUtil.h>

namespace agl {

namespace {

struct FormatParam
{
    u8 component_bit_size[4];
    s8 component_order[4];
    u8 pixel_byte_size;
    u8 component_num;
    bool is_compressed;
    bool is_normalized;
    bool is_float;
    bool is_unsigned;
    bool is_usable_as_render_target_color;
    bool is_usable_as_render_target_depth;
    bool is_srgb;
};
static_assert(sizeof(FormatParam) == 0x11);

static FormatParam s_format_param[cTextureFormat_Num] = {
    {// cTextureFormat_Invalid
        {  0,  0,  0,  0 }, // component_bit_size
        { -1, -1, -1, -1 }, // component_order
         0,                 // pixel_byte_size
         0,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_uNorm
        {  8,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         1,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_uInt
        {  8,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         1,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_sNorm
        {  8,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         1,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_sInt
        {  8,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         1,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_uNorm
        { 16,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_uInt
        { 16,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_sNorm
        { 16,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_sInt
        { 16,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_float
        { 16,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_uNorm
        {  8,  8,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_uInt
        {  8,  8,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_sNorm
        {  8,  8,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_sInt
        {  8,  8,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R5_G6_B5_uNorm
        {  5,  6,  5,  0 }, // component_bit_size
        {  2,  1,  0, -1 }, // component_order
         2,                 // pixel_byte_size
         3,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_A1_B5_G5_R5_uNorm
        {  5,  5,  5,  1 }, // component_bit_size
        {  0,  1,  2,  3 }, // component_order
         2,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R4_G4_B4_A4_uNorm
        {  4,  4,  4,  4 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         2,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R5_G5_B5_A1_uNorm
        {  5,  5,  5,  1 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         2,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_uInt
        { 32,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_sInt
        { 32,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_float
        { 32,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_uNorm
        { 16, 16,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_uInt
        { 16, 16,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_sNorm
        { 16, 16,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_sInt
        { 16, 16,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_float
        { 16, 16,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R11_G11_B10_float
        { 11, 11, 10,  0 }, // component_bit_size
        {  2,  1,  0, -1 }, // component_order
         4,                 // pixel_byte_size
         3,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_A2_B10_G10_R10_uNorm
        { 10, 10, 10,  2 }, // component_bit_size
        {  0,  1,  2,  3 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_A2_B10_G10_R10_uInt
        { 10, 10, 10,  2 }, // component_bit_size
        {  0,  1,  2,  3 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_B8_A8_uNorm
        {  8,  8,  8,  8 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_B8_A8_uInt
        {  8,  8,  8,  8 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_B8_A8_sNorm
        {  8,  8,  8,  8 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_B8_A8_sInt
        {  8,  8,  8,  8 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R8_G8_B8_A8_SRGB
        {  8,  8,  8,  8 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        true                // is_srgb
    },
    {// cTextureFormat_R10_G10_B10_A2_uNorm
        { 10, 10, 10,  2 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R10_G10_B10_A2_uInt
        { 10, 10, 10,  2 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         4,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_G32_uInt
        { 32, 32,  0,  0 }, // component_bit_size
        {  0,  1, -1, -1 }, // component_order
         8,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_G32_sInt
        { 32, 32,  0,  0 }, // component_bit_size
        {  0,  1, -1, -1 }, // component_order
         8,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_G32_float
        { 32, 32,  0,  0 }, // component_bit_size
        {  0,  1, -1, -1 }, // component_order
         8,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_B16_A16_uNorm
        { 16, 16, 16, 16 }, // component_bit_size
        {  1,  0,  3,  2 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_B16_A16_uInt
        { 16, 16, 16, 16 }, // component_bit_size
        {  1,  0,  3,  2 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_B16_A16_sNorm
        { 16, 16, 16, 16 }, // component_bit_size
        {  1,  0,  3,  2 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_B16_A16_sInt
        { 16, 16, 16, 16 }, // component_bit_size
        {  1,  0,  3,  2 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R16_G16_B16_A16_float
        { 16, 16, 16, 16 }, // component_bit_size
        {  1,  0,  3,  2 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_G32_B32_A32_uInt
        { 32, 32, 32, 32 }, // component_bit_size
        {  0,  1,  2,  3 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        true,               // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_G32_B32_A32_sInt
        { 32, 32, 32, 32 }, // component_bit_size
        {  0,  1,  2,  3 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        false,              // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_R32_G32_B32_A32_float
        { 32, 32, 32, 32 }, // component_bit_size
        {  0,  1,  2,  3 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        true,               // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC1_uNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC1_SRGB
        {  0,  0,  0,  0 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
         8,                 // pixel_byte_size
         4,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        true                // is_srgb
    },
    {// cTextureFormat_BC2_uNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC2_SRGB
        {  0,  0,  0,  0 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        true                // is_srgb
    },
    {// cTextureFormat_BC3_uNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC3_SRGB
        {  0,  0,  0,  0 }, // component_bit_size
        {  3,  2,  1,  0 }, // component_order
        16,                 // pixel_byte_size
         4,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        true                // is_srgb
    },
    {// cTextureFormat_BC4_uNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         8,                 // pixel_byte_size
         1,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC4_sNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         8,                 // pixel_byte_size
         1,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC5_uNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
        16,                 // pixel_byte_size
         2,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_BC5_sNorm
        {  0,  0,  0,  0 }, // component_bit_size
        {  1,  0, -1, -1 }, // component_order
        16,                 // pixel_byte_size
         2,                 // component_num
        true,               // is_compressed
        true,               // is_normalized
        false,              // is_float
        false,              // is_unsigned
        false,              // is_usable_as_render_target_color
        false,              // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_Depth_16
        { 16,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         2,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        true,               // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_Depth_32
        { 32,  0,  0,  0 }, // component_bit_size
        {  0, -1, -1, -1 }, // component_order
         4,                 // pixel_byte_size
         1,                 // component_num
        false,              // is_compressed
        false,              // is_normalized
        true,               // is_float
        false,              // is_unsigned
        false,              // is_usable_as_render_target_color
        true,               // is_usable_as_render_target_depth
        false               // is_srgb
    },
    {// cTextureFormat_Depth_24_uNorm_Stencil_8 (This one is simply wrong and was only fixed later in *Switch* games)
        { 24, 16,  0,  0 }, // component_bit_size   (Should be: { 24,  8,  0,  0 })
        {  0,  2, -1, -1 }, // component_order      (Should be: {  0,  1, -1, -1 })
         4,                 // pixel_byte_size
         2,                 // component_num
        false,              // is_compressed
        true,               // is_normalized
        false,              // is_float
        true,               // is_unsigned
        false,              // is_usable_as_render_target_color
        true,               // is_usable_as_render_target_depth
        false               // is_srgb
    }
};

static const char* s_texture_format_string[cTextureFormat_Num] = {
    "cTextureFormat_Invalid",
    "cTextureFormat_R8_uNorm",
    "cTextureFormat_R8_uInt",
    "cTextureFormat_R8_sNorm",
    "cTextureFormat_R8_sInt",
    "cTextureFormat_R16_uNorm",
    "cTextureFormat_R16_uInt",
    "cTextureFormat_R16_sNorm",
    "cTextureFormat_R16_sInt",
    "cTextureFormat_R16_float",
    "cTextureFormat_R8_G8_uNorm",
    "cTextureFormat_R8_G8_uInt",
    "cTextureFormat_R8_G8_sNorm",
    "cTextureFormat_R8_G8_sInt",
    "cTextureFormat_R5_G6_B5_uNorm",
    "cTextureFormat_A1_B5_G5_R5_uNorm",
    "cTextureFormat_R4_G4_B4_A4_uNorm",
    "cTextureFormat_R5_G5_B5_A1_uNorm",
    "cTextureFormat_R32_uInt",
    "cTextureFormat_R32_sInt",
    "cTextureFormat_R32_float",
    "cTextureFormat_R16_G16_uNorm",
    "cTextureFormat_R16_G16_uInt",
    "cTextureFormat_R16_G16_sNorm",
    "cTextureFormat_R16_G16_sInt",
    "cTextureFormat_R16_G16_float",
    "cTextureFormat_R11_G11_B10_float",
    "cTextureFormat_A2_B10_G10_R10_uNorm",
    "cTextureFormat_A2_B10_G10_R10_uInt",
    "cTextureFormat_R8_G8_B8_A8_uNorm",
    "cTextureFormat_R8_G8_B8_A8_uInt",
    "cTextureFormat_R8_G8_B8_A8_sNorm",
    "cTextureFormat_R8_G8_B8_A8_sInt",
    "cTextureFormat_R8_G8_B8_A8_SRGB",
    "cTextureFormat_R10_G10_B10_A2_uNorm",
    "cTextureFormat_R10_G10_B10_A2_uInt",
    "cTextureFormat_R32_G32_uInt",
    "cTextureFormat_R32_G32_sInt",
    "cTextureFormat_R32_G32_float",
    "cTextureFormat_R16_G16_B16_A16_uNorm",
    "cTextureFormat_R16_G16_B16_A16_uInt",
    "cTextureFormat_R16_G16_B16_A16_sNorm",
    "cTextureFormat_R16_G16_B16_A16_sInt",
    "cTextureFormat_R16_G16_B16_A16_float",
    "cTextureFormat_R32_G32_B32_A32_uInt",
    "cTextureFormat_R32_G32_B32_A32_sInt",
    "cTextureFormat_R32_G32_B32_A32_float",
    "cTextureFormat_BC1_uNorm",
    "cTextureFormat_BC1_SRGB",
    "cTextureFormat_BC2_uNorm",
    "cTextureFormat_BC2_SRGB",
    "cTextureFormat_BC3_uNorm",
    "cTextureFormat_BC3_SRGB",
    "cTextureFormat_BC4_uNorm",
    "cTextureFormat_BC4_sNorm",
    "cTextureFormat_BC5_uNorm",
    "cTextureFormat_BC5_sNorm",
    "cTextureFormat_Depth_16",
    "cTextureFormat_Depth_32",
    "cTextureFormat_Depth_24_uNorm_Stencil_8"
};

static TextureCompSel s_default_comp_sel[cTextureFormat_Num][4] = {
    {// cTextureFormat_Invalid
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_0
    },
    {// cTextureFormat_R8_uNorm
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_uInt
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_sNorm
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_sInt
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_uNorm
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_uInt
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_sNorm
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_sInt
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_float
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_G8_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_G8_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_G8_sNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R8_G8_sInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R5_G6_B5_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_1
    },
    {// cTextureFormat_A1_B5_G5_R5_uNorm
        cTextureCompSel_A,
        cTextureCompSel_B,
        cTextureCompSel_G,
        cTextureCompSel_R
    },
    {// cTextureFormat_R4_G4_B4_A4_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R5_G5_B5_A1_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R32_uInt
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R32_sInt
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R32_float
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_G16_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_G16_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_G16_sNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_G16_sInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_G16_float
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R11_G11_B10_float
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_1
    },
    {// cTextureFormat_A2_B10_G10_R10_uNorm
        cTextureCompSel_A,
        cTextureCompSel_B,
        cTextureCompSel_G,
        cTextureCompSel_R
    },
    {// cTextureFormat_A2_B10_G10_R10_uInt
        cTextureCompSel_A,
        cTextureCompSel_B,
        cTextureCompSel_G,
        cTextureCompSel_R
    },
    {// cTextureFormat_R8_G8_B8_A8_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R8_G8_B8_A8_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R8_G8_B8_A8_sNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R8_G8_B8_A8_sInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R8_G8_B8_A8_SRGB
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R10_G10_B10_A2_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R10_G10_B10_A2_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R32_G32_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R32_G32_sInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R32_G32_float
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_R16_G16_B16_A16_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R16_G16_B16_A16_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R16_G16_B16_A16_sNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R16_G16_B16_A16_sInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R16_G16_B16_A16_float
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R32_G32_B32_A32_uInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R32_G32_B32_A32_sInt
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_R32_G32_B32_A32_float
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC1_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC1_SRGB
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC2_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC2_SRGB
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC3_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC3_SRGB
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_B,
        cTextureCompSel_A
    },
    {// cTextureFormat_BC4_uNorm
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_BC4_sNorm
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_BC5_uNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_BC5_sNorm
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_Depth_16
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_Depth_32
        cTextureCompSel_R,
        cTextureCompSel_0,
        cTextureCompSel_0,
        cTextureCompSel_1
    },
    {// cTextureFormat_Depth_24_uNorm_Stencil_8
        cTextureCompSel_R,
        cTextureCompSel_G,
        cTextureCompSel_0,
        cTextureCompSel_1
    }
};

}

u8 TextureFormatInfo::getPixelByteSize(TextureFormat format)
{
    return s_format_param[format].pixel_byte_size;
}

u8 TextureFormatInfo::getComponentNum(TextureFormat format)
{
    return s_format_param[format].component_num;
}

u8 TextureFormatInfo::getComponentBitSize(TextureFormat format, s32 component)
{
    return s_format_param[format].component_bit_size[component];
}

u8 TextureFormatInfo::getComponentOrder(TextureFormat format, s32 component)
{
    return s_format_param[format].component_order[component];
}

bool TextureFormatInfo::isCompressed(TextureFormat format)
{
    return s_format_param[format].is_compressed;
}

bool TextureFormatInfo::isNormalized(TextureFormat format)
{
    return s_format_param[format].is_normalized;
}
bool TextureFormatInfo::isFloat(TextureFormat format)
{
    return s_format_param[format].is_float;
}

bool TextureFormatInfo::isUnsigned(TextureFormat format)
{
    return s_format_param[format].is_unsigned;
}

bool TextureFormatInfo::isSRGB(TextureFormat format)
{
    return s_format_param[format].is_srgb;
}

bool TextureFormatInfo::isUsableAsRenderTargetColor(TextureFormat format)
{
    return s_format_param[format].is_usable_as_render_target_color;
}

bool TextureFormatInfo::isUsableAsRenderTargetDepth(TextureFormat format)
{
    return s_format_param[format].is_usable_as_render_target_depth;
}

const char* TextureFormatInfo::getString(TextureFormat format)
{
    return s_texture_format_string[format];
}

TextureCompSel TextureFormatInfo::getDefaultCompSel(TextureFormat format, s32 component)
{
    return s_default_comp_sel[format][component];
}

TextureFormat TextureFormatInfo::convFormatGX2ToAGL(GX2SurfaceFormat format, bool color_target, bool depth_target)
{
    return detail::TextureDataUtil::convFormatGX2ToAGL(format, color_target, depth_target);
}

GX2SurfaceFormat TextureFormatInfo::convFormatAGLToGX2(TextureFormat format)
{
    return detail::TextureDataUtil::convFormatAGLToGX2(format);
}

#if RIO_IS_DESKTOP

bool TextureFormatInfo::setNativeTextureFormat(rio::NativeTextureFormat* p_native_format, TextureFormat format)
{
    switch (format)
    {
    case cTextureFormat_R8_uNorm:
        p_native_format->internalformat = GL_R8;
        p_native_format->format = GL_RED;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R8_uInt:
        p_native_format->internalformat = GL_R8UI;
        p_native_format->format = GL_RED_INTEGER;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R8_sNorm:
        p_native_format->internalformat = GL_R8_SNORM;
        p_native_format->format = GL_RED;
        p_native_format->type = GL_BYTE;
        return true;

    case cTextureFormat_R8_sInt:
        p_native_format->internalformat = GL_R8I;
        p_native_format->format = GL_RED_INTEGER;
        p_native_format->type = GL_BYTE;
        return true;

    case cTextureFormat_R16_uNorm:
        p_native_format->internalformat = GL_R16;
        p_native_format->format = GL_RED;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_R16_uInt:
        p_native_format->internalformat = GL_R16UI;
        p_native_format->format = GL_RED_INTEGER;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_R16_sNorm:
        p_native_format->internalformat = GL_R16_SNORM;
        p_native_format->format = GL_RED;
        p_native_format->type = GL_SHORT;
        return true;

    case cTextureFormat_R16_sInt:
        p_native_format->internalformat = GL_R16I;
        p_native_format->format = GL_RED_INTEGER;
        p_native_format->type = GL_SHORT;
        return true;

    case cTextureFormat_R16_float:
        p_native_format->internalformat = GL_R16F;
        p_native_format->format = GL_RED;
        p_native_format->type = GL_HALF_FLOAT;
        return true;

    case cTextureFormat_R8_G8_uNorm:
        p_native_format->internalformat = GL_RG8;
        p_native_format->format = GL_RG;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R8_G8_uInt:
        p_native_format->internalformat = GL_RG8UI;
        p_native_format->format = GL_RG_INTEGER;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R8_G8_sNorm:
        p_native_format->internalformat = GL_RG8_SNORM;
        p_native_format->format = GL_RG;
        p_native_format->type = GL_BYTE;
        return true;

    case cTextureFormat_R8_G8_sInt:
        p_native_format->internalformat = GL_RG8I;
        p_native_format->format = GL_RG_INTEGER;
        p_native_format->type = GL_BYTE;
        return true;

    case cTextureFormat_R5_G6_B5_uNorm:
        p_native_format->internalformat = GL_RGB565;
        p_native_format->format = GL_RGB;
        p_native_format->type = GL_UNSIGNED_SHORT_5_6_5;
        return true;

    case cTextureFormat_A1_B5_G5_R5_uNorm:
        p_native_format->internalformat = GL_RGB5_A1;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
        return true;

    case cTextureFormat_R4_G4_B4_A4_uNorm:
        p_native_format->internalformat = GL_RGBA4;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_SHORT_4_4_4_4;
        return true;

    case cTextureFormat_R5_G5_B5_A1_uNorm:
        p_native_format->internalformat = GL_RGB5_A1;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_SHORT_5_5_5_1;
        return true;

    case cTextureFormat_R32_uInt:
        p_native_format->internalformat = GL_R32UI;
        p_native_format->format = GL_RED_INTEGER;
        p_native_format->type = GL_UNSIGNED_INT;
        return true;

    case cTextureFormat_R32_sInt:
        p_native_format->internalformat = GL_R32I;
        p_native_format->format = GL_RED_INTEGER;
        p_native_format->type = GL_INT;
        return true;

    case cTextureFormat_R32_float:
        p_native_format->internalformat = GL_R32F;
        p_native_format->format = GL_RED;
        p_native_format->type = GL_FLOAT;
        return true;

    case cTextureFormat_R16_G16_uNorm:
        p_native_format->internalformat = GL_RG16;
        p_native_format->format = GL_RG;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_R16_G16_uInt:
        p_native_format->internalformat = GL_RG16UI;
        p_native_format->format = GL_RG_INTEGER;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_R16_G16_sNorm:
        p_native_format->internalformat = GL_RG16_SNORM;
        p_native_format->format = GL_RG;
        p_native_format->type = GL_SHORT;
        return true;

    case cTextureFormat_R16_G16_sInt:
        p_native_format->internalformat = GL_RG16I;
        p_native_format->format = GL_RG_INTEGER;
        p_native_format->type = GL_SHORT;
        return true;

    case cTextureFormat_R16_G16_float:
        p_native_format->internalformat = GL_RG16F;
        p_native_format->format = GL_RG;
        p_native_format->type = GL_HALF_FLOAT;
        return true;

    case cTextureFormat_R11_G11_B10_float:
        p_native_format->internalformat = GL_R11F_G11F_B10F;
        p_native_format->format = GL_RGB;
        p_native_format->type = GL_UNSIGNED_INT_10F_11F_11F_REV;
        return true;

    case cTextureFormat_A2_B10_G10_R10_uNorm:
        p_native_format->internalformat = GL_RGB10_A2;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_INT_10_10_10_2;
        return true;

    case cTextureFormat_A2_B10_G10_R10_uInt:
        p_native_format->internalformat = GL_RGB10_A2UI;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_UNSIGNED_INT_10_10_10_2;
        return true;

    case cTextureFormat_R8_G8_B8_A8_uNorm:
        p_native_format->internalformat = GL_RGBA8;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R8_G8_B8_A8_uInt:
        p_native_format->internalformat = GL_RGBA8UI;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R8_G8_B8_A8_sNorm:
        p_native_format->internalformat = GL_RGBA8_SNORM;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_BYTE;
        return true;

    case cTextureFormat_R8_G8_B8_A8_sInt:
        p_native_format->internalformat = GL_RGBA8I;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_BYTE;
        return true;

    case cTextureFormat_R8_G8_B8_A8_SRGB:
        p_native_format->internalformat = GL_SRGB8_ALPHA8;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_BYTE;
        return true;

    case cTextureFormat_R10_G10_B10_A2_uNorm:
        p_native_format->internalformat = GL_RGB10_A2;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_INT_2_10_10_10_REV;
        return true;

    case cTextureFormat_R10_G10_B10_A2_uInt:
        p_native_format->internalformat = GL_RGB10_A2UI;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_UNSIGNED_INT_2_10_10_10_REV;
        return true;

    case cTextureFormat_R32_G32_uInt:
        p_native_format->internalformat = GL_RG32UI;
        p_native_format->format = GL_RG_INTEGER;
        p_native_format->type = GL_UNSIGNED_INT;
        return true;

    case cTextureFormat_R32_G32_sInt:
        p_native_format->internalformat = GL_RG32I;
        p_native_format->format = GL_RG_INTEGER;
        p_native_format->type = GL_INT;
        return true;

    case cTextureFormat_R32_G32_float:
        p_native_format->internalformat = GL_RG32F;
        p_native_format->format = GL_RG;
        p_native_format->type = GL_FLOAT;
        return true;

    case cTextureFormat_R16_G16_B16_A16_uNorm:
        p_native_format->internalformat = GL_RGBA16;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_R16_G16_B16_A16_uInt:
        p_native_format->internalformat = GL_RGBA16UI;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_R16_G16_B16_A16_sNorm:
        p_native_format->internalformat = GL_RGBA16_SNORM;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_SHORT;
        return true;

    case cTextureFormat_R16_G16_B16_A16_sInt:
        p_native_format->internalformat = GL_RGBA16I;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_SHORT;
        return true;

    case cTextureFormat_R16_G16_B16_A16_float:
        p_native_format->internalformat = GL_RGBA16F;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_HALF_FLOAT;
        return true;

    case cTextureFormat_R32_G32_B32_A32_uInt:
        p_native_format->internalformat = GL_RGBA32UI;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_UNSIGNED_INT;
        return true;

    case cTextureFormat_R32_G32_B32_A32_sInt:
        p_native_format->internalformat = GL_RGBA32I;
        p_native_format->format = GL_RGBA_INTEGER;
        p_native_format->type = GL_INT;
        return true;

    case cTextureFormat_R32_G32_B32_A32_float:
        p_native_format->internalformat = GL_RGBA32F;
        p_native_format->format = GL_RGBA;
        p_native_format->type = GL_FLOAT;
        return true;

    case cTextureFormat_BC1_uNorm:
        p_native_format->internalformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC1_SRGB:
        p_native_format->internalformat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC2_uNorm:
        p_native_format->internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC2_SRGB:
        p_native_format->internalformat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC3_uNorm:
        p_native_format->internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC3_SRGB:
        p_native_format->internalformat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC4_uNorm:
        p_native_format->internalformat = GL_COMPRESSED_RED_RGTC1;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC4_sNorm:
        p_native_format->internalformat = GL_COMPRESSED_SIGNED_RED_RGTC1;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC5_uNorm:
        p_native_format->internalformat = GL_COMPRESSED_RG_RGTC2;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_BC5_sNorm:
        p_native_format->internalformat = GL_COMPRESSED_SIGNED_RG_RGTC2;
        p_native_format->format = 0;
        p_native_format->type = 0;
        return true;

    case cTextureFormat_Depth_16:
        p_native_format->internalformat = GL_DEPTH_COMPONENT16;
        p_native_format->format = GL_DEPTH_COMPONENT;
        p_native_format->type = GL_UNSIGNED_SHORT;
        return true;

    case cTextureFormat_Depth_32:
        p_native_format->internalformat = GL_DEPTH_COMPONENT32F;
        p_native_format->format = GL_DEPTH_COMPONENT;
        p_native_format->type = GL_FLOAT;
        return true;

    case cTextureFormat_Depth_24_uNorm_Stencil_8:
        p_native_format->internalformat = GL_DEPTH24_STENCIL8;
        p_native_format->format = GL_DEPTH_STENCIL;
        p_native_format->type = GL_UNSIGNED_INT_24_8;
        return true;

    default:
        return false;
    }
}

#endif // RIO_IS_DESKTOP

}
