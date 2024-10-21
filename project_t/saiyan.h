#ifndef __SAIYAN_H__
#define __SAIYAN_H__
#include <iostream>
#include <string>

//forward declare for friend declaration
class namek;

class saiyan {
private:
    // mutable int m_power{};
    int m_power {};
    std::string m_name {};
    static int s_scount;
    saiyan();
public:
    void* operator new(size_t size)
    {
        // Kiểm tra xem có đủ bộ nhớ hay không
        void* ptr = malloc(size);
        if (ptr == nullptr) {
            // Xử lý lỗi: không đủ bộ nhớ
            throw std::bad_alloc();
        }
        std::cout << "Đã cấp phát " << size << " byte bộ nhớ" << std::endl;
        return ptr;
    }

    void operator delete(void* ptr)
    {
        std::cout << "Đã giải phóng bộ nhớ" << std::endl;
        free(ptr);
    }

    // ... các thành viên khác của class
    // saiyan();
    saiyan(const saiyan&) = delete;
    friend void showPowerFriend(saiyan&);
    friend class namek;
    ~saiyan();
    void kame();
    void setPower(int power) { this->m_power = power; }
    void showPower(/*&this*/) const;
    saiyan& addPower(/*saiyan* this*/ int value)
    {
        m_power += value;
        return *this;
    }

    static saiyan createSaiyan();
    static saiyan* createSaiyanPtr();
    
    static void maxLevel();
};

void showPowerFriend(saiyan&);
#endif
