#!/bin/bash
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
if [[ $# -gt 0 ]] && [[ $1 = "--force" ]]; then
    rm -rf build/test_fast_factor
fi

mkdir -p build/test_fast_factor
cd build/test_fast_factor
cmake -DCMAKE_BUILD_TYPE=Release ${BaskDIR} -DCMAKE_INSTALL_PREFIX=${BaskDIR}/qlib -DBUILD_test_fast_factor=on
make -j8
make install
cd ..
