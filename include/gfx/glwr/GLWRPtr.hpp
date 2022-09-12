#ifndef GLWR_PTR_H
#define GLWR_PTR_H

#include "gfx/glwr/IGLWRBase.hpp"

template <typename T>
class GLWRPtr
{
    T* m_ptr;

public:
    GLWRPtr();
    ~GLWRPtr();
    T** operator&();
    T* Get();
};

template <typename T>
GLWRPtr<T>::GLWRPtr()
    : m_ptr(nullptr)
{
}

template <typename T>
GLWRPtr<T>::~GLWRPtr()
{
    static_cast<IGLWRBase*>(m_ptr)->Release();
}

template <typename T>
T** GLWRPtr<T>::operator&()
{
    return &m_ptr;
}

template <typename T>
T* GLWRPtr<T>::Get()
{
    return m_ptr;
}

#endif
