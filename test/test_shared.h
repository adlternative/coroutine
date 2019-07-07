//
//  Author  : github.com/luncliff (luncliff@gmail.com)
//  License : CC BY 4.0
//
#pragma once
#include <coroutine/frame.h>

#include <coroutine/channel.hpp>
#include <coroutine/concrt.h>
#include <coroutine/return.h>
#include <coroutine/yield.hpp>

#include <gsl/gsl>

#include <array>
#include <atomic>
#include <future>

using namespace std;
using namespace std::experimental;
using namespace std::literals;

#if defined(__clang__) // for clang, use cmake test

#define REQUIRE(cond)                                                          \
    if ((cond) == false) {                                                     \
        printf("%s %d\n", __FILE__, __LINE__);                                 \
        exit(__LINE__);                                                        \
    }
#define PRINT_MESSAGE(msg)                                                     \
    printf("%s %s %d\n", msg.c_str(), __FILE__, __LINE__);
#define FAIL_WITH_MESSAGE(msg)                                                 \
    {                                                                          \
        PRINT_MESSAGE(msg);                                                    \
        exit(__LINE__);                                                        \
    }
#define FAIL_WITH_CODE(ec) FAIL_WITH_MESSAGE(system_category().message(ec));

#elif __has_include(<CppUnitTest.h>) // for msvc, use visual studio test
#include <CppUnitTest.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define REQUIRE(cond) Assert::IsTrue(cond)
#define PRINT_MESSAGE(msg) Logger::WriteMessage(msg.c_str());
#define FAIL_WITH_MESSAGE(msg)                                                 \
    Logger::WriteMessage(msg.c_str());                                         \
    Assert::Fail();
#define FAIL_WITH_CODE(ec)                                                     \
    Logger::WriteMessage(system_category().message(ec).c_str());               \
    Assert::Fail();

#endif