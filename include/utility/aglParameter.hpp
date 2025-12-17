#pragma once

#include <gfx/rio_Color.h>
#include <math/rio_Vector.h>

namespace agl { namespace utl {

// static_assert(sizeof(Parameter<bool>) == 0x14);
// static_assert(sizeof(Parameter<s32>) == 0x14);
// static_assert(sizeof(Parameter<f32>) == 0x14);
// static_assert(sizeof(Parameter<rio::Vector2f>) == 0x18);
// static_assert(sizeof(Parameter<rio::Vector3f>) == 0x1C);
// static_assert(sizeof(Parameter<rio::Vector4f>) == 0x20);
// static_assert(sizeof(Parameter<rio::Color4f>) == 0x20);
//static_assert(sizeof(Parameter< char[32] >) == 0x3C);

template <typename T>
inline void
ParameterBase::copyLerp_(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t)
{
    static_cast<T*>(ptr())->setLerp(*static_cast<const T*>(parameter_a.ptr()), *static_cast<const T*>(parameter_b.ptr()), t);
}

namespace {

static inline f32 lerp(f32 a, f32 b, f32 t)
{
    return a * (1 - t) + b * t;
}

}

template <>
inline void
ParameterBase::copyLerp_<f32>(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t)
{
    *static_cast<f32*>(ptr()) = lerp(*static_cast<const f32*>(parameter_a.ptr()), *static_cast<const f32*>(parameter_b.ptr()), t);
}

template <>
inline void
ParameterBase::copyLerp_<rio::Vector2f>(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t)
{
    static_cast<rio::Vector2f*>(ptr())->x = lerp(static_cast<const rio::Vector2f*>(parameter_a.ptr())->x, static_cast<const rio::Vector2f*>(parameter_b.ptr())->x, t);
    static_cast<rio::Vector2f*>(ptr())->y = lerp(static_cast<const rio::Vector2f*>(parameter_a.ptr())->y, static_cast<const rio::Vector2f*>(parameter_b.ptr())->y, t);
}

template <>
inline void
ParameterBase::copyLerp_<rio::Vector3f>(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t)
{
    static_cast<rio::Vector3f*>(ptr())->x = lerp(static_cast<const rio::Vector3f*>(parameter_a.ptr())->x, static_cast<const rio::Vector3f*>(parameter_b.ptr())->x, t);
    static_cast<rio::Vector3f*>(ptr())->y = lerp(static_cast<const rio::Vector3f*>(parameter_a.ptr())->y, static_cast<const rio::Vector3f*>(parameter_b.ptr())->y, t);
    static_cast<rio::Vector3f*>(ptr())->z = lerp(static_cast<const rio::Vector3f*>(parameter_a.ptr())->z, static_cast<const rio::Vector3f*>(parameter_b.ptr())->z, t);
}

template <>
inline void
ParameterBase::copyLerp_<rio::Vector4f>(const ParameterBase& parameter_a, const ParameterBase& parameter_b, f32 t)
{
    static_cast<rio::Vector4f*>(ptr())->x = lerp(static_cast<const rio::Vector4f*>(parameter_a.ptr())->x, static_cast<const rio::Vector4f*>(parameter_b.ptr())->x, t);
    static_cast<rio::Vector4f*>(ptr())->y = lerp(static_cast<const rio::Vector4f*>(parameter_a.ptr())->y, static_cast<const rio::Vector4f*>(parameter_b.ptr())->y, t);
    static_cast<rio::Vector4f*>(ptr())->z = lerp(static_cast<const rio::Vector4f*>(parameter_a.ptr())->z, static_cast<const rio::Vector4f*>(parameter_b.ptr())->z, t);
    static_cast<rio::Vector4f*>(ptr())->w = lerp(static_cast<const rio::Vector4f*>(parameter_a.ptr())->w, static_cast<const rio::Vector4f*>(parameter_b.ptr())->w, t);
}

template <>
inline ParameterBase::ParameterType
Parameter<bool>::getParameterType() const
{
    return cType_bool;
}

template <>
inline ParameterBase::ParameterType
Parameter<s32>::getParameterType() const
{
    return cType_int;
}

template <>
inline ParameterBase::ParameterType
Parameter<f32>::getParameterType() const
{
    return cType_f32;
}

template <>
inline ParameterBase::ParameterType
Parameter<rio::Vector2f>::getParameterType() const
{
    return cType_vec2;
}

template <>
inline ParameterBase::ParameterType
Parameter<rio::Vector3f>::getParameterType() const
{
    return cType_vec3;
}

template <>
inline ParameterBase::ParameterType
Parameter<rio::Vector4f>::getParameterType() const
{
    return cType_vec4;
}

template <>
inline ParameterBase::ParameterType
Parameter<rio::Color4f>::getParameterType() const
{
    return cType_color;
}

template <>
inline ParameterBase::ParameterType
Parameter< char[32] >::getParameterType() const
{
    return cType_string32;
}

} }
