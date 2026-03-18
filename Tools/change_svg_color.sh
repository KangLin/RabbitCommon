#!/bin/bash

set -e


# 安全的 readlink 函数，兼容各种系统
safe_readlink() {
    local path="$1"
    if [ -L "$path" ]; then
        if command -v readlink >/dev/null 2>&1; then
            if readlink -f "$path" >/dev/null 2>&1; then
                readlink -f "$path"
            else
                readlink "$path"
            fi
        else
            ls -l "$path" | awk '{print $NF}'
        fi
    elif [ -e "$path" ]; then
        if command -v realpath >/dev/null 2>&1; then
            realpath "$path"
        else
            echo "$(cd "$(dirname "$path")" && pwd)/$(basename "$path")"
        fi
    else
        echo "$path"
    fi
}

TOOL_DIR=$(dirname $(safe_readlink $0))
REP_ROOT=$(dirname $TOOL_DIR)
echo "TOOL_DIR: $TOOL_DIR"
echo "REP_ROOT: $REP_ROOT"

if [ $# -lt 4 ]; then
    echo "Input parameters[$#] is fail. "
    echo "Please using:"
    echo "$0 source_path \"source_color\" destination_path \"destination_color\""
    echo ""
	echo "ag:"
	echo "  $0 $REP_ROOT/Src/Resource/icons/rabbit-green/scalable/apps \"#00ff00\" $REP_ROOT/Src/Resource/icons/rabbit-white/scalable/apps \"#ffffff\""
	echo "  $0 $REP_ROOT/Src/Resource/icons/rabbit-black/scalable/apps \"#000000\" $REP_ROOT/Src/Resource/icons/rabbit-white/scalable/apps \"#ffffff\""
	echo "  $0 $REP_ROOT/Src/Resource/icons/rabbit-red/scalable/apps \"#ff0000\" $REP_ROOT/Src/Resource/icons/rabbit-white/scalable/apps \"#ffffff\""
    exit
fi

source_path=$1
source_color=$2
source_color_upper=${source_color^^}

destination_path=$3
destination_color=$4
destination_color_upper=${destination_color^^}

#echo "source_color_upper=$source_color_upper;destination_color_upper=$destination_color_upper"

if [ ! -d $destination_path ]; then
    mkdir -p $destination_path
fi
cp -rf $source_path/* $destination_path/ 
sed -i "s/fill=\"$source_color\"/fill=\"$destination_color_upper\"/g" `grep "fill=\"$source_color\"" -rl $destination_path`
sed -i "s/fill=\"$source_color\"/fill=\"$destination_color_upper\"/g" `grep "fill=\"$source_color_upper\"" -rl $destination_path`
