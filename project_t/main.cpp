#include "namek.h"
#include "saiyan.h"
#include <iostream>

int main(int, char**)
{
    // saiyan goku = saiyan::createSaiyan();
    // saiyan* cadic = saiyan::createSaiyanPtr();
    // namek picolo;
    // saiyan goku;
    // saiyan* gohan = new saiyan();
    // goku.addPower(100);

    saiyan goku = saiyan::createSaiyan();
    
    goku.addPower(100);
    goku.showPower();
    // const saiyan gohan;
    // gohan.showPower();
    // showPowerFriend(goku);
    // cadic->showPower();
    std::cout << "size saiyan: " << sizeof(saiyan) << "\n";
    std::cout << "size string: " << sizeof(std::string) << "\n";
    std::cout << "size int: " << sizeof(int) << "\n";
    // delete gohan;
    // picolo.showPowerFriend(goku);
}
