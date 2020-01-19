﻿//
//  Author  : github.com/luncliff (luncliff@gmail.com)
//  License : CC BY 4.0
//
#include <coroutine/return.h>
#include <coroutine/thread.h>

#include <sstream>

#include "test.h"
using namespace std;
using namespace coro;

auto procedure_call_self(HANDLE& thread, HANDLE finished) -> forget_frame {
    co_await procedure_call_on{thread};

    thread = GetCurrentThread();
    SetEvent(finished);
}

auto win32_procedure_call_on_self() {
    auto efinish = CreateEvent(nullptr, false, false, nullptr);
    auto worker = GetCurrentThread();

    procedure_call_self(worker, efinish);

    auto ec = WaitForSingleObjectEx(efinish, INFINITE, true);
    CloseHandle(efinish);

    {
        std::stringstream sout{};
        sout << "Self: WaitForSingleObjectEx\t" << ec;
        _println_(sout.str().c_str());
    }
    // expect the wait is cancelled by APC (WAIT_IO_COMPLETION)
    _require_(ec == WAIT_OBJECT_0 || ec == WAIT_IO_COMPLETION, //
              __FILE__, __LINE__);
    _require_(worker == GetCurrentThread());

    return EXIT_SUCCESS;
}

#if defined(CMAKE_TEST)
int main(int, char*[]) {
    return win32_procedure_call_on_self();
}

#elif __has_include(<CppUnitTest.h>)
#include <CppUnitTest.h>

template <typename T>
using TestClass = ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<T>;

class coro_win32_procedure_call_on_self
    : public TestClass<coro_win32_procedure_call_on_self> {
    TEST_METHOD(test_win32_procedure_call_on_self) {
        win32_procedure_call_on_self();
    }
};
#endif
