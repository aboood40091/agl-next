#pragma once

#include <container/Buffer.h>
#include <container/SafeArray.h>

#if RIO_IS_CAFE
#include <cafe/gx2.h>
#endif // RIO_IS_CAFE

namespace agl {

class VertexBuffer;

class VertexAttribute
{
    struct Attribute_
    {
        Attribute_()
            : mpVertexBuffer(nullptr)
            , mStreamIndex(-1)
            , mBufferIndex(-1)
        {
        }

        const VertexBuffer* mpVertexBuffer;
        s32 mStreamIndex;
        s32 mBufferIndex;
    };
    static_assert(sizeof(Attribute_) == 0xC, "agl::VertexAttribute::Attribute_ size mismatch");

public:
    static const s32 cVertexAttributeMax = 16;

public:
    VertexAttribute();
    virtual ~VertexAttribute();

    VertexAttribute(const VertexAttribute&) = delete;
    VertexAttribute(VertexAttribute&&) = delete;
    VertexAttribute& operator=(const VertexAttribute&) = delete;
    VertexAttribute& operator=(VertexAttribute&&) = delete;

    void create(u32 buffer_max);
    void cleanUp();
    void destroy();

    void setVertexStream(s16 location, const VertexBuffer* buffer, u32 stream_index);
    void setUp();

    void activate() const;

private:
    s32 enableVertexBuffer_(Attribute_* attr, const VertexBuffer* buffer, u32 stream_index);
    s32 disableVertexBuffer_(Attribute_* attr);

private:
    SafeArray<Attribute_, cVertexAttributeMax> mAttribute;
    Buffer<const VertexBuffer*> mVertexBuffer;
    bool mSetupFinish;
    bool mCreateFinish;
#if RIO_IS_CAFE
    GX2FetchShader mFetchShader;
    u8* mpFetchShaderBuf;
#elif RIO_IS_DESKTOP
    u32 mHandle;
#endif
};
//static_assert(sizeof(VertexAttribute) == 0xF4, "agl::VertexAttribute size mismatch");

}
