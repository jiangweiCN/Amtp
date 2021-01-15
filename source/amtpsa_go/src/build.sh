#!/bin/bash

# make libjwumqa_go
export GO111MODULE=off
source ~/.bash_profile
go build -o ../../../bin/libamtpsa_tester_go
go build -o ../../../bin/libamtpsa_plugin.so -buildmode=plugin amtpsa_plugin/amtpsa_plugin.go

