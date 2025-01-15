#!/bin/bash

#default values for arguments
version=0
mode=0
num_threads=1
arg1="";
arg2="";

# Function to display usage
usage() {
    echo "Usage: $0 -v <version> [-m <mode>] [-n <num_threads>]"
    echo "  -v : Version of the executable to run (1 for Vamana Indexing or 2 for Filtered/Stitched Vamana)"
    echo "  -m : Mode ( 0: Execute Filtered & Stitched, 1:Execute Filtered , 2:Execute Stitched. Only applicable for version 2 )"
    echo "  -n : Number of threads (optional, default is 1)"
    echo "  -o : Load flag for version 2 (cannot be used with -s)"
    echo "  -s : Save flag for version 2 (cannot be used with -o)"
    exit 1
}

# Parse command-line options
while getopts "v:m:n:os" opt; do
    case $opt in
        v)
            version=$OPTARG
            if ! [[ "$version" =~ ^[12]$ ]]; then
                echo "Error: Invalid version specified. Version must be 1 or 2."
                usage
            fi
            ;;
        m)
            if [ "$version" -eq 2 ]; then
                mode=$OPTARG
            else
                echo "Error: Mode (-m) is only valid when version is 2."
                exit 1
            fi
            ;;
        n)
            if [[ "$OPTARG" =~ ^[0-9]+$ ]]; then
                num_threads=$OPTARG
            else
                echo "Error: Number of threads (-n) must be a positive integer."
                exit 1
            fi
            ;;
        o)
            if [ -n "$arg2" ]; then
                echo "Error: Cannot use both -o (load) and -s (save) at the same time."
                exit 1
            fi
            arg1="-o"
            ;;
        s)
            if [ -n "$arg1" ]; then
                echo "Error: Cannot use both -o (load) and -s (save) at the same time."
                exit 1
            fi
            arg2="-s"
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            usage
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            usage
            ;;
    esac
done






# initialize variables
a1=1.2
R1=100
L1=120
k1=100
# comment/uncomment the following line to run the project with your own thread number
# simple_Vamana_thread_num=10

#paths for project 1
base_file_path="./datasets/siftsmall/siftsmall_base.fvecs"
query_file_path="./datasets/siftsmall/siftsmall_query.fvecs"
ground_file_path="./datasets/siftsmall/siftsmall_groundtruth.ivecs"

# base_file_path="./datasets/sift/sift_base.fvecs"
# query_file_path="./datasets/sift/sift_query.fvecs"
# ground_file_path="./datasets/sift/sift_groundtruth.ivecs"

a2=1.2
k2=100

R2=32
L2=100

t=1
R_small=32
L_small=50

R_stitched=50
# variable for unfiltered queries improvement.
L_unfilitered=10

#paths for project 2
filtered_base_file_path="./datasets/dummy/dummy-data.bin"
filtered_query_file_path="./datasets/dummy/dummy-queries.bin"
filtered_ground_file_path="./datasets/dummy/dummy-ground.bin"

# filtered_base_file_path="./datasets/release1M/contest-data-release-1m.bin"
# filtered_query_file_path="./datasets/release1M/contest-queries-release-1m.bin"
# filtered_ground_file_path="./datasets/release1M/contest-ground-release-1m.bin"



# checking which version was provided to run the corresponding executable
if [ $version -eq 1 ]; then
    if [ -z ${simple_Vamana_thread_num+x} ]; then #check if user requested multi-threading
        ./build/projectJJ -a $a1 -R $R1 -L $L1 -k $k1 -b $base_file_path -q $query_file_path -g $ground_file_path
    else
        ./build/projectJJ -a $a1 -R $R1 -L $L1 -k $k1 -b $base_file_path -q $query_file_path -g $ground_file_path -n $num_threads
    fi
    exit 0
elif [ $version -eq 2 ]; then    
    ./build/projectJJ_filtered -a $a2 -R $R2 -L $L2 -k $k2 -t $t -b $filtered_base_file_path -q $filtered_query_file_path -g $filtered_ground_file_path -r $R_small -l $L_small $arg1 $arg2 -n $num_threads -m $mode -u $L_unfilitered 
    exit 0
else
    echo "Usage: $0 <version>"
    echo "Version must be 1 or 2."
    exit 1
fi