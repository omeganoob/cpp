#include "saiyan.h"
#include <iostream>

#define _LOG std::cout << "[" << __FUNCTION__ << "] [" << __LINE__ << "]"

saiyan::saiyan()
{
    _LOG << std::endl;
}

saiyan::~saiyan()
{
    _LOG << std::endl;
}