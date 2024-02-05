#!/bin/bash

# Use to generate update json file in linux
# $1: the name of json file
# $2: the file of package
# $3: OS
# $4: ARCH
# $5: urls

function usage()
{
    echo "Usage:"
    echo "$0 'the name of json file' 'the file of package' 'operating system' 'archecture' 'url1,url2,...'"
    echo "urls separated by commas[,]"
    exit
}

if [ -z "$1" ]; then
    usage
fi

if [ -z "$2" ]; then
    usage
fi

if [ -z "$3" ]; then
    usage
fi

if [ -z "$4" ]; then
    usage
fi

if [ -z "$5" ]; then
    usage
fi

md5=`md5sum $2|awk '{print $1}'`
name=`basename $2`

urls=$5
array=(${urls//,/ })

for var in ${array[@]}
do
    if [ -z "$szUrls" ]; then
        szUrls=$var
    else
        szUrls="\"$szUrls\",\"$var\""
    fi
done   

cat > $1 << EOF
{
    "os": "$3",
    "arch": "$4",
    "md5": "${md5}",
    "name": "${name}",
    "urls": [
        $szUrls
    ]
}
EOF
