#!/bin/bash
rm -rf /home/maxinxin/cache/log/test_fast_factor/log/*
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
./build/test_fast_factor/test/sim ./qlib/Release/libtest_fast_factor.so ./src/test_fast_factor/md.ini ./src/test_fast_factor/config.ini
