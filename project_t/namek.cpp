#include "namek.h"
#include "saiyan.h" //include heare for definition
#include <iostream>

void namek::showPowerFriend(saiyan& s) {
    std::cout << "[namek]" << "Friend power is: " << s.m_power << "\n";
}