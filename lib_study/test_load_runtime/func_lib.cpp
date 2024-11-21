#include <iostream>
#include <cassert>

#include <dlfcn.h>

#define math_lib_path "../../dll/bin/libmy_math_lib.so"

using pf_add = int(*)(int, int);
using pf_sub = int(*)(int, int);

class SmartLoader
{
    void* lib_handler;
    
};

int main(int, char**)
{
    void* p_mathlib = dlopen(math_lib_path, RTLD_LAZY);

    if(!p_mathlib)
    {
        std::cerr << "Can not load library: " << math_lib_path << "\n";
        return 1;
    }

    pf_add paddfunc = (pf_add) dlsym(p_mathlib, "add");

    if(!paddfunc)
    {
        std::cerr << "Can not resolve symbol: add" << " - " << math_lib_path << "\n";
        return 1;
    }

    pf_sub psubfunc = (pf_sub) dlsym(p_mathlib, "sub");
    
    if(!paddfunc)
    {
        std::cerr << "Can not resolve symbol: sub" << " - " << math_lib_path << "\n";
        return 1;
    }

    std::cout << paddfunc(3, 4) << "\n";
    std::cout << psubfunc(100, 27) << "\n";

    dlclose(p_mathlib);

    return 0;
}