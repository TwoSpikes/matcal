#!/bin/env bash

C_COMPILER='cc';
C_FLAGS='-Wall -Wextra -Wconversion';

${C_COMPILER} main.c ${C_FLAGS};
./a.out;
