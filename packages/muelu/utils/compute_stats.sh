#!/bin/bash

HEADERS_SOURCES=$(find . -regex '.*\.\(cpp\|hpp\)')

NUM_LINES=$(find . -regex '.*\.\(cpp\|hpp\)' -exec wc -l {} \; | awk '{s+=$1} END {printf "%.0f", s}')
NUM_CHARS=$(find . -regex '.*\.\(cpp\|hpp\)' -exec wc -m {} \; | awk '{s+=$1} END {printf "%.0f", s}')
CHARS_PER_LINE=$(echo "${NUM_CHARS} / ${NUM_LINES}" | bc -l)
NUM_TODOS=$(grep -ri "TODO:" | wc -l)
NUM_FIXMES=$(grep -ri "FIXME:" | wc -l)

echo "----- MueLu Stats -----"
echo "Number of lines:  ${NUM_LINES}"
echo "Number of chars:  ${NUM_CHARS}"
echo "Chars/line:       ${CHARS_PER_LINE}"
echo "Number of TODOs:  ${NUM_TODOS}"
echo "Number of FIXMEs: ${NUM_FIXMES}"
