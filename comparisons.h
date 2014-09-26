#ifndef INCLUDED_COMPARISONS
#define INCLUDED_COMPARISONS

#ifndef INCLUDED_TYPE_TRAITS
#define INCLUDED_TYPE_TRAITS
#include <type_traits>
#endif

namespace range2 {

template<typename T>
struct GenerateDerivedComparisonOperations : std::false_type {};

// Scheme works as it is generating functions in the same namespace as the entity T.

template<typename T>
constexpr typename std::enable_if<GenerateDerivedComparisonOperations<T>::value, bool>::type
operator!=(T const& x, T const& y) { return !(x == y); }

template<typename T>
constexpr typename std::enable_if<GenerateDerivedComparisonOperations<T>::value, bool>::type
operator<=(T const& x, T const& y) { return !(y < x); }

template<typename T>
constexpr typename std::enable_if<GenerateDerivedComparisonOperations<T>::value, bool>::type
operator>(T const& x, T const& y) { return y < x; }

template<typename T>
constexpr typename std::enable_if<GenerateDerivedComparisonOperations<T>::value, bool>::type
operator>=(T const& x, T const& y) { return !(x < y); }

} // namespace range2

#endif
