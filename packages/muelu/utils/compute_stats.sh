#!/bin/bash

HEADERS_SOURCES=$(find . -regex '.*\.\(cpp\|hpp\)')

echo "Processing sources..."
NUM_LINES=$(find . -regex '.*\.\(cpp\|hpp\)' -exec wc -l {} \; | awk '{s+=$1} END {printf "%.0f", s}')
NUM_CHARS=$(find . -regex '.*\.\(cpp\|hpp\)' -exec wc -m {} \; | awk '{s+=$1} END {printf "%.0f", s}')
NUM_BYTES=$(find . -regex '.*\.\(cpp\|hpp\)' -exec du {} \; | awk '{s+=$1} END {printf "%.0fKB", s}')
CHARS_PER_LINE=$(echo "${NUM_CHARS} / ${NUM_LINES}" | bc -l)
echo "Processing xmls..."
XML_LINES=$(find . -regex '.*\.\(xml\)' -exec wc -l {} \; | awk '{s+=$1} END {printf "%.0f", s}')
XML_CHARS=$(find . -regex '.*\.\(xml\)' -exec wc -m {} \; | awk '{s+=$1} END {printf "%.0f", s}')
XML_BYTES=$(find . -regex '.*\.\(xml\)' -exec du {} \; | awk '{s+=$1} END {printf "%.0fKB", s}')
XML_PER_LINE=$(echo "${XML_CHARS} / ${XML_LINES}" | bc -l)
echo "Processing cmake..."
CMAKE_LINES=$(find . -regex '\(.*CMakeLists\.txt\|.*\.cmake\)' -exec wc -l {} \; | awk '{s+=$1} END {printf "%.0f", s}')
CMAKE_CHARS=$(find . -regex '\(.*CMakeLists\.txt\|.*\.cmake\)' -exec wc -m {} \; | awk '{s+=$1} END {printf "%.0f", s}')
CMAKE_BYTES=$(find . -regex '\(.*CMakeLists\.txt\|.*\.cmake\)' -exec du {} \; | awk '{s+=$1} END {printf "%.0fKB", s}')
CMAKE_PER_LINE=$(echo "${CMAKE_CHARS} / ${CMAKE_LINES}" | bc -l)
echo "Processing matrices..."
MAT_LINES=$(find . -regex '.*\.\(mm\|mat\)' -exec wc -l {} \; | awk '{s+=$1} END {printf "%.0f", s}')
MAT_CHARS=$(find . -regex '.*\.\(mm\|mat\)' -exec wc -m {} \; | awk '{s+=$1} END {printf "%.0f", s}')
MAT_BYTES=$(find . -regex '.*\.\(mm\|mat\)' -exec du {} \; | awk '{s+=$1} END {printf "%.0fKB", s}')
MAT_PER_LINE=$(echo "${MAT_CHARS} / ${MAT_LINES}" | bc -l)
echo "Processing all..."
NUM_TODOS=$(grep -ri "TODO:" | wc -l)
NUM_FIXMES=$(grep -ri "FIXME:" | wc -l)

echo "---------- MueLu Stats ----------"
echo "*.cpp/*.hpp files"
echo "Number of bytes:  ${NUM_BYTES}"
echo "Number of lines:  ${NUM_LINES}"
echo "Number of chars:  ${NUM_CHARS}"
echo "Chars/line:       ${CHARS_PER_LINE}"
echo
echo "xml files"
echo "Number of bytes: ${XML_BYTES}"
echo "Number of lines: ${XML_LINES}"
echo "Number of chars: ${XML_CHARS}"
echo
echo "cmake files"
echo "Number of bytes: ${CMAKE_BYTES}"
echo "Number of lines: ${CMAKE_LINES}"
echo "Number of chars: ${CMAKE_CHARS}"
echo
echo "matrix files"
echo "Number of bytes: ${MAT_BYTES}"
echo "Number of lines: ${MAT_LINES}"
echo "Number of chars: ${MAT_CHARS}"
echo
echo "all files"
echo "Number of TODOs:  ${NUM_TODOS}"
echo "Number of FIXMEs: ${NUM_FIXMES}"
echo
