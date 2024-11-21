#include <iostream>
#include "carsys.h"
#include <stdexcept>
#include <exception>


constexpr void assert_that(bool statement, const char* msg)
{
    if (!statement) throw std::runtime_error{ msg };
}

void initial_speed_is_zero()
{
    MockServiceBus bus{};
    AutoBreak auto_break{bus};
    assert_that(auto_break.get_speed_mps() == 0L, "speed not equal zero");
}

void initial_sentitivity_is_five()
{
    MockServiceBus bus{};
    AutoBreak auto_break{bus};
    assert_that(auto_break.get_collision_threshold_s() == 5L, "sensitivity is not 5");
}

void sensitivity_greater_than_1()
{
    MockServiceBus bus;
    AutoBreak auto_break{bus};
    try
    {
        auto_break.set_collision_threshold_s(0.5L);
    }
    catch (const std::exception& e)
    {
        return;
    }
    assert_that(false, "no exception thown");
}

void speed_is_saved()
{
    MockServiceBus bus{};
    AutoBreak auto_break{ bus };
    bus.m_speed_update_callback(SpeedUpdate{100L});
    assert_that(100L == auto_break.get_speed_mps(), "speed not saved to 100");
    bus.m_speed_update_callback(SpeedUpdate{ 50L });
    assert_that(50L == auto_break.get_speed_mps(), "speed not saved to 50");
    bus.m_speed_update_callback(SpeedUpdate{ 0L });
    assert_that(0L == auto_break.get_speed_mps(), "speed not saved to 0");
}

void alert_when_imminent()
{
    MockServiceBus bus{};

    AutoBreak auto_break{ bus };

    auto_break.set_collision_threshold_s(10L);
    bus.m_speed_update_callback(SpeedUpdate{ 100L });
    bus.m_car_detected_callback(CarDetected{ 100L, 0L });
    assert_that(bus.m_last_cmd.time_to_collision_s == 1L, "break command published not one");
}

void alert_when_not_imminent()
{
    MockServiceBus bus{};

    AutoBreak auto_break{ bus };

    auto_break.set_collision_threshold_s(2L);
    bus.m_speed_update_callback(SpeedUpdate{ 100L });
    bus.m_car_detected_callback(CarDetected{ 1000L, 50L });
    assert_that(bus.m_command_published == 0, "break command published not one");
}

void run_test(void(*unit_test)(), const char* name)
{
    try
    {
        unit_test();
        std::cout << "[+] Test " << name << " successful.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[-] Test failure in " << name << ". " << e.what() << ".\n";
    }

}

int main(int, char**) {

    run_test(initial_speed_is_zero, "initial speed is 0");
    run_test(initial_sentitivity_is_five, "initial sensitivity is 5");
    run_test(sensitivity_greater_than_1, "sensitivity greater than 1");
    run_test(speed_is_saved, "speed is saved");
    run_test(alert_when_imminent, "alert when imminent");
    run_test(alert_when_not_imminent, "alert when not imminent");

    return 0;
}
