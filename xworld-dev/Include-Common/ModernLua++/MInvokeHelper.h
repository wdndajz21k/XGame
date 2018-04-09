#pragma once
#include "LuaPush.h"
#include "MPureTypeHelper.h"
#include <tuple>
#include <utility>

namespace mLua
{
    class IMethodHolder
    {
    public:
        virtual ~IMethodHolder() = default;
        virtual void Invoke(lua_State *L, void *objThis) = 0;
    };

    template<typename ClassT, typename FuncT, typename T, std::size_t... I>
    inline auto ThisInvokeB(ClassT *p, FuncT f, const T& t, std::integer_sequence<std::size_t, I...>) -> decltype((p->*f)(std::get<I>(t)...))
    {
        return (p->*f)(std::get<I>(t)...);
    }

    template<typename ClassT, typename FuncT, typename T>
    inline auto ThisInvokeA(ClassT *p, FuncT f, const T& t) -> decltype(ThisInvokeB(p, f, t, std::make_index_sequence<std::tuple_size<T>::value>()))
    {
        return ThisInvokeB(p, f, t, std::make_integer_sequence<std::size_t, std::tuple_size<T>::value>());
    }

    template<typename FuncT, typename T, std::size_t... I>
    inline auto InvokeB(FuncT f, const T& t, std::integer_sequence<std::size_t, I...>) -> decltype(f(std::get<I>(t)...))
    {
        return f(std::get<I>(t)...);
    }

    template<typename FuncT, typename T>
    inline auto InvokeA(FuncT f, const T& t) -> decltype(InvokeB(f, t, std::make_index_sequence<std::tuple_size<T>::value>()))
    {
        return InvokeB(f, t, std::make_index_sequence<std::tuple_size<T>::value>());
    }

    template <int N, typename T>
    inline void FillParams(lua_State *L, T& t)
    {
    }

    template <int N, typename T, typename Head, typename ... Tail>
    inline void FillParams(lua_State *L, T& t)
    {
        std::get<N>(t) = M_GetLuaValue<Head>(L, N + 1);
        FillParams<N + 1, T, Tail...>(L, t);
    }
    
    template <typename FuncT>
    class MethodHolder : public IMethodHolder
    {
        MethodHolder() = delete;
        MethodHolder(FuncT) = delete;
    };

    template <typename ClassT, typename RetT, typename ... ArgsT>
    class MethodHolder<RetT(ClassT::*)(ArgsT...)> : public IMethodHolder
    {
        typedef RetT(ClassT::*FuncType)(ArgsT...);
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        MethodHolder(FuncType f) : TargetFunc(f) { }
        virtual void Invoke(lua_State *L, void *objThis) override
        {
            TupleType t;
            FillParams<0, TupleType, ArgsT...>(L, t);
            RetT retValue = ThisInvokeA((ClassT*)objThis, TargetFunc, t);
            M_Push(L, retValue);
        }
    };

    template <typename ClassT, typename RetT, typename ... ArgsT>
    class MethodHolder<RetT(ClassT::*)(ArgsT...) const> : public IMethodHolder
    {
        typedef RetT(ClassT::*FuncType)(ArgsT...) const;
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        MethodHolder(FuncType f) : TargetFunc(f) { }
        virtual void Invoke(lua_State *L, void *objThis) override
        {
            TupleType t;
            FillParams<0, TupleType, ArgsT...>(L, t);
            RetT retValue = ThisInvokeA((ClassT*)objThis, TargetFunc, t);
            M_Push(L, retValue);
        }
    };

    template <typename ClassT, typename ... ArgsT>
    class MethodHolder<void(ClassT::*)(ArgsT...)> : public IMethodHolder
    {
        typedef void(ClassT::*FuncType)(ArgsT...);
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        MethodHolder(FuncType f) : TargetFunc(f) { }
        virtual void Invoke(lua_State *L, void *objThis) override
        {
            TupleType t;
            FillParams<0, TupleType, ArgsT...>(L, t);
            ThisInvokeA((ClassT*)objThis, TargetFunc, t);
        }
    };

    template <typename RetT, typename ... ArgsT>
    class MethodHolder<RetT(*)(ArgsT...)> : public IMethodHolder
    {
        typedef RetT(*FuncType)(ArgsT...);
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        MethodHolder(FuncType f) : TargetFunc(f) { }
        virtual void Invoke(lua_State *L, void *objThis) override
        {
            TupleType t;
            FillParams<0, TupleType, ArgsT...>(L, t);
            RetT retValue = InvokeA(TargetFunc, t);
            M_Push(L, retValue);
        }
    };

    template <typename ... ArgsT>
    class MethodHolder<void(*)(ArgsT...)> : public IMethodHolder
    {
        typedef void(*FuncType)(ArgsT...);
        typedef std::tuple<ArgsT...> TupleType;
        FuncType TargetFunc;
    public:
        MethodHolder(FuncType f) : TargetFunc(f) { }
        virtual void Invoke(lua_State *L, void *objThis) override
        {
            TupleType t;
            FillParams<0, TupleType, ArgsT...>(L, t);
            InvokeA(TargetFunc, t);
        }
    };

    template <typename FuncType>
    IMethodHolder* MakeMethodHolder(FuncType f)
    {
        return new MethodHolder<FuncType>(f);
    }
}
