#!/bin/bash
#
#   Author  : github.com/luncliff (luncliff@gmail.com)
#   Example
#       sudo bash install-cmake-3.sh 3.16.1
#
version=${1:-"3.16.2"}

echo ""
echo "cmake:"
echo " - version: ${version}"
echo " - source:  https://github.com/Kitware/CMake/releases"
echo ""

# render packaged folder's name. for example ...
#   cmake-3.16.1-Linux-x86_64
dirname="cmake-${version}-$(uname)-$(uname -m)"
source="https://github.com/Kitware/CMake/releases/download"

# download & extract in /tmp to keep working directory clean
uri="${source}/v${version}/${dirname}.tar.gz"
wget -q -O /tmp/cmake3.tar.gz ${uri} 

pushd /tmp
    tar -x -f cmake3.tar.gz
    rsync -ah "/tmp/${dirname}/bin"     /usr
    rsync -ah "/tmp/${dirname}/share"   /usr
popd
cmake --version
