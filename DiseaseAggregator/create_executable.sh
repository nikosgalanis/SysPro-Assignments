#!/bin/bash

cd worker
make
cd ../aggregator
make
cd ..
