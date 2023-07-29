#!/bin/bash

echo 
gcc -o p producer.c
gcc -o c consumer.c
./c >>output.txt &
./c >>output1.txt &
./p &
