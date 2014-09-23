#ifndef INCLUDED_COMPILER_SPECIFICS
#define INCLUDED_COMPILER_SPECIFICS

#ifndef INLINE
#define INLINE inline
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE INLINE __attribute__((always_inline))
#endif

#ifndef HIDDEN
#define HIDDEN __attribute__((__visibility__("hidden")))
#endif

#ifndef INLINE_HIDDEN
#define INLINE_HIDDEN INLINE HIDDEN
#endif

#ifndef ALWAYS_INLINE_HIDDEN
#define ALWAYS_INLINE_HIDDEN ALWAYS_INLINE HIDDEN
#endif

#ifndef TYPE_DEFAULT_VISIBILITY
#define TYPE_DEFAULT_VISIBILITY __attribute__((__visibility__("default")))
#endif

#ifndef TYPE_HIDDEN_VISIBILITY
#define TYPE_HIDDEN_VISIBILITY __attribute__((__visibility__("hidden")))
#endif

// Place this just before the closing brace of the metaprogramming class
// passing in the class name as the parameter. This prevents accidental
// construction of a metaprogramming only class.
// Should be used for all TYPE_HIDDEN_VISIBILITY classes
#ifndef METAPROGRAMMING_ONLY
#define METAPROGRAMMING_ONLY(x) private: x(); ~x(); public:
#endif

#ifndef INTROSPECTION_EXPECT
#define INTROSPECTION_EXPECT(Expected, Actual) __builtin_expect(Expected, (Actual))
#endif

#ifndef INTROSPECTION_LIKELY
#define INTROSPECTION_LIKELY(x) __builtin_expect(true, !!(x))
#endif

#ifndef INTROSPECTION_UNLIKELY
#define INTROSPECTION_UNLIKELY(x) __builtin_expect(false, !!(x))
#endif

#endif
