#!/bin/env bash

C_COMPILER='clang++'
ERROR_LIMIT=5000
C_FLAGS="-Wall -Wextra -Wconversion --std=c++2b -O3 -g -ferror-limit=${ERROR_LIMIT}"

${C_COMPILER} ./main.cpp ${C_FLAGS}
errorcode=$?
install ./a.out $PREFIX/bin/matcal
exit $errorcode
