#ifndef OBSERVER_H
#define OBSERVER_H

namespace venk
{
	template <typename Observed> class Observable
	{
  private:
		typedef std::function<bool(SDL_Event* e)> Observer;
		std::unordered_map< int, Observer > observers;
  public:
		int addObserver(Observer&& observer) {
			static int channel = 1;
			observers.emplace(channel, observer);
			channel++;
			return channel-1;
		}
		void removeObserver(int channel)
		{
			observers.erase(channel);
		}
		void notify(SDL_Event* e) {
			for (const auto& kv : observers) kv.second(e);
		}
	};
}

#endif
