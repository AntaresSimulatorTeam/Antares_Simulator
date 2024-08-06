#pragma once

#include <mutex>
#include <vector>

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
        std::lock_guard<std::mutex> lock(mutex_); //  Add the object to the registry

        registry_.push_back(std::make_unique<Derived>(std::forward<Args>(args)...));
        return registry_.back().get(); //  Return the pointer to the newly created object
    }

private:
    std::vector<std::unique_ptr<Base>>
      registry_; //  Registry to manage dynamically allocated objects
    std::mutex mutex_;
};
