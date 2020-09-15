# coroutine

C++ 20 Coroutines in Action

[![Build Status](https://dev.azure.com/luncliff/personal/_apis/build/status/luncliff.coroutine?branchName=dev/2.0)](https://dev.azure.com/luncliff/personal/_build/latest?definitionId=27&branchName=dev/2.0)
[![Build status](https://ci.appveyor.com/api/projects/status/vpjssf4g6cv4a4ys/branch/master?svg=true)](https://ci.appveyor.com/project/luncliff/coroutine/branch/master)
[![Build Status](https://travis-ci.com/luncliff/coroutine.svg?branch=dev/2.0)](https://travis-ci.com/luncliff/coroutine)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/38aa16f6d7e046898af3835918c0cd5e)](https://app.codacy.com/app/luncliff/coroutine?utm_source=github.com&utm_medium=referral&utm_content=luncliff/coroutine&utm_campaign=Badge_Grade_Dashboard)
[![](https://sonarcloud.io/api/project_badges/measure?project=luncliff_coroutine&metric=ncloc)](https://sonarcloud.io/dashboard?id=luncliff_coroutine)

### Purpose of this repository

> **If you are looking for other materials, visit [the MattPD's collection](https://gist.github.com/MattPD/9b55db49537a90545a90447392ad3aeb#file-cpp-std-coroutines-draft-md)!**

* Help understanding of the C++ Coroutines
* Provide meaningful design example with the feature

In that perspective, the library will be maintained as small as possible. Have fun with them. And try your own coroutines!
There is a custom implementation to handle diffence between compilers for the C++ Coroutines. If you are interested, see the [`<coroutine/frame.h>`](./include/coroutine/frame.h) and [its internal](./src/modules/frame.cpp).

#### File Organization

* [compile_test/](./compile_test/)
* [include/](./include/coroutine)
* [test/](./test/): Files to test use of current implementation required libraries
* [samples/](./samples): How to use C++ Coroutines

### Pre-requisite

* [Microsoft GSL v3.1+](https://github.com/microsoft/GSL/releases)

The installation of this library will install it together.
All other required modules for build/test will be placed in [external/](./external).

### Tool Support

With [CMake 3.12](./CMakeLists.txt), I'm trying to followup the listed compilers

* MSVC
* `clang-cl`: Works with VC++ headers
* `clang`: Linux
* `AppleClang`: Mac
* GCC 10.1+: Mac, Linux

#### Known Issues

* https://github.com/microsoft/STL, issue 100
* ... TBA ...

## How To

### Setup

Simply clone and initialize submodules recursively :)

```bash
git clone https://github.com/luncliff/coroutine
pushd coroutine
  git submodule update --init --recursive
popd
```

#### Linux

You may have to install libcxx / libcxxabi to use `<experimental/coroutine>`. There is [a bash script for their installation](./scripts/install-libcxx.sh).

Of course you can just [the official guideline for building the libcxx](http://libcxx.llvm.org/docs/BuildingLibcxx.html).

```console
user@host:/.../coroutine$ sudo bash ./scripts/install-libcxx.sh release_90

llvm:
 - source  : https://github.com/llvm-mirror
 - branch  : release_90
 - install : /usr/{include, lib}
 - target:
   - libcxx
   - libcxxabi

/tmp /.../coroutine
...
```

### Build

For detailed steps, I recommend to see CI configuration files. [.travis.yml](./.travis.yml) will show how to configure with CMake and build.

```bash
mkdir -p build && pushd build
  cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/tmp
  cmake --build . --target install

  cmake .. -DBUILD_TESTING=true
  cmake --build .
  ctest --verbose --output-on-failure
popd
```

### Import

> If you want some tool support, please let me know. 
> I'm willing to learn about it.

#### CMake 3.12+

Expect there is a higher CMake project which uses this library.
The library exports 3 targets.

```cmake
cmake_minimum_required(VERSION 3.12)

find_package(coroutine CONFIG REQUIRED)
# or add_subdirectory(coroutine) if you want to be simple

target_link_libraries(main
PUBLIC
    coroutine_portable
    coroutine_system
    coroutine_net
)
```

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
