#!/bin/bash

echo

gcc -w -o p producer.c
gcc -w -o c consumer.c

./p &
./c >> out &
