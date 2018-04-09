#pragma once
#include "MC2LType.h"
#include "MGetLuaValue.h"
#include "MInvokeHelper.h"
#include "LuaAttach.h"
#include "MVariableHelper.h"
#include <stddef.h>
#include <unordered_map>

#define M_COUNT_OF(CLASS, FIELD) &(((CLASS*)0)->FIELD)

#define DECLARE_LUA_CLASS(CLASSNAME)                                         \
    private:                                                                 \
        static mLua::SLEClassPreRegister __SLERegister;                      \
    public:                                                                  \
        mLua::LuaAttach<CLASSNAME> __PointerProtector;                       \
        bool __CreatedByLua = false;                                         \
        typedef CLASSNAME TYPE_CLASS;                                        \
        const char *M_GetClassName() const                                   \
        {                                                                    \
            return #CLASSNAME;                                               \
        }                                                                    \
        static TYPE_CLASS* M_S_New()                                         \
        {                                                                    \
            TYPE_CLASS *p = new TYPE_CLASS;                                  \
            p->__CreatedByLua = true;                                        \
            return p;                                                        \
        }                                                                    \
        static const char* M_S_GetClassName()                                \
        {                                                                    \
            return #CLASSNAME;                                               \
        }                                                                    \
        static mLua::LuaAttachBase* M_S_GetPointerProtector(void *obj)       \
        {                                                                    \
            return &(((TYPE_CLASS*)obj)->__PointerProtector);                \
        }                                                                    \
        static void M_S_Init(lua_State *L, const char *className = nullptr); \
    private:

#define BEGIN_DECLARE_SCRIPT(CLASSNAME) \
    mLua::SLEClassPreRegister CLASSNAME::__SLERegister(#CLASSNAME, CLASSNAME::M_S_Init, (ptrdiff_t)M_COUNT_OF(CLASSNAME, __PointerProtector)); \
    void CLASSNAME::M_S_Init(lua_State *L, const char *className) \
    { \
        if (!className) \
            className = M_S_GetClassName();

#define END_DECLARE_SCRIPT() }

#define LUA_FUNC(NAME, ...)                 mLua::RegisterMemberFunc(L, className, #NAME, &TYPE_CLASS::NAME, (""#__VA_ARGS__)[0] == '\0' ? -1 : __VA_ARGS__ + 0)
#define LUA_STATIC_FUNC(NAME, ...)          mLua::RegisterStaticFunc(L, className, #NAME, &NAME, (""#__VA_ARGS__)[0] == '\0' ? -1 : __VA_ARGS__ + 0)
#define LUA_VAR(NAME)                       mLua::RegisterVar(L, className, #NAME, &((TYPE_CLASS*)0)->NAME,     false, false)
#define LUA_STATIC_VAR(NAME)                mLua::RegisterVar(L, className, #NAME, &NAME,                       true,  false)
#define LUA_RVAR(NAME)                      mLua::RegisterVar(L, className, #NAME, &((TYPE_CLASS*)0)->NAME,     false, true)
#define LUA_RSTATIC_VAR(NAME)               mLua::RegisterVar(L, className, #NAME, &NAME,                       true,  true)
#define LUA_CONSTRUCTOR                     LUA_STATIC_FUNC(M_S_New)
#define LUA_DERIVED(PARENT)                 PARENT::M_S_Init(L, className)

#define MRET std::make_tuple

namespace mLua
{
    class IMethodHolder;

    enum class LUA_REGISTER_TYPE
    {
        Function,
        Variable,
    };

    struct RegLuaInfoBase
    {
        LUA_REGISTER_TYPE RegType;
        const char *ClassName;
        const char *Name;
        bool IsStatic;
    };

    struct RegLuaFuncInfo : RegLuaInfoBase
    {
        int ParamsType[SLE_MAX_PARAMS_COUNT];
        int MinArgCount;
        int MaxArgCount;
        IMethodHolder *Method;
    };

    struct RegLuaVarInfo : RegLuaInfoBase
    {
        ptrdiff_t Address;
        void(*ReadMethod)(lua_State*, intptr_t, ptrdiff_t);
        bool(*WriteMethod)(lua_State*, intptr_t, ptrdiff_t, int);
        bool Readonly;
    };

    class SLEClassPreRegister
    {
    public:
        typedef void(*M_InitClassFunc)(lua_State *L, const char *className);
        struct SLEClassPreRegisterInfo
        {
            M_InitClassFunc M_InitFunc;
            ptrdiff_t ProtectorOffset;
        };
        typedef std::unordered_map<std::string, SLEClassPreRegisterInfo> CLASS_REGISTER_MAP;

    public:
        SLEClassPreRegister(const char *className, M_InitClassFunc initFunc, ptrdiff_t protectorOffset);

    public:
        static CLASS_REGISTER_MAP* SLEClassSet;
    };

    M_API RegLuaInfoBase* RegisterLuaElement(lua_State *L, RegLuaInfoBase *regInfo, int size);
    M_API void RegisterLuaClass(lua_State *L, const char *className, int protectorOffset);
    M_API void SLERegisterAllLuaClass(lua_State *L);
    M_API RegLuaInfoBase* QueryLuaElement(lua_State *L, const char *className, const char *field);

    template<typename... T>
    struct FetchCFuncParamsType
    {
        static void Fetch(RegLuaFuncInfo *funcInfo, int n = 0) { }
    };

    template<typename Head, typename... Tail>
    struct FetchCFuncParamsType < Head, Tail... >
    {
        static void Fetch(RegLuaFuncInfo *funcInfo, int n = 0)
        {
            int luaType = CType2LuaType<Head>();
            funcInfo->ParamsType[n] = luaType;
            FetchCFuncParamsType<Tail...>::Fetch(funcInfo, n + 1);
        }
    };

    template<>
    struct FetchCFuncParamsType < >
    {
        static void Fetch(RegLuaFuncInfo *funcInfo, int n = 0) { }
    };

    template <typename VarType>
    inline RegLuaVarInfo* RegisterVar(lua_State *L, const char *className, const char *varName, VarType *addr, bool isStatic, bool readonly)
    {
        RegLuaVarInfo varInfo;
        varInfo.RegType = LUA_REGISTER_TYPE::Variable;
        varInfo.ClassName = className;
        varInfo.Name = varName;
        varInfo.Address = (ptrdiff_t)addr;
        varInfo.IsStatic = isStatic;
        varInfo.Readonly = readonly;
        varInfo.ReadMethod = CVarRead<VarType>::Read;
        varInfo.WriteMethod = CVarWrite<VarType>::Write;
        return (RegLuaVarInfo*)RegisterLuaElement(L, &varInfo, sizeof(varInfo));
    }

    template <typename FuncType>
    inline RegLuaFuncInfo* RegisterFunc(lua_State *L, const char *className, const char *funcName, bool isStatic, FuncType func, int minArgCount, int maxArgCount)
    {
        RegLuaFuncInfo funcInfo;
        funcInfo.RegType = LUA_REGISTER_TYPE::Function;
        funcInfo.ClassName = className;
        funcInfo.Name = funcName;
        funcInfo.MinArgCount = minArgCount;
        funcInfo.MaxArgCount = maxArgCount;
        funcInfo.IsStatic = isStatic;
        funcInfo.Method = new MethodHolder<FuncType>(func);
        return (RegLuaFuncInfo*)RegisterLuaElement(L, &funcInfo, sizeof(funcInfo));
    }

    template <typename RET, typename CLASS, typename ... ArgsType>
    inline void RegisterMemberFunc(lua_State *L, const char *pszClassName, const char *pszName, RET(CLASS::*pFunc)(ArgsType...), int minArg = -1)
    {
        int argCount = sizeof...(ArgsType);
        assert(minArg < argCount);
        RegLuaFuncInfo *funcInfo = RegisterFunc(L, pszClassName, pszName, false, pFunc, minArg == -1 ? argCount : minArg, argCount);
        FetchCFuncParamsType<ArgsType...>::Fetch(funcInfo);
    }

    template <typename RET, typename CLASS, typename ... ArgsType>
    inline void RegisterMemberFunc(lua_State *L, const char *pszClassName, const char *pszName, RET(CLASS::*pFunc)(ArgsType...) const, int minArg = -1)
    {
        int argCount = sizeof...(ArgsType);
        RegLuaFuncInfo *funcInfo = RegisterFunc(L, pszClassName, pszName, false, pFunc, minArg == -1 ? argCount : minArg, argCount);
        FetchCFuncParamsType<ArgsType...>::Fetch(funcInfo);
    }

    template <typename RET, typename ... ArgsType>
    inline void RegisterStaticFunc(lua_State *L, const char *pszClassName, const char *pszName, RET(*pFunc)(ArgsType...), int minArg = -1)
    {
        int argCount = sizeof...(ArgsType);
        RegLuaFuncInfo *funcInfo = RegisterFunc(L, pszClassName, pszName, true, pFunc, minArg == -1 ? argCount : minArg, argCount);
        FetchCFuncParamsType<ArgsType...>::Fetch(funcInfo);
    }
}
