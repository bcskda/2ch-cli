#!/bin/sh
mkdir -p log
rm -f log/*
./2ch-cli 2>log/main
