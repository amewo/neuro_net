#!/bin/bash

protoc --cpp_out=. states.proto
mv states.pb.cc states.pb.cpp

