#!/usr/bin/env bash
#
# Calculate the project variables and place shell lines
# on stdout to define them
# 
# project_vars -cfg <config file> -exp <exports>
#
# config file : (optional) Path and name of the PLUGIN_CONFIG file to use.
#       If this is not given (or is empty), it will try to find it by looking
#       up the directory heirarchy
#
# exports : (optional) If present, the script will generate export statements 
#       as well.

cfg_file=""
do_export=""

#set -x

while [[ "$1" ]]; do
    case "$1" in
        -cfg)
            cfg_file="$2"
            shift; shift
            ;;
        -exp)
            do_export=$2
            shift; shift
            ;;
        *)
            echo "ERROR: Unknown option '$1'"
            exit 1
    esac
done

if [[ $do_export == "" || $do_export == "ps" || $do_export == "sh" ]]; then
    true
else
    echo "ERROR: Invalid value for -exp : '$do_export'"
    exit 1
fi

# Find the config file if they didn't give it to us.
if [[  -z "$cfg_file"  ||   "X${cfg_file}Z" == "X-Z"  ]]; then
    cfg_file="PLUGIN_CONFIG"
    for file in "${cfg_file}" "../${cfg_file}"
    do
        if [ -e "$file" ]; then
            cfg_file=$file
            break
        fi
    done

    if [ ! -f "${cfg_file}" ]; then
        echo "ERROR: Could not find CONFIG file"
        exit 1
    fi
elif [ ! -f "$cfg_file" ]; then
    echo "ERROR: file '${cfg_file}' does not exist."
    exit 1
fi

if [ "$do_export" == "ps" ]; then
    export="\$env:"
elif [[ $do_export == "sh" ]]; then
    export="export "
else
    export=""
fi

if [ -z "$OS_TAG" ]; then
    uname=`uname`
    case ${uname} in
    "Darwin")
        OS_TAG="macos"
        ;;
    "Linux")
        OS_TAG="linux"
        ;;
    *)
        echo "Cannot determine the OS from '${uname}'"
        exit 1
        ;;
    esac
fi

Q=""
if [[ "$do_export" == "ps" ]]; then
    # powershell values need to surrounded in quotes
    Q='"'
fi

SEP="/"
if [[ "$OS_TAG" == "win64" ]]; then
    SEP="\\"
fi

# Output Variables from the config file
echo "${export}OS_TAG=$Q${OS_TAG}$Q";
while read -r line
do
    if [ $line == "#*" ]; then
        continue
    fi
    if [[ "$do_export" == "ps" ]]; then
        IFS="=" read -r key value <<< "$line"
        echo "${export}${key}=\"$value\""
    else 
        echo "${export}${line}"
    fi
done < ${cfg_file}

# --- Create derived variables
#
source "${cfg_file}"

PROJ_LOWER=$(echo "$SF_PROJECT" | tr '[:upper:]' '[:lower:]')

OUTPUT_STEM=${SF_PROJECT}-V${SF_VERSION}-${OS_TAG}

case "$OS_TAG" in
"macos")
    ARTIFACT_PATH="."
    BUILD_FILE=${SF_PROJECT}.vst3
    SEP="/"
    ;;
"linux")
    ARTIFACT_PATH="."
    BUILD_FILE=${SF_PROJECT}.vst3
    SEP="/"
    ;;
"win64")
    ARTIFACT_PATH="${SF_PROJECT}.vst3\\Contents\\x86_64-win"
    BUILD_FILE=${SF_PROJECT}.vst3
    SEP="\\"
    ;;
*)
    echo "ERROR: unknown OS_TAG = '$OS_TAG'"
    exit 1
    ;;
esac

IN_RUNNER=""
if [[ -n "$GITHUB_OUTPUT" ]]; then
    IN_RUNNER=1
fi

VST3_BUILD_PATH="Source${SEP}${SF_PROJECT}_artefacts${SEP}Release${SEP}VST3"

# OUTPUT derived variables
echo "${export}SF_PROJ_LOWER=$Q${PROJ_LOWER}$Q";
echo "${export}SF_ARTIFACT_PATH=$Q${ARTIFACT_PATH}$Q";
echo "${export}SF_BUILD_FILE=$Q${BUILD_FILE}$Q";
echo "${export}SF_VST3_BUILD_PATH=$Q${VST3_BUILD_PATH}$Q";
echo "${export}SF_VST3_PLUGIN_PATH=$Q${VST3_BUILD_PATH}${SEP}${ARTIFACT_PATH}${SEP}${BUILD_FILE}$Q";
echo "${export}SF_OUTPUT_STEM=$Q${OUTPUT_STEM}$Q";
echo "${export}SF_IN_RUNNER=$Q${IN_RUNNER}$Q";
