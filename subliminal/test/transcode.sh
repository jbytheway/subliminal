#!/bin/bash

set -e
set -u

if [ $# != 1 ]
then
  echo "Usage: $0 PARAMS" >&2
  exit 1
fi

PARAMS_FILE="$1"
INPUT_FILE=root_video.avi
OUTPUT_FILE="${PARAMS_FILE%.params}.subbed"

. "$PARAMS_FILE"

mencoder $MENCODER_ARGS -o "$OUTPUT_FILE" "$INPUT_FILE"

