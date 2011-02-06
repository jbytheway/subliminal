#!/bin/bash

set -e
set -u

function echo_and_run
{
  printf "%s\n" "$*"
  "$@"
}

if [ $# != 1 ]
then
  echo "Usage: $0 PARAMS" >&2
  exit 1
fi

PARAMS_FILE="$1"
INPUT_FILE=subbed_video.avi
OUTPUT_FILE="${PARAMS_FILE%.params}.subbed"

. "$PARAMS_FILE"

echo_and_run mencoder $MENCODER_ARGS -o "$OUTPUT_FILE" "$INPUT_FILE"

