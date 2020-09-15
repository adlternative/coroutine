/**
 * @file coroutine/return.h
 * @author github.com/luncliff (luncliff@gmail.com)
 * 
 * @brief   All helpers to support compiler independent source code
 * 
 * @copyright CC BY 4.0
 * @todo    C++ 20 Concepts https://devblogs.microsoft.com/cppblog/c20-concepts-are-here-in-visual-studio-2019-version-16-3/
 */
#pragma once
#if defined(__cpp_lib_coroutine)
#endif

#if defined(__cpp_concepts)
/*
template <typename T, typename R = void>
concept awaitable = requires(T a, coroutine_handle<void> h) {
    { a.await_ready() } ->bool;
    { a.await_suspend(h) } ->void;
    { a.await_resume() } ->R;
};
template <typename P>
concept promise_requirement_basic = requires(P p) {
    { p.initial_suspend() } ->awaitable;
    { p.final_suspend() } ->awaitable;
    { p.unhandled_exception() } ->void;
};
*/
#endif

#if __has_include(<coroutine/frame.h>) && !defined(USE_EXPERIMENTAL_COROUTINE)
#include <coroutine/frame.h>

namespace coro {
using std::coroutine_handle;
using std::suspend_always;
using std::suspend_never;

#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>

namespace coro {
using std::experimental::coroutine_handle;
using std::experimental::suspend_always;
using std::experimental::suspend_never;

#else
#error "requires header <experimental/coroutine> or <coroutine/frame.h>"
#endif // <experimental/coroutine>

#ifndef COROUTINE_PROMISE_AND_RETURN_TYPES_H
#define COROUTINE_PROMISE_AND_RETURN_TYPES_H
#include <type_traits>

/**
 * @defgroup Return
 * Types for easier coroutine promise/return type definition.
 */

/**
 * @brief   no suspend in initial/final + `void` return
 * @ingroup Return
 */
class null_frame_promise {
  public:
    suspend_never initial_suspend() {
        return {};
    }
    suspend_never final_suspend() {
        return {};
    }
    void unhandled_exception() noexcept(false) {
        throw;
    }
    void return_void() noexcept {
    }
};

/**
 * @brief   `void` retrun for coroutine function
 * @note    The library supports `coroutine_traits` specialization for `nullptr_t`.
 *          This type is for GCC, which doesn't allow non-struct return.
 * 
 * @code
 * // for MSVC, Clang, GCC
 * auto coroutine_1() -> null_frame_t {
 *     co_await suspend_never{};
 * }
 * 
 * // GCC doesn't allow this! (`nullptr_t` is not class)
 * auto coroutine_2() -> nullptr_t {
 *     co_await suspend_never{};
 * }
 * @endcode
 */
struct null_frame final {
    struct promise_type : public null_frame_promise {
        null_frame get_return_object() noexcept {
            return {};
        }
    };
};
using null_frame_t = null_frame;

/**
 * @brief   A type to acquire `coroutine_handle<void>` from anonymous coroutine's return. 
 *          Requires manual `destroy` of the coroutine handle.
 * 
 * @ingroup Return
 * @see coroutine_handle<void>
 * @see promise_na
 */
class frame : public coroutine_handle<void> {
  public:
    /**
     * @brief Acquire `coroutine_handle<void>` from current object and expose it through `get_return_object`
     */
    class promise_type {
      public:
        suspend_never initial_suspend() {
            return {};
        }
        suspend_always final_suspend() {
            return {};
        }
        /**
         * @brief The `frame_t` will do nothing for exception handling
         */
        void unhandled_exception() noexcept(false) {
            throw;
        }
        void return_void() noexcept {
        }
        /**
         * @brief Acquire `coroutine_handle<void>` from current promise and return it
         * @return frame_t 
         */
        frame get_return_object() noexcept {
            return frame{coroutine_handle<promise_type>::from_promise(*this)};
        }
    };

  public:
    explicit frame(coroutine_handle<void> frame = nullptr) noexcept
        : coroutine_handle<void>{frame} {
    }
};
using frame_t = frame;

/**
 * @brief Suspend after invoke and expose its `coroutine_handle<void>` through return
 * @ingroup Return
 * @see coroutine_handle<void>
 * @see frame_t
 * @see promise_aa
 */
class passive_frame : public coroutine_handle<void> {
  public:
    class promise_type {
      public:
        suspend_always initial_suspend() {
            return {};
        }
        suspend_always final_suspend() {
            return {};
        }
        void unhandled_exception() noexcept(false) {
            throw;
        }
        void return_void() noexcept {
        }
        passive_frame get_return_object() noexcept {
            return passive_frame{
                coroutine_handle<promise_type>::from_promise(*this)};
        }
    };
    explicit passive_frame(coroutine_handle<void> frame = nullptr) noexcept
        : coroutine_handle<void>{frame} {
    }
};
using passive_frame_t = passive_frame;

} // namespace coro

#if defined(__GNUC__)
/// @todo workaround for `coroutine_traits<std::nullptr_t>`
#else
namespace std {
namespace experimental {

/**
 * @brief Allow `void` return of the coroutine
 * 
 * @tparam P input parameter types of the coroutine's signature
 * @ingroup Return
 */
template <typename... P>
struct coroutine_traits<nullptr_t, P...> {
    struct promise_type : public coro::null_frame_promise {
        /**
         * @brief Since this is template specialization for `void`, the return type is fixed to `void`
         */
        nullptr_t get_return_object() noexcept {
            return nullptr;
        }
    };
};

} // namespace experimental
} // namespace std
#endif // __GNUC__

#endif // COROUTINE_PROMISE_AND_RETURN_TYPES_H
