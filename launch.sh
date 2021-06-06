#!/bin/bash

rm -rf build
make 

cd build 
./test_cameraServiceLaunc
