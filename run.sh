#!/bin/bash

# check if the version argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <version>"
    echo "Version must be 1 or 2."
    exit 1
fi

version=$1

arg1="";
arg2="";

if [ $# -ge 2 ]; then 
    if [ "$2" == "-s" ] || [ "$2" == "-o" ] ; then
        arg1=$2
    fi
fi

if [ $# -ge 3 ]; then 
    if [ "$3" == "-s" ] || [ "$3" == "-o" ] ; then
        arg2=$3
    fi
fi




# initialize variables
a1=1.2
R1=100
L1=120
k1=100
#paths for project 1
base_file_path="./datasets/siftsmall/siftsmall_base.fvecs"
query_file_path="./datasets/siftsmall/siftsmall_query.fvecs"
ground_file_path="./datasets/siftsmall/siftsmall_groundtruth.ivecs"

a2=1.0
R2=100
L2=120
k2=100
t=1
R_small=32
L_small=100

R_stitched=50


#paths for project 2
filtered_base_file_path="./datasets/2/dummy-data.bin"
filtered_query_file_path="./datasets/2/dummy-queries.bin"
filtered_ground_file_path="./datasets/test/ground.bin"

# checking which version was provided to run the corresponding executable
if [ $version -eq 1 ]; then
    ./build/projectJJ -a $a1 -R $R1 -L $L1 -k $k1 -b $base_file_path -q $query_file_path -g $ground_file_path
    exit 0
else    
    ./build/projectJJ_filtered -a $a2 -R $R2 -L $L2 -k $k2 -t $t -b $filtered_base_file_path -q $filtered_query_file_path -g $filtered_ground_file_path -r $R_small -l $L_small $arg1 $arg2
    exit 0
fi