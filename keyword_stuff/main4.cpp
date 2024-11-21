#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iostream>

void handler(int sig)
{
    void* array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    exit(1);
}

void beee()
{
    int* foo = (int*)-1;    // make a bad pointer
    printf("%d\n", *foo);   // causes segfault
}

void bar() { beee(); }
void foo() { 
    int a = 10;
    std::cout << a << "\n";
    bar();
}

int main(int argc, char** argv) {
    
    // install our handler
    // signal(SIGSEGV, handler);   
    
    // this will call foo, bar, and beee.  beee segfaults.
    
    std::this_thread::sleep_for(std::chrono::seconds(2));

    foo();

    return 0;
}