#!/bin/bash

set -e

if [ $# -lt 4 ]; then
    echo "Input parameters[$#] is fail. "
    echo "Please using:"
    echo "$0 source_path \"source_color\" destination_path \"destination_color\""
	echo "ag:"
	echo "$0 ../Src/Resource/icons/rabbit-green \"#00ff00\" ../Src/Resource/icons/rabbit-white \"#ffffff\""
	echo "$0 ../Src/Resource/icons/rabbit-black \"#000000\" ../Src/Resource/icons/rabbit-white \"#ffffff\""
    echo "$0 ../Src/Resource/icons/rabbit-red \"#ff0000\" ../Src/Resource/icons/rabbit-white \"#ffffff\""
    exit
fi

source_path=$1
source_color=$2
destination_path=$3
destination_color=$4

if [ ! -d $destination_path ]; then
    mkdir -p $destination_path
fi
cp -rf $source_path/* $destination_path/ 
sed -i "s/fill=\"$source_color\"/fill=\"$destination_color\"/g" `grep "fill=\"$source_color\"" -rl $destination_path`
