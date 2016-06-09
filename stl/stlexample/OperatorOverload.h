#ifndef __OperatorOverload_h__
#define __OperatorOverload_h__
using namespace std;
template <class Type> class average :binary_function<Type, Type, Type>
{
private:
	Type m_data;
	typedef struct
	{
		first_argument_type m_firstArg;
		second_argument_type m_secondArg;
	} Arguments;
	Arguments arguments;

public:
    average(Type data)
    {
        m_data = data;
    }
    result_type operator( ) (first_argument_type a,
        second_argument_type b)
    {
		arguments.m_firstArg = a;
		arguments.m_secondArg = b;
        return (result_type)((a + b) / 2);
    }
	//type cast operator, cast average to result_type
	operator result_type ()
	{
		reutn m_data;
	}
	//member access operator
	Arguments* operator ->()
	{
		return &arguments;
	}
};
#endif