#ifndef __VirutalInheritance_H__
#define __VirutalInheritance_H__
class Animal {
public:
    virtual void eat()
    {
        printf("%s\r\n", __FUNCTION__);
    };
    int data;
};

// Two classes virtually inheriting Animal:, without virtual inhertance Bat.eat will be ambiguous 
// since it inherits two copies of functions and members from Mammal and WingedAnimal
class Mammal : public virtual Animal {
public:
    virtual void breathe(){};
};

class WingedAnimal : public virtual Animal {
public:
    virtual void flap(){};
};

// A bat is still a winged mammal
class Bat : public Mammal, public WingedAnimal {
};
#endif