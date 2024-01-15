#!/bin/bash

set -e
set -u

#####################
# BREW LLVM
#####################

brewLLVMPath="/opt/homebrew/Cellar/llvm/17.0.6"
includePath="$brewLLVMPath/include"
binPath="$brewLLVMPath/bin"

function brewLLVMInstall() {
  # Download llvm via Brew
  brew install llvm@17
  brewLLVMCopyToolchain
}

function brewLLVMUninstall() {
  brew uninstall llvm@17
}

function brewLLVMClangObjCHelloWorld() {
  cd SampleIOSProject
  ../generate-compilation-db.sh xcodebuild build -project SampleIOSProject.xcodeproj -scheme SampleIOSProject -destination 'generic/platform=iOS'
}
# Demonstrates creating a program that links against LLVM tools
function brewLLVMClangCPlusPlusHelloWorld() {

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

function brewLLVMCreateSampleCompilationDatabase() {

    compilationDatabasePath="CompilationDatabase"
    if [[ -e $compilationDatabasePath ]]; then 
      rm -rf "$compilationDatabasePath"
    fi

    mkdir "$compilationDatabasePath"
    
    commands_path="compile_commands.json"
    rm -rf "$commands_path"
    
    # Add the -v flag if failures are found in the compilation database but not when building directly
    # You can then compare the resuling flags for differences (extra flags get added to the resulting clang-17 commands)
    # The -resource-dir flag was not needed when build C++ usually, but is to produce the right compilation commands.
    
    ${binPath}/clang++ -I "/opt/homebrew/opt/llvm/include" SampleCPlusPlusProject/sample.cpp -o SampleCPlusPlus.o -gen-cdb-fragment-path "$compilationDatabasePath" -resource-dir /opt/homebrew/Cellar/llvm/17.0.6/lib/clang/17

    local -a files=$(ls ${compilationDatabasePath}/*.json)

    for file in "$@"; do
        [[ -f "$file" ]] || continue

        # Fragments generated by clang have a comma before EOF
        # If a fragment is stil invalid after removing it, it should be skipped
        if sed -e '$s/,$//' "$file" | "$JQ" . > /dev/null; then
            files+=("$file")
        fi
    done

    if (( ${#files[@]} == 0 )); then
        echo "No input files found!" >&2
        exit 1
    fi
    
    sed -e '1s/^/[\'$'\n''/' -e '$s/,$/\'$'\n'']/' "${files[@]}" > "$commands_path"
    echo "$commands_path"
}

function brewLLVMCopyToolchain() {
  #rm -rf "/Users/bill/Library/Developer/Toolchains/
  mkdir -p /Users/bill/Library/Developer/Toolchains
  cp -r "$brewLLVMPath/Toolchains/LLVM17.0.6.xctoolchain" "/Users/bill/Library/Developer/Toolchains"
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
