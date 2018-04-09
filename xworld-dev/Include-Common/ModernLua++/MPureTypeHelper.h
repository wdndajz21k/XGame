#pragma once

namespace mLua
{
    template<typename T>
    struct M_REMOVE_CONST
    {
        typedef T type;
    };

    template<typename T>
    struct M_REMOVE_CONST < const T >
    {
        typedef T type;
    };

    template<typename T>
    struct M_REMOVE_REFERENCE
    {
        typedef T type;
    };

    template<typename T>
    struct M_REMOVE_REFERENCE < T& >
    {
        typedef T type;
    };

    template<typename T>
    struct M_REMOVE_POINTER
    {
        typedef T type;
    };

    template<typename T>
    struct M_REMOVE_POINTER < T* >
    {
        typedef T type;
    };

    // remove_const_referance_pointer
    template<typename T>
    struct M_GET_PURE_TYPE
    {
        typedef typename M_REMOVE_POINTER<typename M_REMOVE_REFERENCE<typename M_REMOVE_CONST<T>::type>::type>::type type;
    };

    template <typename T1, typename T2>
    struct M_Translate
    {
        static T2 GetValue(void *pObj)	{ assert(false); }
    };

    template <typename T1>
    struct M_Translate < T1, T1* >
    {
        static T1* GetValue(void *pObj)
        {
            return (T1*)pObj;
        }
    };

    template <typename T1>
    struct M_Translate < T1, T1& >
    {
        static T1& GetValue(void *pObj)
        {
            return *(T1*)pObj;
        }
    };
}
