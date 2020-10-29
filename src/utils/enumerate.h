#pragma once

template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          // typename TValue = decltype(*std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
auto enumerate(T && iterable)
{
    struct value_type
    {
        size_t i;
        TIter iter;
        auto operator->() { return iter.operator->(); }
        auto value() { return *iter; }
    };
    struct iterator
    {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return value_type{i, iter}; }
    };
    struct iterable_wrapper
    {
        T iterable;
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}