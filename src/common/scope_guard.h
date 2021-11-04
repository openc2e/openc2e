#pragma once

template <typename T>
class scope_guard final {
  public:
	explicit scope_guard(T t_)
		: active(true), t(std::move(t_)) {}
	scope_guard(const scope_guard&) = delete;
	scope_guard(scope_guard&& other)
		: active(true), t(other.t) {
		other.active = false;
	}
	scope_guard& operator=(const scope_guard&) = delete;
	scope_guard& operator=(scope_guard&&) = delete;
	~scope_guard() {
		if (active) {
			t();
		}
	}

  private:
	bool active = false;
	T t;
};

template <typename T>
scope_guard<T> make_scope_guard(T&& t) {
	return scope_guard<T>(std::forward<T>(t));
}