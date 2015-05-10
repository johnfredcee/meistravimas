#ifndef SERVICE_H
#define SERVICE_H


namespace venk
{

	enum ServiceStatus
	{
		NOT_READY,
		IN_USE,
		CHECKED_IN,
		AVAILABLE
	};

	template<class Q> class ServiceRegistry;
	template<class V> class ServiceRegistry;

	/** Service singleton. All services must derive from this to be
	 * accessible through the locator. */
	template<class T> class Service {
			friend class Locator;
			template <class Q> friend class ServiceRegistry;
			template <class V> friend class ServiceCheckout;
		protected:
			Service() {};
			virtual ~Service() {}

		private:
			static T *create()
			{
				/** if this fires, it has already been created */
				SDL_assert(T::serviceName == ServiceCounter::NO_SERVICE);
				if (!instance)	{
						instance = new T();
						T::initialise(instance);
						SDL_assert(instance != NULL);
						ServiceCounter::serviceCount++;
						T::serviceName = ServiceCounter::serviceCount;
#ifndef NDEBUG						
						T::useCount = 0;
#endif						
					}

				return instance;
			}

			static void destroy()
			{
#ifndef NDEBUG				
				SDL_assert(useCount == 0);
#endif				
				T::shutdown(instance);
				if (instance) {
					delete instance;
					instance = 0;
				}
			}

			// unique Id of our service used to locate it
			static Locator::ServiceName_t serviceName;

			// 
			static T			*instance;
#ifndef NDEBUG
			static Uint32        useCount;
#endif
	};


	// Variable initialisation for Service singleton:
	template<class T>
		T *Service<T>::instance = NULL;

	template<class T>
		Locator::ServiceName_t Service<T>::serviceName = ServiceCounter::NO_SERVICE;

#ifndef NDEBUG
	template<class T>
		Uint32 Service<T>::useCount  = 0;
#endif

}

#endif
