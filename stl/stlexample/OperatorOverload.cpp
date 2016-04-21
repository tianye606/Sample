#include "stdafx.h"

template <class Type> class average :binary_function<Type, Type, Type>
{
public:
    average(Type data)
    {
        m_data = data;
    }
    result_type operator( ) (first_argument_type a,
        second_argument_type b)
    {
        return (result_type)((a + b) / 2);
    }
private:
    Type m_data;
};