#pragma once

namespace agl {

inline void
IndexStream::draw() const
{
    drawInstanced(1);
}

inline void
IndexStream::draw(u32 start, u32 count) const
{
    drawInstanced(start, count, 1);
}

inline void
IndexStream::drawInstanced(u32 instance_num) const
{
    drawInstanced(0, getCount(), instance_num);
}

inline void
IndexStream::drawInstanced(u32 start, u32 count, u32 instance_num) const
{
    if (count > 0)
    {
        RIO_ASSERT(( start + count ) <= getCount());
#if RIO_IS_CAFE
        GX2DrawIndexedEx(static_cast<GX2PrimitiveMode>(mPrimitiveType), count, static_cast<GX2IndexType>(mFormat), getBufferPtr(start), 0, instance_num);
#elif RIO_IS_DESKTOP
        RIO_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandle));
        RIO_GL_CALL(glDrawElementsInstanced(mPrimitiveType, count, mFormat == cIndexStreamFormat_u16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, getBufferPtr(start), instance_num));
#endif // RIO_IS_DESKTOP
    }
}

}
