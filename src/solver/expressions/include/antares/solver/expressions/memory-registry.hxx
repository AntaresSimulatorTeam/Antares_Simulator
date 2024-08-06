#pragma once

#include <vector>

//  Template class to manage a registry of pointers to dynamically allocated objects
template<class T>
struct Registry
{
    //  Method to add a pointer to the registry
    void add(T* x)
    {
        mem_.push_back(x);
    }

    //  Destructor to delete all objects in the registry
    ~Registry()
    {
        for (T* x: mem_)
        {
            delete x;
        }
    }

private:
    std::vector<T*> mem_; //  Vector to store pointers to dynamically allocated objects
};

//  Template class to manage the memory allocation and registry for a base class
template<class Base>
class MemoryManager
{
public:
    //  Method to create a new derived class object and add it to the registry
    template<class Derived, class... Args>
    requires std::derived_from<Derived, Base>
    Base* create(Args&&... args)
    {
        Base* x = new Derived(
          std::forward<Args>(args)...); //  Dynamically allocate a new derived class object
        registry_.add(x);               //  Add the object to the registry
        return x;                       //  Return the pointer to the newly created object
    }

private:
    Registry<Base> registry_; //  Registry to manage dynamically allocated objects
};
