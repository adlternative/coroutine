#
#   github.com/luncliff (luncliff@gmail.com)
#
cmake_minimum_required(VERSION 3.12) # supports CXX_STANDARD 20
project(coroutine   LANGUAGES   CXX 
                    VERSION     2.0.0
)
if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS true)
endif()
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

message(STATUS "system: ${CMAKE_SYSTEM}")
message(STATUS "build_type: ${CMAKE_BUILD_TYPE}")
message(STATUS "paths:")
message(STATUS " - ${PROJECT_SOURCE_DIR}")
message(STATUS " - ${CMAKE_INSTALL_PREFIX}")
message(STATUS " - ${CMAKE_SOURCE_DIR}")
message(STATUS " - ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS)
message(STATUS "compiler:")
message(STATUS " - ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - ${CMAKE_CXX_COMPILER}")
message(STATUS)
message(STATUS "cmake:")
message(STATUS " - ${CMAKE_VERSION}")
message(STATUS " - ${CMAKE_COMMAND}")
message(STATUS " - ${CMAKE_GENERATOR}")
message(STATUS " - ${CMAKE_BUILD_TOOL}")
if(DEFINED CMAKE_TOOLCHAIN_FILE)
message(STATUS " - ${CMAKE_TOOLCHAIN_FILE}")
endif()
message(STATUS)




#
# import external libraries. see 'external/'
#
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Microsoft.GSL CONFIG) # microsoft/GSL v3.1.0
if(FALSE)
    get_target_property(GSL_INCLUDE_DIR 
                        Microsoft.GSL::GSL INTERFACE_INCLUDE_DIRECTORIES)
else()
    add_subdirectory(external/ms-gsl)
    if (NOT TARGET Microsoft.GSL::GSL)
        message(FATAL_ERROR "expect target Microsoft.GSL::GSL")
    endif()
    get_target_property(GSL_INCLUDE_DIR 
                        Microsoft.GSL::GSL INTERFACE_INCLUDE_DIRECTORIES)
endif()
message(STATUS "using ms-gsl: ${GSL_INCLUDE_DIR}")
message(STATUS)




#
# Acquire informations about current build environment. Especially for Compiler & STL
#   - support_latest
#   - support_coroutine
#   - support_intrinsic_builtin: optional. compiler supports `__builtin_coro_*`
#   - has_coroutine
#   - has_coroutine_ts
#
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
# include(CheckCXXSymbolExists)
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    if(WIN32)
        check_cxx_compiler_flag("/std:c++latest"            support_latest)
        check_cxx_compiler_flag("/clang:-fcoroutines-ts"    support_coroutine)
        check_include_file_cxx("experimental/coroutine" has_coroutine_ts
            "/std:c++latest"
        )
    else()
        check_cxx_compiler_flag("-std=c++2a"          support_latest)
        check_cxx_compiler_flag("-fcoroutines-ts"     support_coroutine)
        check_include_file_cxx("experimental/coroutine" has_coroutine_ts
            "-std=c++2a -stdlib=libc++"
        )
    endif()

elseif(MSVC)
    #
    # Notice that `/std:c++latest` and `/await` is exclusive to each other.
    # With MSVC, we have to distinguish Coroutines TS & C++ 20 Coroutines
    #
    check_cxx_compiler_flag("/std:c++latest"    support_latest)
    check_cxx_compiler_flag("/await"            support_coroutine)
    check_include_file_cxx("coroutine"  has_coroutine
        "/std:c++latest"
    )
    if(NOT has_coroutine)
        message(STATUS "Try <expeirmental/coroutine> (Coroutines TS) instead of <coroutine> ...")
        check_include_file_cxx("experimental/coroutine" has_coroutine_ts
            "/std:c++17" "/await"
        )
    endif()
    # has coroutine headers?
    if(NOT has_coroutine AND NOT has_coroutine_ts)
        message(FATAL_ERROR "There are no headers for C++ Coroutines")
    endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    #
    # expect GCC 10 or later
    #
    check_cxx_compiler_flag("-std=gnu++20"        support_latest)
    check_cxx_compiler_flag("-fcoroutines"        support_coroutine)
    check_include_file_cxx("coroutine" has_coroutine
        "-std=gnu++20 -fcoroutines"
    )
    if(APPLE)
        # -isysroot "/usr/local/Cellar/gcc/${CMAKE_CXX_COMPILER_VERSION}/include/c++/${CMAKE_CXX_COMPILER_VERSION}"
        # -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include"
        # -isysroot "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1"
    endif()
    set(try_compile_flags "-fcoroutines")
endif()
# support compiler options for coroutine?
if(NOT support_coroutine)
    message(FATAL_ERROR "The compiler doesn't support C++ Coroutines")
endif()
# support `__builtin_coro_*`?
if(NOT DEFINED support_intrinsic_builtin)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    get_filename_component(intrinsic_test_cpp
                           compiler_test/__builtin.cpp ABSOLUTE)
    try_compile(support_intrinsic_builtin ${CMAKE_BINARY_DIR}/supports
        SOURCES         ${intrinsic_test_cpp}
        # This is a workaround. for some reasone CMAKE_FLAGS has no effect.
        # Instead, place the compiler option where macro definition are placed
        COMPILE_DEFINITIONS ${try_compile_flags}    
        CXX_STANDARD    20                          
        OUTPUT_VARIABLE report
    )
    if(NOT support_intrinsic_builtin)
        message(VERBOSE ${report})
    endif()
    message(STATUS "supports __builtin_coro: ${support_intrinsic_builtin}")
endif()



#
# Known STL-Compiler issues
#   - https://github.com/microsoft/STL, issue 100
#
if(CMAKE_CXX_COMPILER_ID MATCHES Clang AND WIN32)
    message(WARNING "clang-cl won't work with <experimental/coroutine>")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    message(WARNING "GCC may fails to build current code")
endif()



add_library(coroutine_portable
    include/coroutine/frame.h
    src/frame.cpp
)

# todo: remove this
set_target_properties(coroutine_portable
PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS true
)

target_include_directories(coroutine_portable
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

target_link_libraries(coroutine_portable
PUBLIC
    Microsoft.GSL::GSL
)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    if(WIN32)
        # 'target_compile_options' removes duplicated -Xclang directive.
        # avoide the removal using cmake flag variable
        target_compile_options(coroutine_portable
        PUBLIC
            "/clang:-fcoroutines-ts"
            -fms-compatibility /std:c++17
        )
        # in this case, we always use portable header
        target_compile_definitions(coroutine_portable
        PUBLIC
            _RESUMABLE_FUNCTIONS_SUPPORTED
        )

    elseif(UNIX OR APPLE)
        target_compile_options(coroutine_portable
        PUBLIC
            -std=c++2a -stdlib=libc++
            -fcoroutines-ts
        )
    endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(coroutine_portable
    PUBLIC
        -std=gnu++2a
        -fcoroutines -fconcepts # -fno-exceptions
    )
    target_link_libraries(coroutine_portable
    PUBLIC
        stdc++
    )

elseif(MSVC)
    # select between C++ Coroutines TS & C++ 20 Coroutines
    if(support_intrinsic_builtin AND has_coroutine)
        target_compile_options(coroutine_portable
        PUBLIC
            /std:c++latest
        )
    else()
        target_compile_options(coroutine_portable
        PUBLIC
            /std:c++17 /await
        )
    endif()
    target_compile_options(coroutine_portable
    PRIVATE
        /JMC-
    )
endif()

if(WIN32)
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
    if(ANDROID)
        target_link_libraries(coroutine_portable
        PUBLIC
            ${ANDROID_STL}
        )
    else()
        # target_link_libraries(coroutine_portable
        # PUBLIC
        #     stdc++
        # )
    endif()

elseif(UNIX OR APPLE)
    target_link_libraries(coroutine_portable
    PUBLIC
        c++
    )

endif()

# add_subdirectory(modules/system)    # coroutine_system
# add_subdirectory(modules/net)       # coroutine_net

install(FILES           include/coroutine/frame.h
                        include/coroutine/return.h
                        include/coroutine/channel.hpp
                        include/coroutine/yield.hpp
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
)
if(WIN32)
    install(FILES       include/coroutine/windows.h
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
    )
    # install(FILES       $<TARGET_PDB_FILE:coroutine_portable>
    #         DESTINATION bin
    # )

elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
    install(FILES       include/coroutine/linux.h
                        include/coroutine/pthread.h
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
    )

elseif(UNIX OR APPLE)
    install(FILES       include/coroutine/unix.h
    include/coroutine/pthread.h
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
    )

endif()
install(FILES           include/coroutine/net.h
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
)

install(TARGETS     coroutine_portable # coroutine_system coroutine_net
        EXPORT      coroutine-config
        # INCLUDES    DESTINATION     ${CMAKE_INSTALL_PREFIX}/include
        RUNTIME     DESTINATION     ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY     DESTINATION     ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE     DESTINATION     ${CMAKE_INSTALL_PREFIX}/lib
)

#
# export declared cmake targets
#
# 'coroutine-targets' is indeed better name, but without using 'configure_file()'
# the exporting step will be more complicated for non-CMake users.
# just merge all contents into the file 'coroutine-config.cmake'
#
install(EXPORT      ${PROJECT_NAME}-config
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)

#
# generate/install config & version info
#
include(CMakePackageConfigHelpers)
set(VERSION_FILE_PATH   ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}-config-version.cmake)
write_basic_package_version_file(${VERSION_FILE_PATH}
    VERSION             ${PROJECT_VERSION}
    COMPATIBILITY       SameMajorVersion
)
install(FILES           ${VERSION_FILE_PATH} 
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)

#
# for testing, CTest will be used
#
if(NOT BUILD_TESTING)
    message(STATUS "Test is disabled.")
    return()
elseif(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "This is not a root project. Skipping the tests")
    return()
elseif(ANDROID OR IOS)
    return()
endif()
enable_testing()

# helper for test codes
set(BUILD_TESTING OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(external/latch)
# add_subdirectory(external/sockets)

#
# create_ctest( ... )
#
#   See source files in 'test/'. All files in the filder will become test program.
#   Arguments after TEST_NAME should be library (or CMake target) name
#
function(create_ctest TEST_NAME)
    # create a test exe with the given name ...
    add_executable(test_${TEST_NAME} test/${TEST_NAME}.cpp)
    add_test(NAME ${TEST_NAME} COMMAND test_${TEST_NAME})
    foreach(idx RANGE 1 ${ARGC})
        target_link_libraries(test_${TEST_NAME}
        PRIVATE
            ${ARGV${idx}} 
        )
    endforeach()
    if(WIN32)
        target_compile_definitions(test_${TEST_NAME}
        PRIVATE
            WIN32_LEAN_AND_MEAN NOMINMAX
        )
    elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
        target_link_libraries(test_${TEST_NAME}
        PUBLIC
            stdc++ c++
        )
    endif()
endfunction()

#
#   <coroutine/yield.hpp>
#
if(TARGET coroutine_portable)
    create_ctest( enumerable_accumulate     coroutine_portable )
    create_ctest( enumerable_iterator       coroutine_portable )
    create_ctest( enumerable_move           coroutine_portable )
    create_ctest( enumerable_yield_never    coroutine_portable )
    create_ctest( enumerable_yield_once     coroutine_portable )
    create_ctest( enumerable_yield_rvalue   coroutine_portable )
endif()

#
#   <coroutine/windows.h>
#   <coroutine/unix.h>, <coroutine/linux.h>
#   <coroutine/pthread.h>
#
if(TARGET coroutine_system)
if(WIN32)
    create_ctest( windows_event_set             coroutine_system )
    create_ctest( windows_event_cancel          coroutine_system )
    create_ctest( windows_event_wait_one        coroutine_system )
    create_ctest( windows_event_wait_multiple   coroutine_system )
    create_ctest( windows_on_apc_known          coroutine_system )
    create_ctest( windows_on_apc_self           coroutine_system )
    create_ctest( windows_on_thread_pool        coroutine_system latch )

elseif(UNIX)
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    # gcc-10 reports internal compiler error for these pthread tests
else()
    create_ctest( pthread_detach_no_spawn       coroutine_system )
    create_ctest( pthread_detach_spawn          coroutine_system )
    create_ctest( pthread_join_no_spawn         coroutine_system )
    create_ctest( pthread_join_spawn_1          coroutine_system )
    create_ctest( pthread_join_spawn_2          coroutine_system )
endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    create_ctest( linux_event_no_wait       coroutine_system )
    create_ctest( linux_event_wait          coroutine_system )
    create_ctest( linux_event_signal        coroutine_system )

elseif(UNIX)
    create_ctest( unix_kqueue_single_thread  coroutine_system )

endif()
endif()


#
#   <coroutine/channel.hpp>
#
if(TARGET coroutine_portable)
    create_ctest( channel_close_read            coroutine_portable )
    create_ctest( channel_close_write           coroutine_portable )
    create_ctest( channel_ownership_consumer    coroutine_portable ) # workaround: GCC 10.0
    create_ctest( channel_ownership_producer    coroutine_portable ) # workaround: GCC 10.0
    create_ctest( channel_read_write_mutex      coroutine_portable )
    create_ctest( channel_read_write_nolock     coroutine_portable )
    create_ctest( channel_write_read_mutex      coroutine_portable )
    create_ctest( channel_write_read_nolock     coroutine_portable )
endif()
if(TARGET coroutine_system)    
    create_ctest( channel_select_empty          coroutine_system )
    create_ctest( channel_select_type           coroutine_system )
if(WIN32)
    create_ctest( channel_race_condition        coroutine_system latch )
endif()
    create_ctest( channel_sample_wrap           coroutine_system latch )
endif()

#
#   <coroutine/net.h>
#
if(TARGET coroutine_net)
    create_ctest( net_socket_tcp_echo   coroutine_net ssf latch )
    create_ctest( net_socket_udp_echo   coroutine_net ssf latch )
    create_ctest( net_resolve_name      coroutine_net ssf )
    create_ctest( net_resolve_ip6       coroutine_net ssf )
    create_ctest( net_resolve_tcp6      coroutine_net ssf )
    create_ctest( net_resolve_udp6      coroutine_net ssf )
endif()

#
# Try to compile the given source code and execute via ctest
#
function(create_sample SAMPLE_NAME SOURCE_FILE)
    add_executable(${SAMPLE_NAME} ${SOURCE_FILE})
    add_test(NAME ${SAMPLE_NAME} COMMAND ${SAMPLE_NAME})
    target_link_libraries(${SAMPLE_NAME}
    PRIVATE
        coroutine_portable # PUBLIC compile options for <coroutine>
                           # Currently <coroutine> samples are for latest MSVC
    )
    if(WIN32)
        target_compile_definitions(${SAMPLE_NAME}
        PRIVATE
            WIN32_LEAN_AND_MEAN NOMINMAX
        )
    elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
        target_link_libraries(${SAMPLE_NAME}
        PUBLIC
            c++
        )
    endif()
endfunction()

if(has_coroutine_ts OR has_coroutine)
    create_sample(sample_02 samples/not_coroutine.cpp)
    create_sample(sample_03 samples/not_subroutine.cpp)
    create_sample(sample_04 samples/destroy_return.cpp)
    create_sample(sample_05 samples/destroy_handle.cpp)

    create_sample(sample_11 samples/russian_roulette.cpp)
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        # the following code failes on GCC
    else()
        create_sample(sample_12 samples/max_element.cpp)
    endif()
endif()
