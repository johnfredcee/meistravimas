#ifndef OBSERVER_H
#define OBSERVER_H

namespace venk
{
	template <typename Observed> class Observable
	{
  private:
		typedef  std::function<bool(SDL_Event* e)> Observer;
		std::vector< Observer > observers;
  public:
		void addObserver(Observer&& observer) {
			observers.push_back(observer);
		}
		void notify(SDL_Event* e) {
			for (const auto& obs : observers) obs(e);
		}
	};
}

#endif
