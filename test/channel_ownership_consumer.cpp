/**
 * @author github.com/luncliff (luncliff@gmail.com)
 */

#undef NDEBUG
#include <cassert>

#include <coroutine/channel.hpp>
#include <coroutine/return.h>

using namespace std;
using namespace coro;
#if defined(__GNUC__)
// for GCC the coroutine's return type must be `struct`
using no_return_t = coro::null_frame_t;
#else
using no_return_t = std::nullptr_t;
#endif

constexpr int bye = 0;

#if not defined(__GNUC__)
// this coroutine sends some messages to the channel
auto producer(channel<int>& ch) -> no_return_t {
    for (int msg : {1, 2, 3, bye}) {
        auto ok = co_await ch.write(msg);
        // ok == true: we sent a value
        assert(ok);
    }
    puts("producer loop exit");

    // we know that we sent the `bye`,
    // but will send again to check `ok` becomes `false`
    int msg = -1;
    auto ok = co_await ch.write(msg);
    // ok == false: channel is going to be destroyed (no effect for read/write)
    assert(ok == false);
    puts("channel destruction detected");
}
#else
// GCC 10 reports internal compiler error when `co_await` is used in the for loop.
// Use unrolled code
auto producer(channel<int>& ch) -> no_return_t {
    int msg = 1;
    auto ok = co_await ch.write(msg);
    assert(ok);
    msg = 2;
    ok = co_await ch.write(msg);
    assert(ok);
    msg = 3;
    ok = co_await ch.write(msg);
    assert(ok);
    msg = bye;
    ok = co_await ch.write(msg);
    assert(ok);
    puts("producer loop exit");

    // we know that we sent the `bye`,
    // but will send again to check `ok` becomes `false`
    msg = -1;
    ok = co_await ch.write(msg);
    // ok == false: channel is going to be destroyed (no effect for read/write)
    assert(ok == false);
    puts("channel destruction detected");
}
#endif

auto consumer_owner() -> no_return_t {
    channel<int> ch{};
    producer(ch); // start a producer routine

    // the type doesn't support for-co_await for now
    for (auto [msg, ok] = co_await ch.read(); ok;
         tie(msg, ok) = co_await ch.read()) {
        // ok == true: we sent a value
        if (msg == bye) {
            puts("consumer loop exit");
            break;
        }
    }
    puts("consumer_owner is going to return");
}

int main(int, char*[]) {
    consumer_owner();
    return EXIT_SUCCESS;
}
