#!/bin/bash
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
if [[ $# -gt 0 ]] && [[ $1 = "--force" ]]; then
    rm -rf IC1
fi

mkdir -p IC1
cd IC1
cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_INSTALL_PREFIX=../qlib -DBUILD_IC1=on
make -j8
make install
cd ..
