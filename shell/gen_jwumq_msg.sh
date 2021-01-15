#!/bin/bash

protoc -I=../source/jwumq_protocol/ --cpp_out=../source/jwumq_protocol/ jwumq_msg.proto
mv ../source/jwumq_protocol/jwumq_msg.pb.cc ../source/jwumq_protocol/jwumq_msg.pb.cpp
