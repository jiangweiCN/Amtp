#!/bin/bash

protoc -I=../source/jwumq_external/ --cpp_out=../source/jwumq_external/ jwumq_external_message.proto
mv ../source/jwumq_external/jwumq_external_message.pb.cc ../source/jwumq_external/jwumq_external_message.pb.cpp
