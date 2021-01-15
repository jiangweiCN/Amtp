#!/bin/bash

protoc -I=../source/amtpa_protocol/ --cpp_out=../source/amtpa_protocol/ amtpap_msg.proto
mv ../source/amtpa_protocol/amtpap_msg.pb.cc ../source/amtpa_protocol/amtpap_msg.pb.cpp
