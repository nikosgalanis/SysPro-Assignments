#!/bin/bash

cd master
make
cd ../whoServer
make
cd ../whoClient
make
cd ../worker
make
cd ..