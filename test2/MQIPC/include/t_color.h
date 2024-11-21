#pragma once
#include <iostream>
#include <mutex>
#include <cstdarg>
#include <vector>

namespace tcolor
{
    using color = const char*;

    constexpr color reset      { "\033[0m"    };
    constexpr color fblack     { "\033[1;30m" };
    constexpr color fred       { "\033[1;31m" };
    constexpr color fgreen     { "\033[1;32m" };
    constexpr color fyellow    { "\033[1;33m" };
    constexpr color fblue      { "\033[1;34m" };
    constexpr color fmagenta   { "\033[1;35m" };
    constexpr color fcyan      { "\033[1;36m" };
    constexpr color fwhite     { "\033[1;37m" };

    constexpr color bblack     { "\033[1;40m" };
    constexpr color bred       { "\033[1;41m" };
    constexpr color bgreen     { "\033[1;42m" };
    constexpr color byellow    { "\033[1;43m" };
    constexpr color bblue      { "\033[1;44m" };
    constexpr color bmagenta   { "\033[1;45m" };
    constexpr color bcyan      { "\033[1;46m" };
    constexpr color bwhite     { "\033[1;47m" };

    class Printer
    {
    public:
        template<typename... Args>
        static void execute(tcolor::color color, Args&&... args);
        static std::mutex _mtx;
    };
    template<typename ...Args>
    inline void Printer::execute(tcolor::color color, Args && ...args)
    {
         std::lock_guard<std::mutex> lock(_mtx);
        (std::cout << color << ... << args) << tcolor::reset << std::endl;
    }
}