#pragma leco add_impl ts-cpp
#pragma leco add_impl ts-java
#pragma leco add_include_dir "tree-sitter/lib/include"
#pragma leco add_include_dir "tree-sitter/lib/src"

#if __has_include(<tree_sitter/api.h>)
#include "tree-sitter/lib/src/lib.c"
#endif
