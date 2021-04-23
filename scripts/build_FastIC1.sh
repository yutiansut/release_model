#!/bin/bash
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
if [[ $# -gt 0 ]] && [[ $1 = "--force" ]]; then
    rm -rf build/FastIC1
fi

mkdir -p build/FastIC1
cd build/FastIC1
cmake -DCMAKE_BUILD_TYPE=Release ${BaskDIR} -DCMAKE_INSTALL_PREFIX=${BaskDIR}/qlib -DBUILD_FastIC1=on
make -j8
make install
cd ..
