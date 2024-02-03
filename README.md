# Program Complexity LLVM Pass
This is a implementation of a LLVM analysis pass that outputs information about analyzed functions, that will later be used to help create analytical models for them. Pass is designed as a LLVM plugin pass.

All commands in instructions below are executed from root dir of this repo, unless otherwise specified.

## Prerequisites

In directory separate from this project, clone [llvm-project](https://github.com/llvm/llvm-project) repository. Set used sha to one used during development of this pass ([```86bc18ade8f335f03f607142311957129e156efc```](https://github.com/llvm/llvm-project/commit/86bc18ade8f335f03f607142311957129e156efc)).

    git clone https://github.com/llvm/llvm-project.git
    git checkout 86bc18ade8f335f03f607142311957129e156efc

Build and install llvm-project in debug build type. Best is to create separate workspace for llvm-project, as it will not be modified. Use below script:

    ROOT_DIR=./llvm-project/llvm
    BUILD_DIR=./build_llvm
    INSTALL_DIR=./llvm-install
    BUILD_TYPE=Debug

    mkdir -p $BUILD_DIR
    cmake $ROOT_DIR \
            -B $BUILD_DIR \
            -G "Ninja" \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DLLVM_ENABLE_PROJECTS="llvm;clang" \
            -DLLVM_DEFAULT_TARGET_TRIPLE="x86_64-unknown-linux-gnu"

    cmake --build $BUILD_DIR --target opt
    cmake --install $BUILD_DIR

LLVM will be installed in ```llvm-install``` dir.

## Building pass

In this projects repository, create build directory and build pass:

    mkdir build
    cd build
    CMAKE_PREFIX_PATH=<path to llvm install> cmake ..

## Using pass

To use this pass first you have to compile your code to LLVM IR form. During development of pass we assume code is compiled with O2 optimization level. Clang ```-g``` option, enabling generating source-level debug information.

    <llvm-install>/bin/clang ./test/loop/index_is_input.c -O2 -g -S -emit-llvm -o ./test/loop/index_is_input_O2.ll

To use this pass, run LLVMs opt tool with given options, with compiled LLVM IR file as input:

    <llvm-install>/bin/opt --load-pass-plugin=./build/ProgramComplexity.so --passes="print-program-complexity" --disable-output ./test/loop/index_is_input_O2.ll 2>output.json

Pass prints information about compiled functions in json form, to redirect output from pass to file use ```2> output.json```

More on running passes with opt [here](https://llvm.org/docs/WritingAnLLVMPass.html#running-a-pass-with-opt) and [here](https://llvm.org/docs/NewPassManager.html#invoking-opt).

## Debugging pass

To enable printing of debug information from pass, run opt tool with -debug option.

    <llvm-install>/bin/opt -debug

## Pass output structure

TBD

## Tests

Currently in folder test/loop, there is one example "index_is_input.c". It was compiled with optimization flag O1. In folder there are:
- compiled LLVM IR, in regular and debug mode.
- output json file, containing output from pass.
- dot graph to see visual representation of LLVM IR code.
