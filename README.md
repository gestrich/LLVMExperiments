### LLVMExperiments

This includes some experients to build LLVM / Clang on a mac. All the experiments are in tools.h

## Random LLVM Build Notes

- On Mac M1, I hit this error when building Clang:
    - [  1%] Generating outline_atomic_helpers.dir/outline_atomic_ldadd8_2.S
CMake Error: failed to create symbolic link '/Users/bill/dev/personal/examples/apple/llvm-build6/projects/compiler-rt/lib/builtins/outline_atomic_helpers.dir/outline_atomic_ldadd8_2.S': File exists
    - I just needed to run again using `make -j4` and it finished without error :/