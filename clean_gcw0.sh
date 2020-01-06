#!/bin/bash

export PATH=$PATH:/opt/gcw0-toolchain/usr/bin/

make -f Makefile.gcw0 clean
rm build/rg350-a2600.dge
rm build/rg350-a2600.opk
