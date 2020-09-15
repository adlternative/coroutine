/**
 * @author github.com/luncliff (luncliff@gmail.com)
 * 
 * @note GCC 10.0 fails to compile this. (internal compiler error)
 */
#undef NDEBUG
#include <algorithm>
#include <array>
#include <cassert>

#include <coroutine/yield.hpp>

using namespace std;

auto yield_values() -> coro::enumerable<uint32_t> {
    array<uint32_t, 4> storage{1, 2, 3, 4};
    for (auto v : storage)
        co_yield v;
    co_yield 5u;
}

int main(int, char*[]) {
    auto g = yield_values();
    auto it = std::max_element(g.begin(), g.end());
    // after iteration is finished (`co_return`),
    // the iterator will hold nullptr.
    // so referencing it will lead to access violation
    assert(it.operator->() == nullptr);
    return EXIT_SUCCESS;
}
