#!/bin/bash

# assume this is run from MueLu's top level
if [ ! -f "utils/run_clang_format.sh" ] ; then
  echo "Error! run_clang_format.sh should be run from the packages/muelu directory! Aborting..."
  exit 1
fi

# check if clang-format exists on the user's machine. if not, formatting can't be performed
if [ ! -x clang-format ] ; then
  echo "Error! clang-format was not found! Please place clang-format in your PATH or current directory"
  echo
  echo "If you are looking to reformat your code due to the MueLu clang-format Github action, please use clang-format 8.0.1"
  echo 'See https://github.com/llvm/llvm-project/releases/tag/llvmorg-8.0.1 to find the appropriate clang-format version'
  echo "Linux x64 developers can find here:"
  echo '  https://github.com/llvm/llvm-project/releases/download/llvmorg-8.0.1/clang+llvm-8.0.1-x86_64-linux-gnu-ubuntu-14.04.tar.xz'
  exit 1
else
  CLANGFORMAT_VERSION=$(clang-format --version)
  # check if clang-format is version 8.0.1. if not, don't let the user continue
  if [[ ! "${CLANGFORMAT_VERSION}" =~ "8.0.1" ]] ; then
      echo "Error! clang-format is not version 8.0.1, so it will likely have major conflicts with the MueLu style!"
      echo "Detected clang-format version: ${CLANGFORMAT_VERSION}"
      exit 1
  fi
fi

HEADERS=$(find src/ -name "*.hpp")
SOURCES=$(find src/ -name "*.cpp")

echo "Running MueLu clang formatting..."

for HEADER in ${HEADERS} ; do
  echo ${HEADER}
  clang-format -style=file -i ${HEADER}
done

for SOURCE in ${SOURCES} ; do
  echo ${SOURCE}
  clang-format -style=file -i ${SOURCE}
done

echo "Done running MueLu clang formatting!"
