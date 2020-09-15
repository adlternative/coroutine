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
// this coroutine receives some messages from the channel
auto consumer(channel<int>& ch) -> no_return_t {
    // the type doesn't support for-co_await for now
    for (auto [msg, ok] = co_await ch.read(); ok;
         tie(msg, ok) = co_await ch.read()) {
        // ok == true: we received a value
        if (msg == bye) {
            puts("consumer loop exit");
            break;
        }
    }
    // we know that we received the `bye`,
    // but will read again to check `ok` becomes false
    auto [_, ok] = co_await ch.read();
    // ok == false: channel is going to be destroyed (no effect for read/write)
    assert(ok == false);
    puts("channel destruction detected");
}

auto producer_owner() -> no_return_t {
    channel<int> ch{};
    consumer(ch); // start a consumer routine

    for (int msg : {1, 2, 3, bye}) {
        auto ok = co_await ch.write(msg);
        // ok == true: we sent a value
        assert(ok == true);
    }
    puts("producer loop exit");
    puts("producer_owner is going to return");
}

#else
// GCC 10 reports internal compiler error when `co_await` is used in the for loop.
// Use unrolled code
auto consumer(channel<int>& ch) -> no_return_t {
    int msg{};
    bool ok{};
    std::tie(msg, ok) = co_await ch.read();
    assert(msg == 1);
    std::tie(msg, ok) = co_await ch.read();
    assert(msg == 2);
    std::tie(msg, ok) = co_await ch.read();
    assert(msg == 3);
    std::tie(msg, ok) = co_await ch.read();
    assert(msg == bye);
    puts("consumer loop exit");
    std::tie(msg, ok) = co_await ch.read();
    assert(ok == false);
    puts("channel destruction detected");
}

auto producer_owner() -> no_return_t {
    channel<int> ch{};
    consumer(ch); // start a consumer routine

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
    puts("producer_owner is going to return");
}

#endif

int main(int, char*[]) {
    producer_owner();
    return EXIT_SUCCESS;
}
