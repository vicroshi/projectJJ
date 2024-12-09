#!/bin/bash

# check if the version argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <version>"
    echo "Version must be 1 or 2."
    exit 1
fi

version=$1

# initialize variables
# standard variables,used in both project 1 and project 2
a=1.0
R=60
L=120
k=100

#paths for project 1
base_file_path="../datasets/siftsmall/siftsmall_base.fvecs"
query_file_path="../datasets/siftsmall/siftsmall_query.fvecs"
ground_file_path="../datasets/siftsmall/siftsmall_groundtruth.ivecs"

# variables used in project 2 exclusively
t=100
R_small=10
L_small=15
R_stitched=10

#paths for project 2
filtered_base_file_path="/home/zenki/Documents/DIT/jj/datasets/2/dummy-data.bin"
filtered_query_file_path="/home/zenki/Documents/DIT/jj/datasets/2/dummy-queries.bin"
filtered_ground_file_path="/home/zenki/Documents/DIT/jj/datasets/2/ground.bin"

# checking which version was provided to run the corresponding executable
if [ $version -eq 1 ]; then
    ./build/projectJJ -a $a -R $R -L $L -k $k -b $base_file_path -q $query_file_path -g $ground_file_path
    exit 0
else    
    ./build/projectJJ_filtered -a $a -R $R -L $L -k $k -t $t -b $filtered_base_file_path -q $filtered_query_file_path -g $filtered_ground_file_path
    exit 0
fi