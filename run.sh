#!/bin/bash

rm -rf file_recovery *.txt
gcc -g -Wall file_recovery.c -o file_recovery
./file_recovery
