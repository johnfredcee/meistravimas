#ifndef LOCATOR_H
#define LOCATOR_H


namespace venk
{
/**
 * This file describes the mechanics of a service singleton. A service singleton
 * is a classic singleton that is intialised and destroyed when registered and
 * unregistered with a registry which ensures there is only one copy of it.
 * Behind the scenes it is managed by a Locator used to find the single pointer
 * to it's instance and ensure only one copy of it is extant at any one time.
 * The user obtains access to the instance by constructing a service checkout
 * object which automatically checks it back in when it goes out of scope.
 */

template <class T>
class ServiceRegistry;

template <class T>
class ServiceCheckout;

// the humble beginnings of a serivce locator singleton
class Locator
{
    template <typename T>
    friend class ServiceRegistry;
    template <typename T>
    friend class ServiceCheckout;

public:
    typedef Uint32 ServiceName_t;

private:
    typedef std::unordered_map<Locator::ServiceName_t, void *>          ServicePtrDict_t;
    static ServicePtrDict_t                        servicePtrs;
};

}
#endif
