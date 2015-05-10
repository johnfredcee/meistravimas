#ifndef SERVICECHECKOUT_H
#define SERVICECHECKOUT_H


namespace venk
{
    /**
     * Service Checkout object ensures exclusive access to some service
     * usage eg:
     * {
     *  ServiceCheckout<FontSystem> fontSystem;
     *  Font *newFont = fontSystem->makeFont("font.tga");
     * // do stuff
     * }
     * // service now checked in
     */
    template <typename T>
		class ServiceCheckout 		
		{
			friend class Locator;
		public:
			// Construct this object to checkout a service
			ServiceCheckout() {
				SDL_assert(Locator::servicePtrs.find(T::serviceName) != Locator::servicePtrs.end());
				instance = Service<T>::instance;
#ifndef NDEBUG				
				instance->useCount++;
#endif				
				status = IN_USE;
			}
	
			// Get an instance of the service from the checkout object
			T* operator->() const {
				SDL_assert(instance != nullptr);
				SDL_assert(status == IN_USE);
				return instance;
			}
		
			// destructor ensures checkout when we go out of scope
			~ServiceCheckout<T>() {
				if (status != CHECKED_IN) {
#ifndef NDEBUG
					instance->useCount--;
#endif				  
					status = CHECKED_IN;
				}
			}
	
		private:
			/** no assignment or copy construction */
			ServiceCheckout(ServiceCheckout& other) {};
			ServiceCheckout& operator=(ServiceCheckout& other) {};
			ServiceStatus status;
			static T* instance;
		};

    template <class T>
		T* ServiceCheckout<T>::instance = NULL;

}
#endif
