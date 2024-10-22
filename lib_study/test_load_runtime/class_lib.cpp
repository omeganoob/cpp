#include <iostream>
#include <dlfcn.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <functional>

#define dllname "../../dll/bin/libmy_dlib.so"

// using sv_constructor = std::function<void(float, float)>;

using sv_constructor = void(*)(void*, float, float);
using sv_magnitude = float(*)(const void*);
using sv_cout = std::ostream& (*)(std::ostream&, const void*);
using f_getVecSize = const int(*)();

int main(int, char**) {
    std::cout << "Hello, from test_1!\n";

    auto plibobj = dlopen(dllname, RTLD_LAZY);

    if (!plibobj)
    {
        std::cerr << "Error loading lib " << dllname << " - " << dlerror() << "\n";
        exit(EXIT_FAILURE);
    }

    dlerror();

    sv_constructor p_sv_constructor = (sv_constructor) dlsym(plibobj, "_ZN7my_dlib10scalar_vecC2Eff");
    if (!p_sv_constructor) {
        std::cerr << "Unable to find scalar_vec constructor: " << dlerror() << std::endl;
        dlclose(plibobj);
        return 1;
    }

    sv_magnitude p_sv_magnitude = (sv_magnitude) dlsym(plibobj, "_ZNK7my_dlib10scalar_vec9magnitudeEv");
    if (!p_sv_magnitude)
    {
        std::cerr << "Unable to find scalar_vec magnitude member: " << dlerror() << std::endl;
        dlclose(plibobj);
        return 1;
    }

    sv_cout p_sv_cout = (sv_cout) dlsym(plibobj, "_ZlsRSoRKN7my_dlib10scalar_vecE");
    if (!p_sv_cout)
    {
        std::cerr << "Unable to find std::cout for scalar_vec: " << dlerror() << std::endl;
        dlclose(plibobj);
        return 1;
    }

    f_getVecSize get_vec_size = (f_getVecSize) dlsym(plibobj, "getVecSize");
    if (!p_sv_cout)
    {
        std::cerr << "Unable to resolve symbol getVecSize: " << dlerror() << std::endl;
        dlclose(plibobj);
        return 1;
    }

    void* my_svec = malloc(get_vec_size());

    p_sv_constructor(my_svec, 3.5f, 4.1f);

    float mag = p_sv_magnitude(my_svec);

    p_sv_cout(std::cout, my_svec) << "\n";

    std::cout << "Magnitude: " << mag << std::endl;

    free(my_svec);
    dlclose(plibobj);

    return 0;
}
