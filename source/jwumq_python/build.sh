#!/bin/bash


python3 setup.py build_ext --inplace
cp -rf libjwumqa_tester.py libjwumqa.cpython-36m-x86_64-linux-gnu.so ../../bin/

