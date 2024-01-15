#!/bin/bash

set -e
set -u

apple_llvm_repo_path="/Users/bill/dev/personal/examples/apple/llvm-project"
apple_llvm_build_folder="/Users/bill/dev/personal/examples/apple/llvm-build"

brewLLVMPath="/Users/bill/Library/Developer/Toolchains/LLVM18.0.0git.xctoolchain"
includePath="$brewLLVMPath/usr/include"
binPath="$brewLLVMPath/usr/bin"

# Demonstrates creating a program that links against LLVM tools
function helloWorld() {

  executableName="program"
  rm -f "$executableName"

  flags="$($binPath/llvm-config --ldflags --libs all --system-libs)"

  "${binPath}"/clang++ BrewClang/main.cpp \
    -o $executableName -std=c++17 \
    -I "$includePath" \
    $flags \
    -lclangsema \
    -lclanganalysis \
    -lclangBasic \
    -lclangAST \
    -lclangASTMatchers \
    -lclangTooling \
    -lclangrewritefrontend \
    -lclangedit \
    -lclangparse \
    -lclangfrontend \
    -lclangbasic \
    -lclangdriver \
    -lclangast \
    -lclangastmatchers \
    -lclanglex \
    -lpthread \
    -lncurses \
    -lclang-cpp

    echo "Compilation db: Creating.."
    commandsPath="$(brewLLVMCreateSampleCompilationDatabase)"
    echo "Compilation db: Done!"

    echo "Clang Tool: Running..."
    ./$executableName SampleCPlusPlusProject/sample.cpp -p "$commandsPath"
    echo "Clang Tool: Done"
}

function appleLLVMClone() {
  git clone --depth=1 https://github.com/apple/llvm-project.git "$apple_llvm_repo_path"
}

# Added the -DDARWIN_ios_ARCHS=AArch64 -DCOMPILER_RT_ENABLE_IOS=On
# This allowed me to build on an iOS Simulator.
# The iOS device libraries are still missing though. This was progress though.
# When building loop, this one runs into an issue:
#   ld: Undefined symbols: __swift_FORCE_LOAD_$_swiftCompatibility56, referenced from:
function appleLLVMBuild6() {
  apple_llvm_build_folder="/Users/bill/dev/personal/examples/apple/llvm-build6"
  apple_cmake_path="$apple_llvm_repo_path/clang/cmake/caches/Apple-stage1.cmake"

  cd "$apple_llvm_repo_path"
  rm -rf "$apple_llvm_build_folder"
  mkdir "$apple_llvm_build_folder"
  cd "$apple_llvm_build_folder"
  cmake -G "Unix Makefiles" \
    -C "$apple_cmake_path" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=~/Library/Developer/ \
    -DLLVM_APPEND_VC_REV=on \
    -DLLVM_CREATE_XCODE_TOOLCHAIN=on \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;compiler-rt" \
    -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64" \
    -DDARWIN_ios_ARCHS=AArch64 -DCOMPILER_RT_ENABLE_IOS=On \
    "$apple_llvm_repo_path/llvm"
  make -j4
  appleLLVMCreateToolChain .
}

#The C++ libraries are not being build
#Adding libcxx to LLVM_ENABLE_RUNTIMES
#When done, try to find the "c++" binary in the toolchain and in the build directory
function appleLLVMBuild7() {
  apple_llvm_build_folder="/Users/bill/dev/personal/examples/apple/llvm-build7"
  apple_cmake_path="$apple_llvm_repo_path/clang/cmake/caches/Apple-stage1.cmake"

  cd "$apple_llvm_repo_path"
  rm -rf "$apple_llvm_build_folder"
  mkdir "$apple_llvm_build_folder"
  cd "$apple_llvm_build_folder"
  cmake -G "Unix Makefiles" \
    -C "$apple_cmake_path" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=~/Library/Developer/ \
    -DLLVM_APPEND_VC_REV=on \
    -DLLVM_CREATE_XCODE_TOOLCHAIN=on \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;compiler-rt;" \
    -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64" \
    -DDARWIN_ios_ARCHS=AArch64 -DCOMPILER_RT_ENABLE_IOS=On \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libc" \
    "$apple_llvm_repo_path/llvm"
  make -j4
  appleLLVMCreateToolChain .
}

function appleLLVMCreateToolChain() {
  buildPath="$1"

  cd "$buildPath/tools/xcode-toolchain"
  sudo make install-xcode-toolchain
}

function appleLLVMCreateSampleCompilationDatabase() {
  cd  SampleIOSProject
  ../generate-compilation-db.sh xcodebuild build -project SampleIOSProject.xcodeproj -scheme SampleIOSProject -allowProvisioningUpdates
}

# Check if the function exists
  if [ $# -gt 0 ]; then 
#if declare -f "$1" > /dev/null
  # call arguments verbatim
  "$@"
else
  # Show a helpful error
  echo "Functions Available:"
  typeset -f | awk '!/^main[ (]/ && /^[^ {}]+ *\(\)/ { gsub(/[()]/, "", $1); print $1}'
  exit 1
fi
