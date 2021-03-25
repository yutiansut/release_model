#!/bin/bash
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}

if [[ $# -gt 0 ]] && [[ $1 = "--force" ]]; then
    rm -rf hft
fi

mkdir -p build/hft
cd build/hft
cmake -DCMAKE_BUILD_TYPE=Release ${BaskDIR} -DCMAKE_INSTALL_PREFIX=${BaskDIR}/qlib -DBUILD_hft=on
make -j8
make install
cd -
#tree ${BaskDIR}/build/hft
#du ${BaskDIR}/build/hft/qlib/Release/libhft.so
#cp ${BaskDIR}/build/hft/qlib/Release/libhft.so ./lib
