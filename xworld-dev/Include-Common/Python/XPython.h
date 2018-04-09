#pragma once
#include "Python/Python.h"

class XPython
{
public:
    static inline void TupleSetItem(PyObject * tp, int index, PyObject * v)
    {
        PyTuple_SetItem(tp, index, v);
    }

    static inline void TupleSetItem(PyObject * tp, int index, bool v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("O", v ? Py_True : Py_False));
    }

    static inline void TupleSetItem(PyObject * tp, int index, int v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("i", v));
    }

    static inline void TupleSetItem(PyObject * tp, int index, size_t v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("l", v));
    }

    static inline void TupleSetItem(PyObject * tp, int index, float v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("f", v));
    }

    static inline void TupleSetItem(PyObject * tp, int index, double v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("d", v));
    }

    static inline void TupleSetItem(PyObject * tp, int index, const char * v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("s", v));
    }

    static inline void TupleSetItem(PyObject * tp, int index, const std::string & v)
    {
        PyTuple_SetItem(tp, index, Py_BuildValue("s", v.c_str()));
    }

    inline static void TuplePush(PyObject *argsObj, int index)
    {
    }

    template <typename First, typename ... Rest>
    inline static void TuplePush(PyObject *argsObj, int index, First first, Rest ... rests)
    {
        TupleSetItem(argsObj, index, first);
        TuplePush(argsObj, index + 1, rests...);
    }

    template <typename ... Args>
    inline static PyObject* Invoke(const char *moduleName, const char *funcName, Args ... args)
    {
        PyObject* result = nullptr;
        PyObject* func = nullptr;
        PyObject *argsObj = nullptr;
        PyObject* module = PyImport_ImportModule(moduleName);
        XLOG_FAILED_JUMP(module);

        func = PyObject_GetAttrString(module, funcName);
        XLOG_FAILED_JUMP(func);

        if (sizeof...(args) > 0)
        {
            argsObj = PyTuple_New(sizeof...(args));
            TuplePush(argsObj, 0, args...);
        }

        result = PyObject_CallObject(func, argsObj);

    Exit0:
        if (!result)
            PyErr_Print();

        Py_CLEAR(argsObj);
        Py_CLEAR(module);
        Py_CLEAR(func);

        return result;
    }

    template <typename ... Args>
    inline static void GEventSpawn(const char *moduleName, const char *funcName, Args ... args)
    {
        PyObject* result = nullptr;
        PyObject* func = nullptr;
        PyObject* module = PyImport_ImportModule(moduleName);
        XLOG_FAILED_JUMP(module);

        func = PyObject_GetAttrString(module, funcName);
        XLOG_FAILED_JUMP(func);

        result = Invoke("gevent", "spawn", func, args...);

    Exit0:
        ;

        if (!result)
            Py_CLEAR(func); //成功的话，func作为参数，在Invoke里已经被释放了

        Py_CLEAR(result);
        Py_CLEAR(module);
    }
};

template <typename ... Args>
inline static PyObject* PyInvoke(const char *moduleName, const char *funcName, Args ... args)
{
    return XPython::Invoke(moduleName, funcName, args...);
}

template <typename ... Args>
inline static void GEventSpawn(const char *moduleName, const char *funcName, Args ... args)
{
    XPython::GEventSpawn(moduleName, funcName, args...);
}
