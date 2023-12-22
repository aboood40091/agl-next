#pragma once

#include <misc/rio_Types.h>

#if RIO_IS_CAFE
#include <cafe.h>
#endif // RIO_IS_CAFE

namespace agl {

class DisplayList
{
public:
#if RIO_IS_CAFE
    static const u32 cDisplayListAlignment = GX2_DISPLAY_LIST_ALIGNMENT;
#else
    static const u32 cDisplayListAlignment = 4;
#endif

public:
    DisplayList();
    virtual ~DisplayList();

    DisplayList(const DisplayList&) = delete;
    DisplayList(DisplayList&&) = delete;
    DisplayList& operator=(const DisplayList&) = delete;
    DisplayList& operator=(DisplayList&&) = delete;

    u8* getBuffer() const
    {
        return mpBuffer;
    }

    bool isEmpty() const
    {
        return mValidSize == 0;
    }

    void setBuffer(u8* buffer, size_t size)
    {
        setBuffer_(buffer, size, false);
    }

    void clear();

    bool beginDisplayList();
    size_t endDisplayList();

    bool beginDisplayListBuffer(u8* buffer, size_t size);
    size_t endDisplayListBuffer();

    bool beginDisplayListTemporary(size_t size);
    size_t endDisplayListTemporary();

    void call() const
    {
#if RIO_IS_CAFE
        if (!isEmpty())
            GX2CallDisplayList(mpBuffer, mValidSize);
#endif // RIO_IS_CAFE
    }

    void directCall() const
    {
#if RIO_IS_CAFE
        if (!isEmpty())
            GX2DirectCallDisplayList(mpBuffer, mValidSize);
#endif // RIO_IS_CAFE
    }

    static size_t suspend(void** p_dl);
    static void resume(void* dl, size_t size);

private:
    void setBuffer_(u8* buffer, size_t size, bool self_alloc);

private:
    u8* mpBuffer;
    size_t mSize;
    size_t mValidSize;

    // Custom
    bool mSelfAlloc;
};
//static_assert(sizeof(DisplayList) == 0x10, "agl::DisplayList size mismatch");

}
