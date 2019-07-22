//
//  Author  : github.com/luncliff (luncliff@gmail.com)
//  License : CC BY 4.0
//
//  Note
//      Return and utility types for coroutine frame management
//
#pragma once
#ifndef COROUTINE_PROMISE_AND_RETURN_TYPES_H
#define COROUTINE_PROMISE_AND_RETURN_TYPES_H

#include <coroutine/frame.h>

namespace coro {
using namespace std::experimental;

#if defined(__cpp_concepts) // clang-format off

template<typename T, typename R = void>
concept bool awaitable = requires(T a, coroutine_handle<void> h) {
    { a.await_ready() } -> bool;
    { a.await_suspend(h) } -> void;
    { a.await_resume() } -> R;
};

template<typename P>
concept bool promise_requirement_basic = requires(P p) {
    { p.initial_suspend() } -> awaitable;
    { p.final_suspend() } -> awaitable;
    { p.unhandled_exception() } -> void;
};
#endif //clang-format on 

class promise_return_destroy {
  public:
    auto initial_suspend() noexcept {
        return suspend_never{};
    }
    auto final_suspend() noexcept {
        return suspend_never{};
    }
    void unhandled_exception() noexcept(false) {
        // customize this part
    }
};

class promise_return_preserve {
  public:
    auto initial_suspend() noexcept {
        return suspend_never{};
    }
    auto final_suspend() noexcept {
        return suspend_always{};
    }
    void unhandled_exception() noexcept(false) {
        // customize this part
    }
};

class promise_always_suspend {
  public:
    auto initial_suspend() noexcept {
        return suspend_always{};
    }
    auto final_suspend() noexcept {
        return suspend_always{};
    }
    void unhandled_exception() noexcept(false) {
        // customize this part
    }
};

class forget_frame {
  public:
    class promise_type final : public promise_return_destroy {
      public:
        void return_void() noexcept {
            // nothing to do because this is `void` return
        }
        auto get_return_object() noexcept -> forget_frame {
            return {this};
        }
        static auto get_return_object_on_allocation_failure() noexcept
            -> forget_frame {
            return {nullptr};
        }
    };

  private:
    forget_frame(const promise_type*) noexcept {
        // the type truncates all given info about its frame
    }

  public:
    // gcc-10 requires the type to be default constructible
    forget_frame() noexcept = default;
};

class preserve_frame : public coroutine_handle<void> {
  public:
    class promise_type final : public promise_return_preserve {
      public:
        void return_void() noexcept {
            // nothing to do because this is `void` return
        }
        auto get_return_object() noexcept -> preserve_frame {
            return {this};
        }
        static auto get_return_object_on_allocation_failure() noexcept
            -> preserve_frame {
            return {nullptr};
        }
    };

  private:
    preserve_frame(promise_type* p) noexcept : coroutine_handle<void>{} {
        if (p == nullptr)
            return;
        coroutine_handle<void>& ref = *this;
        ref = coroutine_handle<promise_type>::from_promise(*p);
    }

  public:
    // gcc-10 requires the type to be default constructible
    preserve_frame() noexcept = default;
};

class save_frame_to final {
  public:
    // provide interface to receive handle
    // when it's used as an operand of `co_await`
    void await_suspend(coroutine_handle<void> coro) noexcept {
        ref = coro;
    }
    constexpr bool await_ready() noexcept {
        return false;
    }
    constexpr void await_resume() noexcept {
    }

  public:
    explicit save_frame_to(coroutine_handle<void>& target) : ref{target} {
    }

  private:
    coroutine_handle<void>& ref;
};

} // namespace coro

#endif // COROUTINE_PROMISE_AND_RETURN_TYPES_H
