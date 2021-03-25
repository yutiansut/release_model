#!/bin/bash
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
if [[ $# -gt 0 ]] && [[ $1 = "--force" ]]; then
    rm -rf build/IC1
fi

mkdir -p build/IC1
cd build/IC1
cmake -DCMAKE_BUILD_TYPE=Release ${BaskDIR} -DCMAKE_INSTALL_PREFIX=${BaskDIR}/qlib -DBUILD_hft=on
make -j8
make install
cd ..
