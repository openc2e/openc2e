#pragma once

template <typename T>
class Singleton {
  public:
	static std::shared_ptr<T> getInstance() {
		static std::weak_ptr<T> weak;
		std::shared_ptr<T> instance = weak.lock();
		if (!instance) {
			weak = instance = std::shared_ptr<T>(new T);
		}
		return instance;
	}
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

  protected:
	Singleton() = default;
};
