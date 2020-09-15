/**
 * @author github.com/luncliff (luncliff@gmail.com)
 */
#undef NDEBUG
#include <cassert>
#include <coroutine/return.h>

#if defined(__GNUC__)
// for GCC the coroutine's return type must be `struct`
using coro::null_frame_t;
#else
using null_frame_t = std::nullptr_t;
#endif

// this is a coroutine
auto invoke_and_forget_frame() -> null_frame_t {
    co_await std::suspend_never{};
    co_return;
};

int main(int, char*[]) {
    invoke_and_forget_frame();
    return 0;
}
