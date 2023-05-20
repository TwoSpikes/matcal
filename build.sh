#!/bin/env bash

C_COMPILER='cc';
C_FLAGS='-Wall -Wextra -Wconversion -O3';

${C_COMPILER} main.c ${C_FLAGS};
