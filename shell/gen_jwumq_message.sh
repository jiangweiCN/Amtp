#!/bin/bash

protoc -I=../source/jwumq/ --cpp_out=../source/jwumq/ jwumq_message.proto
mv ../source/jwumq/jwumq_message.pb.cc ../source/jwumq/jwumq_message.pb.cpp
