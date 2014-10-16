#ifndef SERVICEREGISTRY_H
#define SERVICEREGISTRY_H

#include  <assert.h>
#include  "hashstring.h"
#include  "servicecounter.h"
#include  "service.h"
#include  "locator.h"

namespace venk
{
/**
  * Serivce Registry object in charge of tracking,
  * intialising, and shutting down all services
  * resgistered with it.
  * eg usage
  * ServiceRegistry<FontService>::initialise();
  * ..
  * ServiceRegistry<FontService>::shutdown()
  * A service type must define a name, status,
  * initialise, and shutdown function.
  */

template <class T>
class ServiceRegistry
{
public:

    static bool initialise(void)
    {
        // ask the service to allocate and intialise itself
        Service<T> *instance = Service<T>::create();
        // failing to initalise will be a serious problem
        assert(instance != NULL);
        // register it by adding it to our tables
        Locator::servicePtrs[Service<T>::serviceName] = static_cast<void *>(instance); // Pointer to service object
        return (instance != NULL);
    }

    static void shutdown(void)
    {
        SDL_assert(Service<T>::instance != NULL);
        SDL_assert(Locator::servicePtrs.find(Service<T>::serviceName) != Locator::servicePtrs.end());
        // ask the service to shut itself, delete itself and null the instance
        Service<T>::destroy();
        // delete it
        Locator::servicePtrs[Service<T>::serviceName] = NULL;
    }
};
}
#endif
