#!/bin/bash

set -xe

DEPS="./main.cpp ./utils.cpp ./tf-idf.cpp"
CFLAGS="-Wextra -Wall -ggdb --pedantic -std=c++20"
LINKS="-lpthread -ldl -lm -lsqlite3"

g++ $CFLAGS -o main $DEPS $LINKS
