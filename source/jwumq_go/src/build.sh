#!/bin/bash

# make libjwumqa_go
export GO111MODULE=off
source ~/.bash_profile
go build -o ../../../bin/libjwumqa_tester_go
go build -o ../../../bin/libjwumqa_plugin.so -buildmode=plugin jwumqa_plugin/jwumqa_plugin.go

