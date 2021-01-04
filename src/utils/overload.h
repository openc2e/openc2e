#pragma once

namespace detail {
	template <class... Fs>
	struct overloader;

	template <class F0, class... Frest>
	struct overloader<F0, Frest...> : F0, overloader<Frest...>
	{
		overloader(F0 f0, Frest... rest) : F0(f0), overloader<Frest...>(rest...) {}
		using F0::operator();
		using overloader<Frest...>::operator();
	};

	template <class F0>
	struct overloader<F0> : F0
	{
		overloader(F0 f0) : F0(f0) {}
		using F0::operator();
	};
} // namespace detail

template <class... Fs>
auto overload(Fs... fs)
{
	return detail::overloader<Fs...>(fs...);
}