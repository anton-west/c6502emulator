#!/bin/sh
make
./bin/debug/c6502 -b nestest.nes 2> error.csv