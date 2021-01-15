#!/bin/bash

protoc -I=../source/license/ --cpp_out=../source/license/ license_msg.proto
