#ifndef __MultiInheritance_H__
#define __MultiInheritance_H__
class MultiBase1
{
public:
	int data;
};
class MultiBase2
{
public:
	int data;
};
class MultiChild : public MultiBase1, public MultiBase2
{};
#endif