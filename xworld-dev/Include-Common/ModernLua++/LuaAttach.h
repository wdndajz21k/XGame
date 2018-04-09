#pragma once
#include <unordered_set>

#define LUA_ATTACH_CHECK_CODE 0x1A2B3C4D
namespace mLua
{
    class LuaAttachBase
    {
    public:
        LuaAttachBase() = default;
        LuaAttachBase* operator=(const LuaAttachBase&) = delete;
        LuaAttachBase(const LuaAttachBase&) { };
        virtual ~LuaAttachBase() {};
        virtual void AddRef(LuaUserDataType *ud) = 0;
        virtual void DedRef(LuaUserDataType *ud) = 0;

    public:
        unsigned int CheckCode = LUA_ATTACH_CHECK_CODE;

    protected:
        std::unordered_set<LuaUserDataType*> RefSet;
    };

    template <typename ClassT>
    class LuaAttach : public LuaAttachBase
    {
    public:
        virtual ~LuaAttach()
        {
            for (auto& ud : RefSet)
            {
                ud->ObjectInstance = NULL;
            }
            RefSet.clear();
        }

        virtual void AddRef(mLua::LuaUserDataType *ud) override
        {
            assert(RefSet.find(ud) == RefSet.end());
            RefSet.insert(ud);
        }

        virtual void DedRef(mLua::LuaUserDataType *ud) override
        {
            assert(RefSet.find(ud) != RefSet.end());
            RefSet.erase(ud);
            ClassT *p = (ClassT*)ud->ObjectInstance;
            if (p->__CreatedByLua && RefSet.empty())
            {
                delete p;
                ud->ObjectInstance = NULL;
            }
        }
    };
}
