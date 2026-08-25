#!/bin/bash

# Use to generate update json file in linux

function usage_long()
{
    echo "$0 [ [-h|--help] [-v|--verbose[=0|1]] [-f|--file=<json file>] [-p|--package=<package>] [-s|--system=<system>] [--system-version=<System version>] [-a|--arch=<Archecture>] [-u|-urls=<\"url1,url2...\">]"
    echo "  --help|-h: Show help"
    echo "  -v|--verbose: Show build verbose"
    echo "  -f|--file: json file"
    echo "  -p|--package: package file"
    echo "  -s|--system: system name"
    echo "  --system-version: system version"
    echo "  -a|--arch: archecture"
    echo "  -u|--urls: urls <url1, url2, ...>"
    exit 1
}

# [如何使用getopt和getopts命令解析命令行选项和参数](https://zhuanlan.zhihu.com/p/673908518)
# [【Linux】Shell命令 getopts/getopt用法详解](https://blog.csdn.net/arpospf/article/details/103381621)
if command -V getopt >/dev/null; then
    #echo "getopt is exits"
    #echo "original parameters=[$@]"
    # -o 或 --options 选项后面是可接受的短选项，如 ab:c:: ，表示可接受的短选项为 -a -b -c ，
    # 其中 -a 选项不接参数，-b 选项后必须接参数，-c 选项的参数为可选的
    # 后面没有冒号表示没有参数。后跟有一个冒号表示有参数。跟两个冒号表示有可选参数。
    # -l 或 --long 选项后面是可接受的长选项，用逗号分开，冒号的意义同短选项。
    # -n 选项后接选项解析错误时提示的脚本名字
    OPTS=help,verbose::file:,package:,system:,system-version::,archecture:,urls:
    SHORT_OPTS=h,v::,f:,p:,s:,a:,u:
    ARGS=`getopt -o $SHORT_OPTS -l $OPTS -n $(basename $0) -- "$@"`
    if [ $? != 0 ]; then
        echo_error "exec getopt fail: $?"
        exit 1
    fi
    #echo "ARGS=[$ARGS]"
    #将规范化后的命令行参数分配至位置参数（$1,$2,......)
    eval set -- "${ARGS}"
    #echo "formatted parameters=[$@]"

    while [ $1 ]
    do
        #echo "\$1: $1"
        #echo "\$2: $2"
        case $1 in
        -v | --verbose)
            case $2 in
                "")
                    BUILD_VERBOSE=ON;;
                *)
                    BUILD_VERBOSE=$2;;
            esac
            shift 2
            ;;
        -f | --file)
            JSON_FILE=$2
            shift 2
            ;;
        -p | --package)
            FILE_PACKAGE=$2
            shift 2
            ;;
        -s | --system)
            SYSTEM=$2
            shift 2
            ;;
        --system-version)
            case $2 in
                "")
                    ;;
                *)
                    SYSTEM_VERSION=$2
                    ;;
            esac
            shift 2
            ;;
        -a | --arch)
            ARCH=$2
            shift 2
            ;;
        -u | --urls)
            urls=$2
            shift 2
            ;;
        --) # 当解析到“选项和参数“与“non-option parameters“的分隔符时终止
            shift
            break
            ;;
        -h | -help)
            usage_long
            shift
            ;;
        *)
            usage_long
            break
            ;;
        esac
    done
fi

if [ -z "$FILE_PACKAGE" -o -z "$JSON_FILE" -o -z "$urls" ]; then
    usage_long
fi

chmod a+xr $FILE_PACKAGE
md5=`md5sum $FILE_PACKAGE | awk '{print $1}'`
name=`basename $FILE_PACKAGE`

array=(${urls//,/ })

for var in ${array[@]}
do
    if [ -z "$szUrls" ]; then
        szUrls=$var
    else
        szUrls="\"$szUrls\",\"$var\""
    fi
done   

cat > $JSON_FILE << EOF
{
    "os": "$SYSTEM",
    "os_min_version": "$SYSTEM_VERSION",
    "arch": "$ARCH",
    "md5": "${md5}",
    "name": "${name}",
    "urls": [
        $szUrls
    ]
}
EOF

if [ "$BUILD_VERBOSE" = "ON" ]; then
    echo "Generate json file: ${JSON_FILE}"
    echo ""
    cat $JSON_FILE
    echo ""
fi
