#ifndef SERVICECOUNT_H
#define SERVICECOUNT_H


/*
 * This counts the number of distinct services we have in our system,
 * incremented when a new service is created so that we can give each
 * service a unique Id without resorting to enums
 */
namespace venk
{
	template<class T> class Service;

	class ServiceCounter
	{
		template<class T> friend class Service;
	private:
		static Uint32 serviceCount;
	public:
		static const Locator::ServiceName_t NO_SERVICE = 0;
	};
}

#endif
