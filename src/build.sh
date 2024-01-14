#!/bin/bash

set -xe

g++ -Wextra -Wall -ggdb --pedantic -o main ./main.cpp ./utils.cpp
