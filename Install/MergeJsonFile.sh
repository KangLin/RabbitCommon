#!/bin/bash

# Use to merge update json file in linux
# $1: the name of json file
# $2: the json file of version
# $3: the directory of json files

function usage()
{
    echo "Usage:"
    echo "$0 'the name of json file' 'the json file of version' 'the directory of json files'"
    exit
}

update_json_file=$1
if [ -z $update_json_file ]; then
    update_json_file="update.json"
fi
version_json_file=$2
if [ -z $version_json_file ]; then
    version_json_file="version.json"
fi
if [ ! -f $version_json_file ]; then
    usage
fi
directory_json_file=$3
if [ -z $directory_json_file ]; then
    directory_json_file=`pwd`
fi
if [ ! -d $directory_json_file ]; then
    usage
fi

echo "{" > ${update_json_file}
echo "    \"version\":" >> ${update_json_file}
cat ${version_json_file} >> ${update_json_file}
echo "    ," >> ${update_json_file}
echo "    \"files\":[" >> ${update_json_file}
cd ${directory_json_file}
index=0
for file in `ls *.json`
do
    echo $file
    if [ $file == $version_json_file ]; then
        continue
    fi
    if [ $file == $update_json_file ]; then
        continue
    fi
    if [ $index -gt 0 ]; then
        echo "," >> ${update_json_file}
    fi
    index=$(($index+1))
    cat $file >> ${update_json_file}
done
echo "    ]" >> ${update_json_file}
echo "}" >> ${update_json_file}
echo "cat >> ${update_json_file}"
cat ${update_json_file}
#rm -fr *.json
