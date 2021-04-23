#!/bin/bash
rm -rf /home/maxinxin/cache/log/FastIC1/log/*
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
./build/FastIC1/test/sim ./qlib/Release/libFastIC1.so ./src/FastIC1/md.ini ./src/FastIC1/config.ini
