# Program Complexity LLVM Pass
This is a implementation of a LLVM analysis pass that outputs info about:
- instructions contained in basic blocks
- how many times basic blocks are executed

Pass supports analysis of:
- free basic blocks
- loops
- ~~conditions~~ (TODO)


Output of a printer pass is in json format.

Pass is designed as a plugin. There is a diff file in root of repo that shows how to integrate pass in LLVM tree.
All commands in instructions below are executed from root dir of this repo, unless otherwise specified.

## Prerequisites

Build and install [llvm-project](https://github.com/llvm/llvm-project) in dir separate from this repo:

    git clone https://github.com/llvm/llvm-project.git

    ROOT_DIR=./llvm-project/llvm
    BUILD_DIR=./build_llvm
    INSTALL_DIR=./llvm-install
    BUILD_TYPE=Release

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
    mkdir build
    cd build
    CMAKE_PREFIX_PATH=<path to llvm install> cmake ..


## Using pass

    <llvm-install>/bin/opt --load-pass-plugin=./build/ProgramComplexity.so --passes="print-program-complexity" --disable-output ./test/loop/index_and_limit_input.ll

More on running passes with opt [here](https://llvm.org/docs/NewPassManager.html#invoking-opt).