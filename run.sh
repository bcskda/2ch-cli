#!/bin/sh
mkdir -p log
rm log/*
./build/2ch-cli 2>log/main
