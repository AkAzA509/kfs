#ifndef _ATTRS_H
#define _ATTRS_H

#ifndef __has_c_attribute
#define __has_c_attribute(x) 0
#endif

#if __has_c_attribute(gnu::pure)
#define __PURE [[gnu::pure]]
#else
#define __PURE
#endif

#if __has_c_attribute(gnu::const)
#define __CONST [[gnu::const]]
#else
#define __CONST
#endif

#if __has_c_attribute(unsequenced)
#define __UNSEQUENCED [[unsequenced]]
#elif __has_c_attribute(gnu::const)
#define __UNSEQUENCED [[gnu::const]]
#else
#define __UNSEQUENCED
#endif

#if __has_c_attribute(gnu::nonnull)
#define __NONNULL(...) [[gnu::nonnull __VA_OPT__((__VA_ARGS__))]]
#else
#define __NONNULL(...)
#endif

#if __has_c_attribute(gnu::format)
#define __FORMAT(archetype, fmt_idx, first_arg) \
	[[gnu::format(__##archetype##__, fmt_idx, first_arg)]]
#else
#define __FORMAT(archetype, fmt_idx, first_arg)
#endif

#if __has_c_attribute(noreturn)
#define __NORETURN [[noreturn]]
#elif __has_c_attribute(gnu::noreturn)
#define __NORETURN [[gnu::noreturn]]
#else
#define __NORETURN
#endif

#if __has_c_attribute(nodiscard)
#define __NODISCARD [[nodiscard]]
#define __NODISCARD_MSG(msg) [[nodiscard(msg)]]
#elif __has_c_attribute(gnu::warn_unused_result)
#define __NODISCARD [[gnu::warn_unused_result]]
#define __NODISCARD_MSG(msg) [[gnu::warn_unused_result]]
#else
#define __NODISCARD
#define __NODISCARD_MSG(msg)
#endif

#if __has_c_attribute(fallthrough)
#define __FALLTHROUGH [[fallthrough]]
#elif __has_c_attribute(gnu::fallthrough)
#define __FALLTHROUGH [[gnu::fallthrough]]
#elif __has_c_attribute(clang::fallthrough)
#define __FALLTHROUGH [[clang::fallthrough]]
#else
#define __FALLTHROUGH ((void)0)
#endif

#endif // _ATTRS_H
