REM
REM Recommend Developer Powershell for 2019.
REM Note that this intends CI environment. You can use Command Prompt for VS 2017.
REM
REM      &.\scripts\build-vs2017-cmake-clang.bat Debug
REM
@echo off
if NOT "%1"=="" (
    set cmake_build_type="%1"
) else (
    set cmake_build_type="%BUILD_TYPE%"
)

REM The batch script emulates the 'Visual Studio Native Command Prompt'
REM If the script fails, check the location of 'vcvarsall.bat' and invoke it

REM VC++ Tools from Chocolatey package
REM     choco install  visualstudio2017-workload-vctools;
set vcvarsall_path="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
if exist %vcvarsall_path% (
    call %vcvarsall_path% x86_amd64
)

REM Visual Studio Installer
REM     Enterprise/Professional/Community
set vcvarsall_path="C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
if exist %vcvarsall_path% (
    call %vcvarsall_path% x86_amd64
)
set vcvarsall_path="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %vcvarsall_path% (
    call %vcvarsall_path% x86_amd64
)
set vcvarsall_path="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %vcvarsall_path% (
    call %vcvarsall_path% x86_amd64
)

REM Chocolatey's LLVM install path
path "C:/Program Files/LLVM/bin/";%PATH%
clang-cl --version

REM We can specify with -DCMAKE_CXX_COMPILER=clang-cl
REM But for this script we will use CXX variable
set CXX=clang-cl

md build
md install
cd build
    cmake .. -G "Ninja" ^
        -DCMAKE_INSTALL_PREFIX="./install" ^
        -DBUILD_SHARED_LIBS=ON ^
        -DCMAKE_BUILD_TYPE="%cmake_build_type%"
    cmake --build .
    cmake --build . --target install
cd ..
