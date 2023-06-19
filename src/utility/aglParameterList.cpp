#include <utility/aglParameter.h>
#include <utility/aglParameterList.h>
#include <utility/aglParameterObj.h>

namespace agl { namespace utl {

IParameterList::IParameterList()
    : _70(0)
{
    mChildList.initOffset(offsetof(IParameterList, mListNode));
    mChildObj.initOffset(offsetof(IParameterObj, mListNode));

    setParameterListName_("");
}

void IParameterList::setParameterListName_(const char* name)
{
    mName = name;
    mNameHash = ParameterBase::calcHash(name);
}

void IParameterList::addObj(IParameterObj* child, const char* name)
{
    RIO_ASSERT(child != nullptr);
    child->mName = name;
    child->mNameHash = ParameterBase::calcHash(child->mName.c_str());

    mChildObj.pushBack(child);
}

void IParameterList::applyResParameterList(ResParameterList list)
{
    applyResParameterList_(list);
}

void IParameterList::applyResParameterList_(ResParameterList list, bool lerp, f32 t)
{
    if (!preRead_())
        return;

    for (u32 i = 0; i < list.getResParameterObjNum(); i++)
    {
        ResParameterObj child_obj = list.getResParameterObj(i);

        for (OffsetList<IParameterObj>::iterator itr = mChildObj.begin(), itr_end = mChildObj.end(); itr != itr_end; ++itr)
        {
            if (itr->isApply_(child_obj))
            {
                if (lerp)
                    itr->applyResParameterObj_(child_obj, true, t, this);

                else
                    itr->applyResParameterObj_(child_obj, false, 1.0f, this);

                break;
            }
        }
    }

    for (u32 i = 0; i < list.getResParameterListNum(); i++)
    {
        ResParameterList child_list = list.getResParameterList(i);

        for (OffsetList<IParameterList>::iterator itr = mChildList.begin(), itr_end = mChildList.end(); itr != itr_end; ++itr)
        {
            if (itr->isApply_(child_list))
            {
                if (lerp)
                    itr->applyResParameterListB_(child_list, t);

                else
                    itr->applyResParameterList_(child_list);

                break;
            }
        }
    }

    postRead_();
}

void IParameterList::applyResParameterListB_(ResParameterList list, f32 t)
{
    applyResParameterList_(list, true, t);
}

} }
