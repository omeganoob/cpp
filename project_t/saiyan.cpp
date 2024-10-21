#include "saiyan.h"

int saiyan::s_scount{ 0 };

void saiyan::kame() { std::cout << "KAMEKAMEHAAAA\n"; }

saiyan::saiyan()
{
    std::cout << "Address saiyan: " << this << "\n";
    std::cout << "Address m_power: " << &m_power << "\n";
    std::cout << "Address m_name: " << &m_name << "\n";
    std::cout << "Address m_power + padding: " << (void*)((int64_t)&m_power + 8) << "\n";
    std::cout << "Address m_power + 1: " << &m_power + 1 << "\n";
    std::cout << "Address m_power + 2: " << &m_power + 2 << "\n";
    s_scount++;
}

saiyan::~saiyan() { s_scount++; }

void saiyan::maxLevel()
{
    std::cout << "SSJ3\n";
    // std::cout << "a: " << s_scount << "\n";
}

void saiyan::showPower() const
{
    // const int* ptr = &m_power;
    // m_power = 888;
    std::cout << "My power is: " << m_power << "\n";
}

void showPowerFriend(saiyan& s) { std::cout << "The saiyan's power is:" << s.m_power << "\n"; }

saiyan saiyan::createSaiyan() { return saiyan(); }

saiyan* saiyan::createSaiyanPtr() { return new saiyan(); }
