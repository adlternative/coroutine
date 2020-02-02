/**
 * @author github.com/luncliff (luncliff@gmail.com)
 */
#include <coroutine/linux.h>

using namespace std;
using namespace coro;

auto wait_for_multiple_times(epoll_owner& ep, auto_reset_event& efd, //
                             uint32_t counter) -> forget_frame {
    while (counter--)
        co_await e;
}

int main(int, char*[]) {
    auto_reset_event e1{};

    wait_for_multiple_times(e1, 6);

    auto repeat = 100u; // prevent infinite loop
    while (counter > 0 && repeat > 0) {
        e1.set();
        // resume if there is available event-waiting coroutines
        for (auto task : signaled_event_tasks())
            task.resume();

        --repeat;
    };
    if (counter != 0)
        return __LINE__;

    return EXIT_SUCCESS;
}
