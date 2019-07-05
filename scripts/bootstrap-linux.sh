#!/bin/bash

rm -rf build

cmake -Bbuild -H. -DSANITIZE_ADDRESS=Off -DCMAKE_BUILD_TYPE=Release
