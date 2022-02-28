#pragma once

#include <exception>
#include <stdio.h>
#include <string>
#include <type_traits>

// SimpleVariant
//
// A variant storage type that doesn't implement visit, and thus is simpler and
// faster to compile.

namespace SimpleVariantHelper {

template <typename U>
void construct(size_t idx, void* p) {
	if (idx == 0) {
		new (p) U();
	} else {
		// shouldn't happen
		abort();
	}
}

template <typename U, typename V, typename... Vs>
void construct(size_t idx, void* p) {
	if (idx == 0) {
		new (p) U();
	} else {
		return construct<V, Vs...>(idx - 1, p);
	}
}

template <typename U>
void copy_construct(size_t idx, void* new_p, const void* old_p) {
	if (idx == 0) {
		new (new_p) U(*reinterpret_cast<const U*>(old_p));
	} else {
		// shouldn't happen
		abort();
	}
}

template <typename U, typename V, typename... Vs>
void copy_construct(size_t idx, void* new_p, const void* old_p) {
	if (idx == 0) {
		new (new_p) U(*reinterpret_cast<const U*>(old_p));
	} else {
		return copy_construct<V, Vs...>(idx - 1, new_p, old_p);
	}
}


template <typename U>
void move_construct(size_t idx, void* new_p, void* old_p) {
	if (idx == 0) {
		new (new_p) U(std::move(*reinterpret_cast<U*>(old_p)));
	} else {
		// shouldn't happen
		abort();
	}
}

template <typename U, typename V, typename... Vs>
void move_construct(size_t idx, void* new_p, void* old_p) {
	if (idx == 0) {
		new (new_p) U(std::move(*reinterpret_cast<U*>(old_p)));
	} else {
		return move_construct<V, Vs...>(idx - 1, new_p, old_p);
	}
}

template <typename U>
void destroy(size_t idx, void* p) {
	if (idx == 0) {
		((U*)p)->~U();
	} else {
		// shouldn't happen
		abort();
	}
}

template <typename U, typename V, typename... Vs>
void destroy(size_t idx, void* p) {
	if (idx == 0) {
		((U*)p)->~U();
	} else {
		return destroy<V, Vs...>(idx - 1, p);
	}
}

template <typename T, typename... Ts>
struct IndexImpl;

template <typename T, typename... Ts>
struct IndexImpl<T, T, Ts...> {
	static constexpr size_t value = 0;
};

template <typename T, typename U, typename... Ts>
struct IndexImpl<T, U, Ts...> {
	static constexpr size_t value = 1 + IndexImpl<T, Ts...>::value;
};

template <typename T, typename... Ts>
static constexpr size_t Index = IndexImpl<T, Ts...>::value;

template <typename U, typename... Ts>
struct ContainedInImpl;

template <typename U>
struct ContainedInImpl<U> {};

template <typename U, typename... Ts>
struct ContainedInImpl<U, U, Ts...> {
	using type = void;
};

template <typename U, typename T, typename... Ts>
struct ContainedInImpl<U, T, Ts...>
	: ContainedInImpl<U, Ts...> {};

template <typename... Ts>
using ContainedIn = typename ContainedInImpl<Ts...>::type;

}; // namespace SimpleVariantHelper


template <typename... Ts>
class SimpleVariant {
  public:
	SimpleVariant() {
		init();
	}
	SimpleVariant(const SimpleVariant& other) {
		type_id = other.type_id;
		SimpleVariantHelper::copy_construct<Ts...>(other.type_id, &data, &other.data);
	}
	SimpleVariant& operator=(const SimpleVariant& other) {
		destroy();
		type_id = other.type_id;
		SimpleVariantHelper::copy_construct<Ts...>(other.type_id, &data, &other.data);
		return *this;
	};
	SimpleVariant(SimpleVariant&& other) {
		type_id = other.type_id;
		SimpleVariantHelper::move_construct<Ts...>(other.type_id, &data, &other.data);
		other.reset();
	};
	SimpleVariant& operator=(SimpleVariant&& other) {
		// TODO: if it's the same type, then do a move assignment
		destroy();
		type_id = other.type_id;
		SimpleVariantHelper::move_construct<Ts...>(other.type_id, &data, &other.data);
		other.reset();
		return *this;
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	SimpleVariant(const T& t) {
		type_id = SimpleVariantHelper::Index<T, Ts...>;
		new (&data) T(t);
	}
	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	SimpleVariant(T&& t) {
		type_id = SimpleVariantHelper::Index<T, Ts...>;
		new (&data) T(std::forward<T>(t));
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	SimpleVariant& operator=(const T& t) {
		destroy();
		type_id = SimpleVariantHelper::Index<T, Ts...>;
		new (&data) T(t);
		return *this;
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	SimpleVariant& operator=(T&& t) {
		destroy();
		type_id = SimpleVariantHelper::Index<T, Ts...>;
		new (&data) T(std::forward<T>(t));
		return *this;
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	bool has() const {
		return type_id == SimpleVariantHelper::Index<T, Ts...>;
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	void set(T&& t) {
		*this = t;
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	T& get() {
		if (!has<T>()) {
			std::terminate(); // TODO, throw exception?
		}
		return *reinterpret_cast<T*>(&data);
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	const T& get() const {
		if (!has<T>()) {
			std::terminate(); // TODO, throw exception?
		}
		return *reinterpret_cast<const T*>(&data);
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	T* get_if() {
		if (!has<T>()) {
			return nullptr;
		}
		return reinterpret_cast<T*>(&data);
	}

	template <typename T, typename = SimpleVariantHelper::ContainedIn<T, Ts...>>
	const T* get_if() const {
		if (!has<T>()) {
			return nullptr;
		}
		return reinterpret_cast<const T*>(&data);
	}

	~SimpleVariant() {
		destroy();
	}

  private:
	void init() {
		type_id = 0;
		SimpleVariantHelper::construct<Ts...>(0, &data);
	}
	void destroy() {
		SimpleVariantHelper::destroy<Ts...>(type_id, (void*)&data);
	}
	void reset() {
		destroy();
		init();
	}

	typename std::aligned_union_t<0, Ts...> data;
	size_t type_id;
};