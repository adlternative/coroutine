﻿//
//  Author  : github.com/luncliff (luncliff@gmail.com)
//  License : CC BY 4.0
//
//  Note
//      `enumerable` is simply a copy of `generator` in VC++
//
#ifndef COROUTINE_YIELD_HPP
#define COROUTINE_YIELD_HPP

#include <coroutine/return.h>
#include <iterator>

namespace coro {
using namespace std::experimental;

// <experimental/generator> from the VC++
template <typename T>
class enumerable {
  public:
    class promise_type;
    class iterator;

    using value_type = T;
    using reference = value_type&;
    using pointer = value_type*;

  private:
    coroutine_handle<promise_type> coro{};

  public:
    enumerable(const enumerable&) = delete;
    enumerable& operator=(const enumerable&) = delete;
    enumerable(enumerable&& rhs) noexcept : coro{rhs.coro} {
        rhs.coro = nullptr;
    }
    enumerable& operator=(enumerable&& rhs) noexcept {
        std::swap(coro, rhs.coro);
        return *this;
    }
    enumerable() noexcept = default;
    enumerable(promise_type* ptr) noexcept
        : coro{coroutine_handle<promise_type>::from_promise(*ptr)} {
    }
    ~enumerable() noexcept {
        // enumerable will destroy the frame.
        //  promise/iterator are free from those ownership
        if (coro)
            coro.destroy();
    }

  public:
    iterator begin() noexcept(false) {
        if (coro) // resumeable?
        {
            coro.resume();
            if (coro.done()) // finished?
                return iterator{nullptr};
        }
        return iterator{coro};
    }
    iterator end() noexcept {
        return iterator{nullptr};
    }

  public:
    class promise_type final : public promise_manual_control {
        friend class iterator;
        friend class enumerable;

        pointer current = nullptr;

      public:
        promise_type* get_return_object() noexcept {
            // enumerable will create coroutine handle from the address
            return this;
        }

        // `co_yield` expression. for reference
        auto yield_value(reference ref) noexcept {
            current = std::addressof(ref);
            return suspend_always{};
        }
        // `co_yield` expression. for r-value
        auto yield_value(value_type&& v) noexcept {
            return yield_value(v);
        }
        // `co_return` expression
        void return_void() noexcept {
            // no more access to value
            current = nullptr;
        }
    };

    class iterator final {
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using reference = value_type&;
        using pointer = value_type*;

      public:
        coroutine_handle<promise_type> coro;

      public:
        // `enumerable::end()`
        explicit iterator(std::nullptr_t) noexcept : coro{nullptr} {
        }
        // `enumerable::begin()`
        explicit iterator(coroutine_handle<promise_type> handle) noexcept
            : coro{handle} {
        }

      public:
        iterator& operator++(int) = delete; // post increment
        iterator& operator++() noexcept(false) {
            coro.resume();
            if (coro.done())    // enumerable will destroy
                coro = nullptr; // the frame later...

            return *this;
        }

        pointer operator->() noexcept {
            pointer ptr = coro.promise().current;
            return ptr;
        }
        reference operator*() noexcept {
            return *(this->operator->());
        }

        bool operator==(const iterator& rhs) const noexcept {
            return this->coro == rhs.coro;
        }
        bool operator!=(const iterator& rhs) const noexcept {
            return !(*this == rhs);
        }
    };
};

} // namespace coro

#endif // COROUTINE_YIELD_HPP
