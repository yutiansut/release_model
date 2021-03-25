#!/bin/bash
rm -rf /home/maxinxin/cache/log/IC1/log/*
BaskDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
echo "current directory: ${BaskDIR}"
cd ${BaskDIR}
./build/IC1/test/sim ./qlib/Release/libIC1.so ./src/IC1/md.ini ./src/IC1/config.ini
