#ifndef GLWR_PTR_H
#define GLWR_PTR_H

template <typename T>
class GLWRPtr
{
    T* m_ptr;

public:
    GLWRPtr()
        : m_ptr(nullptr)
    {
    }
    ~GLWRPtr()
    {
        delete m_ptr;
    }
    T** operator&()
    {
        return &m_ptr;
    }
    T* Get()
    {
        return m_ptr;
    }
};

#endif
